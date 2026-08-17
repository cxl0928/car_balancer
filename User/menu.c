/**
 * @file    menu.c
 * @brief   多级菜单系统实现 — 非阻塞函数页面
 *
 * OLED 布局（128x64, 4行×16字符, 8x16字体）:
 *   菜单模式:
 *     Line 1 : [标题               >]
 *     Line 2 : [> MenuItem1        ]
 *     Line 3 : [  MenuItem2        ]
 *     Line 4 : [  MenuItem3     v  ]
 *   函数页面模式:
 *     Line 1 : [标题               ]
 *     Line 2~3: (由函数页面自由使用)
 *     Line 4 : [Hold key2: exit    ]
 *
 * 依赖:
 *   - menu.h   (本模块头文件)
 *   - OLED.h   (OLED_Clear, OLED_ShowString, OLED_ShowChar, OLED_ShowNum)
 */

#include "menu.h"
#include "OLED.h"
#include "encoder.h"
#include "peripheral.h"
#include "pid.h"
#include "../ti_msp_dl_config.h"
#include <string.h>   /* memmove */
#include <stdio.h>

/* ==================== 全局变量 ==================== */
menu_system_t g_menu;
int EN = 0;

/* ==================== 菜单名称宏定义（修改此处即可改OLED显示文字） ==================== */
#define MENU_MAIN_TITLE    "Main"
#define MENU1_TITLE        "Menu1"
#define MENU2_TITLE        "Menu2"
#define MENU3_TITLE        "Menu3"
#define MENU1_SUB1_NAME    "Sub1"
#define MENU1_SUB2_NAME    "Sub2"
#define MENU1_SUB3_NAME    "Sub3"
#define MENU2_SUB1_NAME    "Sub1"
#define MENU2_SUB2_NAME    "Sub2"
#define MENU2_SUB3_NAME    "Sub3"
#define MENU3_SUB1_NAME    "Sub1"
#define MENU3_SUB2_NAME    "Sub2"
#define MENU3_SUB3_NAME    "Sub3"

/* ==================== 按键扫描配置 ==================== */
#define KEY_DEBOUNCE_MS    20    /* 消抖时间 (ms) */
#define KEY_LONG_PRESS_MS  300   /* 长按判定时间 (ms) */
#define KEY_SCAN_INTERVAL  10    /* 主循环调用间隔 (ms)，用于计时换算 */
#define BEEP_DURATION_MS   10    /* 按键蜂鸣器响声时长 (ms) */
#define BEEP_VOLUME        10    /* 按键蜂鸣器音量 1~100 (%) */

/* 按键状态机 */
typedef enum {
    KEY_STATE_IDLE = 0,      /* 空闲 */
    KEY_STATE_DEBOUNCE,      /* 消抖中 */
    KEY_STATE_PRESSED,       /* 已按下，等待判定 */
    KEY_STATE_LONG_PRESS,    /* 长按触发 */
} key_state_t;

static key_state_t key1_state = KEY_STATE_IDLE;
static key_state_t key2_state = KEY_STATE_IDLE;
static uint32_t    key1_timer = 0;
static uint32_t    key2_timer = 0;
static uint32_t    beep_timer = 0;   /* 蜂鸣器响声剩余时间 (ticks) */

/* ==================== 内部宏 ==================== */
#define VISIBLE_ITEMS  3       /* OLED 第2~4行可同时显示的菜单项数       */
#define CHARS_PER_LINE 16      /* OLED 每行最大字符数 (8x16字体, 128/8) */

/* ==================== 函数页面状态 ==================== */
static bool  g_func_page_active = false;   /* 函数页面是否激活 */
static void (*g_current_func)(void) = NULL;                     /* 当前函数页面的更新回调 */
static void (*g_func_key_handler)(menu_key_t key) = NULL;      /* 函数页面的按键回调 */

/* ==================== 前向声明 ==================== */
static int   menu_enter(void);
static void  menu_back(void);
static void  func_page_enter(const char *title, void (*update_func)(void),
                             void (*key_handler)(menu_key_t key));
static void  func_page_exit(void);

