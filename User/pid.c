/**
 * @file    pid.c
 * @brief   双环循迹 PID: 红外外环(位置式) → turn → 速度内环(增量式) → PWM
 *          TIMER_0 ISR 每 10ms: 编码器测速 + IR误差 + 双环PID
 */

#include "User.h"

/* ========================================================================
 *  模式1高速 — 循迹 PID 参数 (位置式)
 * ======================================================================== */
#define TRACK_KP        2.05f
#define TRACK_KI        0.00f
#define TRACK_KD        8.0f
#define TRACK_I_MAX     200.0f
#define TRACK_OUT_MAX   500.0f

#define TRACK_FORWARD   50.0f      /**< 直道基础速度 */

/* ========================================================================
 *  内环 — 速度 PID 参数 (增量式, 不动)
 * ======================================================================= */
#define SPD_KP          10.2f
#define SPD_KI          2.0f
#define SPD_KD          0.0f
#define SPD_I_MAX       500.0f
#define SPD_OUT_MAX     1000.0f

/* ── 模式2: 低速循迹 PID 参数 ── */
#define TRACK2_KP       1.6f
#define TRACK2_KI       0.00f
#define TRACK2_KD       4.0f
#define TRACK2_I_MAX    200.0f
#define TRACK2_OUT_MAX  500.0f
#define TRACK_FORWARD2  40.0f

/* ── 模式3: 定时停车 ── */
#define TIMED_TICKS     3800    /**< 5s @ 2ms 定时器周期 */

/* ── 斜坡缓启动 ── */
#define RAMP_STEP       0.001f   /**< 每周期(2ms)速度增长量 */
#define RAMP_MULT_START 0.1f    /**< 启动时的速度倍率 */

#define TRACK_CHANNELS  8

 
/* ========================================================================
 *  全局 PID 实例
 * ======================================================================== */
typedef struct {
    float Kp, Ki, Kd, imax, omax;
    float setpoint, err, err1, err2, out;
} PID_t;

typedef struct {
    float Kp, Ki, Kd, imax, omax;
    float setpoint, err, err1, sum, out;
} PID_Pos_t;

static PID_t     g_m1, g_m4;          /* 内环: 速度PID M1 + M4 */
static PID_Pos_t g_pid_track;         /* 外环: 循迹PID 模式1 */
static PID_Pos_t g_pid_track2;        /* 外环: 循迹PID 模式2 */
static volatile bool    g_running = false;
static volatile uint8_t g_pid_mode = 0;      /* 0=未选, 1=高速50, 2=低速30, 3=定时5s */
static float     g_lastValidErr = 0.0f;
static uint16_t  g_timed_ticks = 0;   /* 模式3 计时器 */
static float     g_ramp_scale  = 0.1f; /* 缓启动倍率 (0.1 → 1.0) */

volatile bool g_full_black_stop = false; /**< ISR全黑停车标志, 主循环处理蜂鸣 */

/** ISR 内非阻塞发送 '0' 计数器: 每2ms灌一个到TX FIFO, 确保32收到 */
#define STOP_NOTIFY_CNT_MAX    150     /* 150×2ms=300ms窗口 */
volatile uint16_t g_stop_notify_cnt = 0;

/* ========================================================================
 *  TIMER_0 中断 — 编码器测速 + 双环PID + 停车通知灌'0'
 * ======================================================================== */
void TIMER_0_INST_IRQHandler(void)
{
    for (uint8_t i = 0; i < ENCODER_COUNT; i++) {
        int32_t cur    = g_qencCount[i];
        g_qencSpeed[i] = cur - g_lastCount[i];
        g_lastCount[i] = cur;
    }
    if (g_pid_mode == 1)      SpeedPID_Task1();
    else if (g_pid_mode == 2) SpeedPID_Task2();
    else if (g_pid_mode == 3) SpeedPID_Task3();

    /* ── 停车通知: ISR内非阻塞往TX FIFO灌'0', 不依赖主循环 ── */
    if (g_stop_notify_cnt > 0) {
        /* TX FIFO 未满 (STAT[4] TXFF=0) 则写入, 满了就等下一轮ISR */
        if ((UART_0_INST->STAT & 0x10U) == 0U) {
            UART_0_INST->TXDATA = '0';
            g_stop_notify_cnt--;
        }
    }

    DL_TimerG_clearInterruptStatus(TIMER_0_INST,
        DL_TIMERG_INTERRUPT_ZERO_EVENT);
}

