#include "imu_system.h"
#include "icm42688.h"
#include "icm_spi_driver.h"
#include "mahony_ahrs.h"
#include "Delay.h"
#include <stdio.h>
#include <math.h>

#define G_TO_MPS2    9.80665f
#define DPS_TO_RADPS 0.0174532925f
#define FIXED_DT      0.01f

static icm42688_t    g_imu;
static mahony_ahrs_t g_ahrs;
static float ax_buf[128], ay_buf[128], az_buf[128];
static float gx_buf[128], gy_buf[128], gz_buf[128];

static float g_gyro_z_dps = 0.0f;   /**< 最新陀螺仪Z轴角速度 (deg/s), 供循迹补偿 */

static void get_gyro_data(float *gx, float *gy, float *gz) {
    icm42688_get_agt(&g_imu);
    *gx = g_imu.gyr[0] * DPS_TO_RADPS;
    *gy = g_imu.gyr[1] * DPS_TO_RADPS;
    *gz = g_imu.gyr[2] * DPS_TO_RADPS;
}

int imu_system_init(void) {
    uint8_t reg;

    icm42688_init_hal(&g_imu, my_spi_read, my_spi_write, delay_us, SPI_0_INST);

    // 1. 软复位：DEVICE_CONFIG(0x11) = 0x01
    reg = 0x01;
    my_spi_write(SPI_0_INST, 0x11, &reg, 1);
    delay_ms(15);

    // 2. 验证 WHO_AM_I(0x75)
    my_spi_read(SPI_0_INST, 0x75, &reg, 1);
    printf("WHO_AM_I = 0x%02X\r\n", reg);
    if (reg != 0x47) {
        printf("ICM42688 init failed: wrong WHO_AM_I\r\n");
        return -1;
    }

    // 3. 使能传感器：PWR_MGMT0(0x4E) = 0x0F (gyro+accel LN mode)
    reg = 0x0F;
    my_spi_write(SPI_0_INST, 0x4E, &reg, 1);
    delay_ms(1);

    // 4. 加速度计满量程 16g：ACCEL_CONFIG0(0x50), FS_SEL[7:5]=0
    my_spi_read(SPI_0_INST, 0x50, &reg, 1);
    reg = (0 << 5) | (reg & 0x1F);
    my_spi_write(SPI_0_INST, 0x50, &reg, 1);
    g_imu.accel_scale = 16.0f / 32768.0f;

    // 5. 陀螺仪满量程 2000dps：GYRO_CONFIG0(0x4F), FS_SEL[7:5]=0
    my_spi_read(SPI_0_INST, 0x4F, &reg, 1);
    reg = (0 << 5) | (reg & 0x1F);
    my_spi_write(SPI_0_INST, 0x4F, &reg, 1);
    g_imu.gyro_scale  = 2000.0f / 32768.0f;

    // 6. 关闭滤波器：Bank1 GYRO_CONFIG_STATIC2、Bank2 ACCEL_CONFIG_STATIC2
    reg = 0x01; my_spi_write(SPI_0_INST, 0x76, &reg, 1);  // Bank = 1
    reg = 0x03; my_spi_write(SPI_0_INST, 0x0B, &reg, 1);  // NF disable + AAF disable
    reg = 0x02; my_spi_write(SPI_0_INST, 0x76, &reg, 1);  // Bank = 2
    reg = 0x01; my_spi_write(SPI_0_INST, 0x03, &reg, 1);  // AAF disable
    reg = 0x00; my_spi_write(SPI_0_INST, 0x76, &reg, 1);  // Bank = 0

    printf("ICM42688 init OK\r\n");

    // 7. 设置 ODR
    icm42688_set_accel_odr(&g_imu, ICM42688_ODR_100);
    icm42688_set_gyro_odr(&g_imu,  ICM42688_ODR_100);

    /* ── 陀螺仪预热 3 秒，等待内部振荡器稳定 ── */
    printf("Gyro warmup 3s...\r\n");
    delay_ms(3000);

    icm42688_get_agt(&g_imu);
    mahony_ahrs_init(&g_ahrs,
        g_imu.acc[0] * G_TO_MPS2,
        g_imu.acc[1] * G_TO_MPS2,
        g_imu.acc[2] * G_TO_MPS2);

    // 8. 陀螺仪标定 (静止 5 秒)
    printf("Keep still, calibrating...\r\n");
    mahony_ahrs_calibrate_gyro(&g_ahrs, get_gyro_data, delay_ms, 1000, 5);
    printf("Calibration done!\r\n");

    // 9. 使能 FIFO
    icm42688_fifo_enable(&g_imu, true, true, false);
    icm42688_fifo_stream_to_fifo(&g_imu);

    return 0;
}

int imu_system_run(void) {
    if (icm42688_fifo_read(&g_imu) <= 0) return 0;

    size_t n;
    icm42688_fifo_get_accel_x(&g_imu, &n, ax_buf);
    icm42688_fifo_get_accel_y(&g_imu, &n, ay_buf);
    icm42688_fifo_get_accel_z(&g_imu, &n, az_buf);
    icm42688_fifo_get_gyro_x(&g_imu, &n, gx_buf);
    icm42688_fifo_get_gyro_y(&g_imu, &n, gy_buf);
    icm42688_fifo_get_gyro_z(&g_imu, &n, gz_buf);

    for (size_t i = 0; i < n; i++) {
        float mag = ax_buf[i]*ax_buf[i] + ay_buf[i]*ay_buf[i] + az_buf[i]*az_buf[i];
        if (mag > 0.001f) {
            mahony_ahrs_update(&g_ahrs,
                ax_buf[i] * G_TO_MPS2, ay_buf[i] * G_TO_MPS2, az_buf[i] * G_TO_MPS2,
                gx_buf[i] * DPS_TO_RADPS, gy_buf[i] * DPS_TO_RADPS, gz_buf[i] * DPS_TO_RADPS,
                FIXED_DT);
        }
    }

    float roll, pitch, yaw;
    mahony_ahrs_get_euler(&g_ahrs, &roll, &pitch, &yaw);

    /* 存储平均 gyro_z 供外部循迹补偿使用 */
    if (n > 0) {
        float sum = 0;
        for (size_t i = 0; i < n; i++) sum += gz_buf[i];
        g_gyro_z_dps = sum / (float)n;
    }

    return (int)n;
}

void imu_system_get_euler(float *roll, float *pitch, float *yaw) {
    static float last_yaw = 0.0f;
    static int32_t yaw_turns = 0;
    float raw_yaw;

    mahony_ahrs_get_euler(&g_ahrs, roll, pitch, &raw_yaw);

    /* 解缠绕：检测 ±180° 跳变，累加圈数 */
    float diff = raw_yaw - last_yaw;
    if (diff > 180.0f) {
        yaw_turns--;
    } else if (diff < -180.0f) {
        yaw_turns++;
    }
    last_yaw = raw_yaw;

    *yaw = raw_yaw + yaw_turns * 360.0f;
}

/**
 * @brief  获取最新陀螺仪Z轴角速度 (deg/s)
 * @note   由 imu_system_run() 内部更新，无需额外调用
 */
void imu_system_get_gyro_z(float *gz) {
    *gz = g_gyro_z_dps;
}