/* ---- 子菜单函数页面更新回调（前向声明） ---- */
static void func_menu1_sub1_update(void);
static void func_menu1_sub1_key(menu_key_t key);
static void func_menu1_sub2_update(void);
static void func_menu1_sub3_update(void);
static void func_menu2_sub1_update(void);
static void func_menu2_sub2_update(void);
static void func_menu2_sub3_update(void);
static void func_menu3_sub1_update(void);
static void func_menu3_sub2_update(void);
static void func_menu3_sub3_update(void);

/* ---- 子菜单动作回调 ---- */
static void on_menu1_sub1(void);
static void on_menu1_sub2(void);
static void on_menu1_sub3(void);
static void on_menu2_sub1(void);
static void on_menu2_sub2(void);
static void on_menu2_sub3(void);
static void on_menu3_sub1(void);
static void on_menu3_sub2(void);
static void on_menu3_sub3(void);

/* ==================== 按键扫描 ==================== */

/**
 * @brief  按键扫描任务 (非阻塞状态机, 每 10ms 调用)
 *
 *         key1: 向下 (MENU_KEY_DOWN) — 函数页面下忽略
 *         key2: 短按=确认 (MENU_KEY_ENTER)
 *               长按=返回 (MENU_KEY_BACK) — 函数页面下退出函数页面
 *
 * @note   在主循环 while(1) 中调用, 间隔约 10ms。
 *         20ms 消抖, 300ms 长按判定。
 */
void menu_key_scan_task(void)
{
    uint32_t debounce_ticks = KEY_DEBOUNCE_MS / KEY_SCAN_INTERVAL;
    uint32_t long_ticks     = KEY_LONG_PRESS_MS / KEY_SCAN_INTERVAL;

    /* ===== 蜂鸣器响声计时 ===== */
    if (beep_timer > 0) {
        beep_timer--;
        if (beep_timer == 0) {
            Buzzer(0);
        }
    }

    /* ===== key1 扫描 ===== */
    switch (key1_state) {
    case KEY_STATE_IDLE:
        if (KEY1()) {
            key1_state = KEY_STATE_DEBOUNCE;
            key1_timer = 0;
        }
        break;
    case KEY_STATE_DEBOUNCE:
        key1_timer++;
        if (key1_timer >= debounce_ticks) {
            if (KEY1()) {
                key1_state = KEY_STATE_PRESSED;
                key1_timer = 0;
            } else {
                key1_state = KEY_STATE_IDLE;
            }
        }
        break;
    case KEY_STATE_PRESSED:
        if (KEY1()) {
            key1_timer++;
            if (key1_timer >= long_ticks) {
                /* 长按蜂鸣 */
                Buzzer(BEEP_VOLUME);
                beep_timer = BEEP_DURATION_MS / KEY_SCAN_INTERVAL;
                key1_state = KEY_STATE_LONG_PRESS;
            }
        } else {
            /* 松手, 未达长按时间 → 短按 */
            Buzzer(BEEP_VOLUME);
            beep_timer = BEEP_DURATION_MS / KEY_SCAN_INTERVAL;
            // if (!g_func_page_active) {
            //     menu_input(MENU_KEY_DOWN);
            // }
            key1_state = KEY_STATE_IDLE;
        }
        break;
    case KEY_STATE_LONG_PRESS:
        if (!KEY1()) {
            key1_state = KEY_STATE_IDLE;
        }
        break;
    default:
        key1_state = KEY_STATE_IDLE;
        break;
    }

    /* ===== key2 扫描 ===== */
    switch (key2_state) {
    case KEY_STATE_IDLE:
        if (KEY2()) {
            key2_state = KEY_STATE_DEBOUNCE;
            key2_timer = 0;
        }
        break;
    case KEY_STATE_DEBOUNCE:
        key2_timer++;
        if (key2_timer >= debounce_ticks) {
            if (KEY2()) {
                key2_state = KEY_STATE_PRESSED;
                key2_timer = 0;
            } else {
                key2_state = KEY_STATE_IDLE;
            }
        }
        break;
    case KEY_STATE_PRESSED:
        if (KEY2()) {
            key2_timer++;
            if (key2_timer >= long_ticks) {
                /* 长按蜂鸣 */
                Buzzer(BEEP_VOLUME);
                beep_timer = BEEP_DURATION_MS / KEY_SCAN_INTERVAL;
                if (g_func_page_active) {
                    func_page_exit();
                } else {
                    menu_input(MENU_KEY_BACK);
                }
                key2_state = KEY_STATE_LONG_PRESS;
            }
        } else {
            /* 松手, 未达长按时间 → 短按 */
            /* 短按蜂鸣 */
            Buzzer(BEEP_VOLUME);
            beep_timer = BEEP_DURATION_MS / KEY_SCAN_INTERVAL;
            // if (!g_func_page_active) {
            //     menu_input(MENU_KEY_ENTER);
            // } else if (g_func_key_handler) {
            //     g_func_key_handler(MENU_KEY_ENTER);
            // }
            key2_state = KEY_STATE_IDLE;
        }
        break;
    case KEY_STATE_LONG_PRESS:
        if (!KEY2()) {
            key2_state = KEY_STATE_IDLE;
        }
        break;
    default:
        key2_state = KEY_STATE_IDLE;
        break;
    }
}