/* ── 内环 增量PID ── */
static void pid_init(PID_t *p, float kp, float ki, float kd, float imax, float omax)
{
    p->Kp = kp; p->Ki = ki; p->Kd = kd; p->imax = imax; p->omax = omax;
    p->setpoint = 0; p->out = 0;
    p->err = 0; p->err1 = 0; p->err2 = 0;
}

static void pid_clear(PID_t *p) {
    p->err = 0; p->err1 = 0; p->err2 = 0; p->out = 0;
}

static float pid_update(PID_t *p, float meas)
{
    p->err2 = p->err1;
    p->err1 = p->err;
    p->err  = p->setpoint - meas;

    float inc = p->Kp * (p->err - p->err1)
              + p->Ki * p->err
              + p->Kd * (p->err - 2.0f*p->err1 + p->err2);

    p->out += inc;
    if (p->out >  p->omax) p->out =  p->omax;
    if (p->out < -p->omax) p->out = -p->omax;
    return p->out;
}

/* ── 外环 位置PID ── */
static void pidp_init(PID_Pos_t *p, float kp, float ki, float kd, float imax, float omax)
{
    p->Kp = kp; p->Ki = ki; p->Kd = kd; p->imax = imax; p->omax = omax;
    p->setpoint = 0; p->err = 0; p->err1 = 0; p->sum = 0; p->out = 0;
}

static float pidp_update(PID_Pos_t *p, float meas)
{
    p->err1 = p->err;
    p->err  = p->setpoint - meas;

    p->sum += p->err;
    if (p->sum >  p->imax) p->sum =  p->imax;
    if (p->sum < -p->imax) p->sum = -p->imax;

    p->out = p->Kp * p->err + p->Ki * p->sum + p->Kd * (p->err - p->err1);

    if (p->out >  p->omax) p->out =  p->omax;
    if (p->out < -p->omax) p->out = -p->omax;
    return p->out;
}

/* ── 循迹误差 (加权平均, 8通道) ── */
static float track_calc_error(uint8_t result[TRACK_CHANNELS])
{
    float sum_pos = 0.0f, sum_val = 0.0f;

    for (uint8_t i = 0; i < TRACK_CHANNELS; i++) {
        if (result[i] == 0) { sum_pos += (float)i; sum_val += 1.0f; }
    }

    /* 全白(11111111)丢线 → 保持上次方向, 不猛转 */
    if (sum_val == 0.0f) {
        return g_lastValidErr;
    }

    /* ── 检测到≥5个黑线 → 立即停车 + 启动ISR灌'0'通知 ── */
    if (sum_val >= 5.0f) {
        SpeedPID_Stop();
        g_stop_notify_cnt = STOP_NOTIFY_CNT_MAX;
        g_full_black_stop = true;   /* 主循环响蜂鸣 */
        return g_lastValidErr;
    }

    float center = sum_pos / sum_val;
    float err    = center - 3.5f;
    g_lastValidErr = err;
    return err;
}

/* ========================================================================
 *  API
 * ======================================================================== */
void SpeedPID_Init(void)
{
    pid_init(&g_m1, SPD_KP, SPD_KI, SPD_KD, SPD_I_MAX, SPD_OUT_MAX);
    pid_init(&g_m4, SPD_KP, SPD_KI, SPD_KD, SPD_I_MAX, SPD_OUT_MAX);
    pidp_init(&g_pid_track,  TRACK_KP,  TRACK_KI,  TRACK_KD,  TRACK_I_MAX,  TRACK_OUT_MAX);
    pidp_init(&g_pid_track2, TRACK2_KP, TRACK2_KI, TRACK2_KD, TRACK2_I_MAX, TRACK2_OUT_MAX);
}

void SpeedPID_SetTarget(int32_t spd_m1, int32_t spd_m2)
{
    g_m1.setpoint = (float)spd_m1;
    g_m4.setpoint = (float)spd_m2;
}

void SpeedPID_Start1(void)
{
    send_Byte('9'); send_Byte('9'); send_Byte('9');
    pid_clear(&g_m1); pid_clear(&g_m4);
    g_pid_track.err = 0; g_pid_track.err1 = 0; g_pid_track.sum = 0; g_pid_track.out = 0;
    g_lastValidErr = 0.0f;
    g_timed_ticks = 0;
    g_ramp_scale = RAMP_MULT_START;
    g_pid_mode = 1;
    g_running = true;
    Buzzer(50); delay_ms(80); Buzzer(0);
}