/* ==================== API 实现 ==================== */

/**
 * @brief  初始化菜单系统
 */
void menu_init(menu_t *root)
{
    g_menu.current_menu = root;
    g_menu.selected      = 0;
    g_menu.scroll        = 0;
    g_menu.editing       = false;
}

/**
 * @brief  渲染当前菜单到 OLED
 */
void menu_render(void)
{
    if (!g_menu.current_menu) return;

    OLED_Clear();

    uint8_t count      = g_menu.current_menu->item_count;
    uint8_t show_count = count - g_menu.scroll;
    if (show_count > VISIBLE_ITEMS) show_count = VISIBLE_ITEMS;

    /* ===== Line 1 : 标题 ===== */
    const char *title = g_menu.current_menu->title;
    char line_buf[CHARS_PER_LINE + 1];

    if (g_menu.editing) {
        int len = 0;
        const char *prefix = "EDIT:";
        while (*prefix && len < CHARS_PER_LINE) line_buf[len++] = *prefix++;
        while (*title   && len < CHARS_PER_LINE) line_buf[len++] = *title++;
        line_buf[len] = '\0';
    } else {
        int len = 0;
        while (*title && len < CHARS_PER_LINE - 1) line_buf[len++] = *title++;
        if (len < CHARS_PER_LINE) line_buf[len++] = '>';
        line_buf[len] = '\0';
    }
    OLED_ShowString(1, 1, line_buf);

    /* ===== Line 2~4 : 菜单项 ===== */
    for (uint8_t i = 0; i < show_count; i++) {
        uint8_t item_idx = g_menu.scroll + i;
        uint8_t line     = 2 + i;
        bool    is_sel   = (item_idx == g_menu.selected);

        menu_item_t *it = &g_menu.current_menu->items[item_idx];

        int pos = 0;
        line_buf[pos++] = is_sel ? '>' : ' ';

        const char *name = it->name;
        while (*name && pos < CHARS_PER_LINE) line_buf[pos++] = *name++;
        line_buf[pos] = '\0';

        OLED_ShowString(line, 1, line_buf);

        /* 值类型：在当前行右侧显示当前值 */
        if (it->type == MENU_TYPE_VALUE && it->value_ptr) {
            int val = *it->value_ptr;
            if (val < 0) {
                OLED_ShowChar(line, 14, '-');
                OLED_ShowNum(line, 15, (uint32_t)(-val), 2);
            } else {
                OLED_ShowNum(line, 14, (uint32_t)val, 3);
            }
        }
    }

    /* ===== 滚动指示符 ===== */
    if (show_count > 0 && (g_menu.scroll + show_count) < count) {
        OLED_ShowChar(4, 16, 'v');
    }
    if (g_menu.scroll > 0 && show_count > 0) {
        OLED_ShowChar(2, 16, '^');
    }
}

/* ==================== 输入处理 ==================== */

void menu_input(menu_key_t key)
{
    if (!g_menu.current_menu) return;
    if (key == MENU_KEY_NONE) return;

    uint8_t count = g_menu.current_menu->item_count;
    if (count == 0) return;

    /* ========== 值编辑模式 ========== */
    if (g_menu.editing) {
        menu_item_t *it = &g_menu.current_menu->items[g_menu.selected];
        if (it->type != MENU_TYPE_VALUE || !it->value_ptr) {
            g_menu.editing = false;
            return;
        }

        switch (key) {
        case MENU_KEY_UP:
            *it->value_ptr += it->value_step;
            if (*it->value_ptr > it->value_max) *it->value_ptr = it->value_max;
            menu_render();
            break;
        case MENU_KEY_DOWN:
            *it->value_ptr -= it->value_step;
            if (*it->value_ptr < it->value_min) *it->value_ptr = it->value_min;
            menu_render();
            break;
        case MENU_KEY_ENTER:
            g_menu.editing = false;
            menu_render();
            break;
        case MENU_KEY_BACK:
            g_menu.editing = false;
            menu_render();
            break;
        default:
            break;
        }
        return;
    }

    /* ========== 普通导航模式 ========== */
    switch (key) {
    case MENU_KEY_UP:
        if (g_menu.selected > 0) {
            g_menu.selected--;
            if (g_menu.selected < g_menu.scroll) {
                g_menu.scroll = g_menu.selected;
            }
        } else {
            g_menu.selected = count - 1;
            if (count > VISIBLE_ITEMS) {
                g_menu.scroll = count - VISIBLE_ITEMS;
            }
        }
        menu_render();
        break;

    case MENU_KEY_DOWN:
        if (g_menu.selected < count - 1) {
            g_menu.selected++;
            if (g_menu.selected >= g_menu.scroll + VISIBLE_ITEMS) {
                g_menu.scroll = g_menu.selected - VISIBLE_ITEMS + 1;
            }
        } else {
            g_menu.selected = 0;
            g_menu.scroll   = 0;
        }
        menu_render();
        break;

    case MENU_KEY_ENTER:
        {
            int ret = menu_enter();
            if (ret != 1)
                menu_render();
        }
        break;

    case MENU_KEY_BACK:
        menu_back();
        menu_render();
        break;

    default:
        break;
    }
}

/* ==================== 内部导航逻辑 ==================== */

static int menu_enter(void)
{
    menu_item_t *it = &g_menu.current_menu->items[g_menu.selected];

    switch (it->type) {
    case MENU_TYPE_SUBMENU:
        if (it->submenu) {
            g_menu.current_menu = it->submenu;
            g_menu.selected   = 0;
            g_menu.scroll     = 0;
            g_menu.editing    = false;
            return 0;
        }
        return -1;

    case MENU_TYPE_ACTION:
        if (it->action) {
            it->action();
            return 1;
        }
        return -1;

    case MENU_TYPE_VALUE:
        if (it->value_ptr) {
            if (it->action) {
                it->action();
                return 1;
            }
            g_menu.editing = true;
            return 2;
        }
        return -1;

    default:
        return -1;
    }
}

static void menu_back(void)
{
    if (!g_menu.current_menu || !g_menu.current_menu->parent) {
        return;
    }

    menu_t *parent = g_menu.current_menu->parent;
    uint8_t restore_idx = 0;

    for (uint8_t i = 0; i < parent->item_count; i++) {
        if (parent->items[i].type == MENU_TYPE_SUBMENU &&
            parent->items[i].submenu == g_menu.current_menu) {
            restore_idx = i;
            break;
        }
    }

    g_menu.current_menu = parent;
    g_menu.selected     = restore_idx;
    g_menu.editing      = false;

    if (restore_idx >= VISIBLE_ITEMS) {
        g_menu.scroll = restore_idx - VISIBLE_ITEMS + 1;
    } else {
        g_menu.scroll = 0;
    }
}

/* ==================== 菜单项 CRUD ==================== */

int menu_add_item(menu_t *menu, const menu_item_t *item)
{
    if (!menu || !item) return -1;
    menu->items[menu->item_count] = *item;
    menu->item_count++;
    return menu->item_count;
}

int menu_del_item(menu_t *menu, uint8_t index)
{
    if (!menu || index >= menu->item_count) return -1;
    if (index < menu->item_count - 1) {
        uint8_t move_count = menu->item_count - index - 1;
        memmove(&menu->items[index],
                &menu->items[index + 1],
                move_count * sizeof(menu_item_t));
    }
    menu->item_count--;
    return menu->item_count;
}