void SpeedPID_Start2(void)
{
    send_Byte('9'); send_Byte('9'); send_Byte('9');
    pid_clear(&g_m1); pid_clear(&g_m4);
    g_pid_track2.err = 0; g_pid_track2.err1 = 0; g_pid_track2.sum = 0; g_pid_track2.out = 0;
    g_lastValidErr = 0.0f;
    g_timed_ticks = 0;
    g_ramp_scale = RAMP_MULT_START;
    g_pid_mode = 2;
    g_running = true;
    Buzzer(50); delay_ms(80); Buzzer(0);
}

void SpeedPID_Stop(void)
{
    g_running = false;
    g_pid_mode = 0;
    PWM(1, 0); PWM(4, 0);
}

/** 主循环中调用(按键停车), 启动ISR灌'0' + 蜂鸣 */
void SpeedPID_Stop_Notify(void)
{
    g_stop_notify_cnt = STOP_NOTIFY_CNT_MAX;
    SpeedPID_Stop();
    Buzzer(50); delay_ms(80); Buzzer(0);
}

void SpeedPID_Start3(void)
{
    send_Byte('9'); send_Byte('9'); send_Byte('9');
    pid_clear(&g_m1); pid_clear(&g_m4);
    g_pid_track2.err = 0; g_pid_track2.err1 = 0; g_pid_track2.sum = 0; g_pid_track2.out = 0;
    g_lastValidErr = 0.0f;
    g_timed_ticks = 0;
    g_ramp_scale = RAMP_MULT_START;
    g_pid_mode = 3;
    g_running = true;
    Buzzer(50); delay_ms(80); Buzzer(0);
}

bool SpeedPID_IsRunning(void) { return g_running; }
uint8_t SpeedPID_GetMode(void) { return g_pid_mode; }

/* ── 内部: 通用循迹任务 ── */
static void SpeedPID_Task_Internal(PID_Pos_t *track_pid, float forward_speed)
{
    if (!g_running) return;

    /* ── 斜坡缓启动: 每周期递增, 直到 1.0 ── */
    if (g_ramp_scale < 1.0f) {
        g_ramp_scale += RAMP_STEP;
        if (g_ramp_scale > 1.0f) g_ramp_scale = 1.0f;
    }
    forward_speed *= g_ramp_scale;

    /* ── 1. 红外读取 ── */
    uint8_t ir[TRACK_CHANNELS];
    Encoder_Compare(ir);

    /* ── 2. 循迹PID → turn ── */
    float err  = track_calc_error(ir);
    if (!g_running) return;
    float turn = pidp_update(track_pid, err);

    /* ── 3. 内环速度目标 (差速转向) ── */
    float fwd = forward_speed;
    float abs_err = (err > 0.0f) ? err : -err;
    if (abs_err > 1.0f)       fwd = forward_speed * 0.6f;
    else if (abs_err > 0.5f)  fwd = forward_speed * 0.8f;

    g_m1.setpoint = fwd - turn;
    g_m4.setpoint = fwd + turn;

    /* ── 4. 速度PID → PWM ── */
    int32_t s0 = QENC_GetSpeed(0);
    int32_t s3 = QENC_GetSpeed(3);

    float out1 = pid_update(&g_m1, (float)s0);
    float out4 = pid_update(&g_m4, (float)s3);

    PWM(1, (int16_t)out1);
    PWM(4, (int16_t)out4);
}

/* ── 模式1: 高速50 ── */
void SpeedPID_Task1(void) { SpeedPID_Task_Internal(&g_pid_track, TRACK_FORWARD); }

/* ── 模式2: 低速30 ── */
void SpeedPID_Task2(void) { SpeedPID_Task_Internal(&g_pid_track2, TRACK_FORWARD2); }

/* ── 模式3: 低速定时5s自动停 ── */
void SpeedPID_Task3(void)
{
    g_timed_ticks++;
    if (g_timed_ticks >= TIMED_TICKS) {
        SpeedPID_Stop();
        g_stop_notify_cnt = STOP_NOTIFY_CNT_MAX;
        g_full_black_stop = true;   /* 主循环响蜂鸣 */
        return;
    }
    SpeedPID_Task_Internal(&g_pid_track2, TRACK_FORWARD2);
}