int menu_mod_item(menu_t *menu, uint8_t index, const menu_item_t *item)
{
    if (!menu || !item || index >= menu->item_count) return -1;
    menu->items[index] = *item;
    return 0;
}

/* ==================== 函数页面系统 ==================== */

/**
 * @brief  查询是否在函数页面中
 */
bool menu_is_func_page(void)
{
    return g_func_page_active;
}

/**
 * @brief  进入函数页面
 * @param  title       页面标题（显示在 OLED Line 1）
 * @param  update_func 每帧调用的非阻塞更新函数
 */
static void func_page_enter(const char *title, void (*update_func)(void),
                             void (*key_handler)(menu_key_t key))
{
    g_func_page_active  = true;
    g_current_func      = update_func;
    g_func_key_handler  = key_handler;
    OLED_Clear();
    OLED_ShowString(1, 1, (char*)title);
}

/**
 * @brief  退出函数页面，回到菜单
 */
static void func_page_exit(void)
{
    g_func_page_active  = false;
    g_current_func      = NULL;
    g_func_key_handler  = NULL;
    menu_render();
}

/**
 * @brief  函数页面非阻塞更新（主循环调用）
 *
 * @note   函数页面激活时，每帧调用 g_current_func()。
 *         子菜单的 update 函数内部必须是非阻塞的，
 *         不能包含 while(1) 或长时间 delay。
 */
void func_page_update(void)
{
    if (g_func_page_active && g_current_func) {
        g_current_func();
    }
}

/* ==================== 子菜单函数页面更新回调 ==================== */
/*
 * 每个子菜单对应一个 update 函数，由主循环每 ~10ms 调用一次。
 * 当前为空实现，用户按需填充非阻塞逻辑。
 */

static void func_menu1_sub1_update(void)
{
    /* 仅显示 EN 状态，翻转由按键触发 */
    char buf[17];
    int pos = snprintf(buf, sizeof(buf), "EN: %d", EN);
    while (pos < 16) buf[pos++] = ' ';
    buf[16] = '\0';
    OLED_ShowString(2, 1, buf);
}

static void func_menu1_sub1_key(menu_key_t key)
{
    if (key == MENU_KEY_ENTER) {
        EN ^= 1;
    }
}

static void func_menu1_sub2_update(void)
{
    static uint8_t refresh_cnt = 0;
    uint8_t result[ENCODER_CHANNEL_COUNT];
    char buf[17];
    int pos;

    if (++refresh_cnt < 10) return;  /* 每 100ms 刷新一次 */
    refresh_cnt = 0;

    Encoder_Compare(result);

    /* Line 2: 显示 8路灰度 */
    pos = snprintf(buf, sizeof(buf), "Gy:");
    for (int i = 0; i < ENCODER_CHANNEL_COUNT; i++)
        buf[pos++] = result[i] ? '1' : '0';
    while (pos < 16) buf[pos++] = ' ';
    buf[16] = '\0';
    OLED_ShowString(2, 1, buf);

    /* Line 3: 阈值 */
    OLED_ShowString(3, 1, "ch0..7 ADC thld ");
}

static void func_menu1_sub3_update(void) { /* TODO: 填充非阻塞逻辑 */ }
static void func_menu2_sub1_update(void) { /* TODO: 填充非阻塞逻辑 */ }
static void func_menu2_sub2_update(void) { /* TODO: 填充非阻塞逻辑 */ }
static void func_menu2_sub3_update(void) { /* TODO: 填充非阻塞逻辑 */ }
static void func_menu3_sub1_update(void) { /* TODO: 填充非阻塞逻辑 */ }
static void func_menu3_sub2_update(void) { /* TODO: 填充非阻塞逻辑 */ }
static void func_menu3_sub3_update(void) { /* TODO: 填充非阻塞逻辑 */ }

/* ==================== 子菜单动作回调 ==================== */
/*
 * 短按 key2 确认时触发，进入对应的函数页面。
 * 函数页面中长按 key2 退出，回到菜单。
 */

static void on_menu1_sub1(void) { func_page_enter("Menu1-Sub1", func_menu1_sub1_update, func_menu1_sub1_key); }
static void on_menu1_sub2(void) { func_page_enter("Menu1-Sub2", func_menu1_sub2_update, NULL); }
static void on_menu1_sub3(void) { func_page_enter("Menu1-Sub3", func_menu1_sub3_update, NULL); }
static void on_menu2_sub1(void) { func_page_enter("Menu2-Sub1", func_menu2_sub1_update, NULL); }
static void on_menu2_sub2(void) { func_page_enter("Menu2-Sub2", func_menu2_sub2_update, NULL); }
static void on_menu2_sub3(void) { func_page_enter("Menu2-Sub3", func_menu2_sub3_update, NULL); }
static void on_menu3_sub1(void) { func_page_enter("Menu3-Sub1", func_menu3_sub1_update, NULL); }
static void on_menu3_sub2(void) { func_page_enter("Menu3-Sub2", func_menu3_sub2_update, NULL); }
static void on_menu3_sub3(void) { func_page_enter("Menu3-Sub3", func_menu3_sub3_update, NULL); }

/* ==================== 菜单树构建 ==================== */

void OLED_menu_init(void)
{
    /* ===== Menu1 submenu ===== */
    static menu_item_t menu1_items[3];
    static menu_t menu_1 = {MENU1_TITLE, menu1_items, 0, NULL};

    /* ===== Menu2 submenu ===== */
    static menu_item_t menu2_items[3];
    static menu_t menu_2 = {MENU2_TITLE, menu2_items, 0, NULL};

    /* ===== Menu3 submenu ===== */
    static menu_item_t menu3_items[3];
    static menu_t menu_3 = {MENU3_TITLE, menu3_items, 0, NULL};

    /* ===== Main menu ===== */
    static menu_item_t main_items[3];
    static menu_t menu_main = {MENU_MAIN_TITLE, main_items, 0, NULL};

    menu_item_t it;

    /* Build Menu1 */
    it = (menu_item_t){.name=MENU1_SUB1_NAME, .type=MENU_TYPE_ACTION, .action=on_menu1_sub1};
    menu_add_item(&menu_1, &it);
    it = (menu_item_t){.name=MENU1_SUB2_NAME, .type=MENU_TYPE_ACTION, .action=on_menu1_sub2};
    menu_add_item(&menu_1, &it);
    it = (menu_item_t){.name=MENU1_SUB3_NAME, .type=MENU_TYPE_ACTION, .action=on_menu1_sub3};
    menu_add_item(&menu_1, &it);

    /* Build Menu2 */
    it = (menu_item_t){.name=MENU2_SUB1_NAME, .type=MENU_TYPE_ACTION, .action=on_menu2_sub1};
    menu_add_item(&menu_2, &it);
    it = (menu_item_t){.name=MENU2_SUB2_NAME, .type=MENU_TYPE_ACTION, .action=on_menu2_sub2};
    menu_add_item(&menu_2, &it);
    it = (menu_item_t){.name=MENU2_SUB3_NAME, .type=MENU_TYPE_ACTION, .action=on_menu2_sub3};
    menu_add_item(&menu_2, &it);

    /* Build Menu3 */
    it = (menu_item_t){.name=MENU3_SUB1_NAME, .type=MENU_TYPE_ACTION, .action=on_menu3_sub1};
    menu_add_item(&menu_3, &it);
    it = (menu_item_t){.name=MENU3_SUB2_NAME, .type=MENU_TYPE_ACTION, .action=on_menu3_sub2};
    menu_add_item(&menu_3, &it);
    it = (menu_item_t){.name=MENU3_SUB3_NAME, .type=MENU_TYPE_ACTION, .action=on_menu3_sub3};
    menu_add_item(&menu_3, &it);

    /* Build Main menu */
    it = (menu_item_t){.name=MENU1_TITLE, .type=MENU_TYPE_SUBMENU, .submenu=&menu_1};
    menu_1.parent = &menu_main;
    menu_add_item(&menu_main, &it);

    it = (menu_item_t){.name=MENU2_TITLE, .type=MENU_TYPE_SUBMENU, .submenu=&menu_2};
    menu_2.parent = &menu_main;
    menu_add_item(&menu_main, &it);

    it = (menu_item_t){.name=MENU3_TITLE, .type=MENU_TYPE_SUBMENU, .submenu=&menu_3};
    menu_3.parent = &menu_main;
    menu_add_item(&menu_main, &it);

    menu_init(&menu_main);
    menu_render();
}
