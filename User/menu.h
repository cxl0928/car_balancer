/**
 * @file    menu.h
 * @brief   多级菜单系统 — 适配 128x64 OLED (SSD1306, 4行×16字符)
 *
 * 特性:
 *  - 无限级菜单深度（每个父节点可挂任意数量子节点）
 *  - 支持子菜单、动作回调、可编辑数值三种菜单项
 *  - 滚动显示 (>4 项时自动滚动)
 *  - 值编辑模式 (上下键增减, 支持 min/max 限制)
 *  - 完全解耦输入硬件 (通过回调函数注入按键事件)
 *
 * 依赖:
 *  - OLED.h  (OLED_ShowString / OLED_ShowChar / OLED_ShowNum / OLED_Clear)
 */

#ifndef __MENU_H__
#define __MENU_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 类型定义 ==================== */

/** 菜单项类型 */
typedef enum {
    MENU_TYPE_SUBMENU = 0,  /**< 子菜单入口（需绑定下一级 menu_t）   */
    MENU_TYPE_ACTION,       /**< 执行回调函数                        */
    MENU_TYPE_VALUE,        /**< 可编辑整数值（按确认进入编辑模式）   */
} menu_item_type_t;

/* 前向声明 */
struct menu;
typedef struct menu menu_t;

/** 菜单项 */
typedef struct menu_item {
    const char     *name;           /**< 显示名称（≤13字符，留空给指示符） */
    menu_item_type_t type;          /**< 菜单项类型                        */

    /* ----- 不同类型使用的字段（union 更省 RAM，这里为清晰分开）----- */
    menu_t         *submenu;        /**< MENU_TYPE_SUBMENU: 指向子菜单    */
    void          (*action)(void);  /**< MENU_TYPE_ACTION : 回调函数指针   */

    /* ----- MENU_TYPE_VALUE 专用 ----- */
    int            *value_ptr;      /**< 指向被编辑的变量                  */
    int             value_min;      /**< 最小值                            */
    int             value_max;      /**< 最大值                            */
    int             value_step;     /**< 编辑步长                          */
} menu_item_t;

/** 菜单 */
struct menu {
    const char    *title;           /**< 菜单标题（≤15字符 + '>'指示符）  */
    menu_item_t   *items;           /**< 菜单项数组指针                    */
    uint8_t        item_count;      /**< 菜单项个数                        */
    menu_t        *parent;          /**< 父菜单（根菜单为 NULL）            */
};

/**
 * 输入事件枚举
 *
 * 用户在自己的主循环中检测按键/编码器，然后调用 menu_input()
 */
typedef enum {
    MENU_KEY_NONE  = 0,
    MENU_KEY_UP    = 1,
    MENU_KEY_DOWN  = 2,
    MENU_KEY_ENTER = 3,  /**< 短按：确认/进入 */
    MENU_KEY_BACK  = 4,  /**< 短按：返回上一级 */
} menu_key_t;

/* ==================== 全局状态（每个系统一个实例） ==================== */

typedef struct {
    menu_t         *current_menu;   /**< 当前所在菜单     */
    uint8_t         selected;       /**< 当前选中项索引   */
    uint8_t         scroll;         /**< 显示起始偏移     */
    bool            editing;        /**< 是否处于值编辑态 */
} menu_system_t;

/* ==================== 外部可访问的系统实例 ==================== */

extern menu_system_t g_menu;       /**< 全局菜单控制器 */

/* ==================== API ==================== */

/**
 * @brief  初始化菜单系统
 * @param  root  根菜单指针
 *
 * @note   内部将 g_menu 置为根菜单、选中第 0 项、清空滚动。
 *         调用后应紧接着调用 menu_render() 绘制首页。
 *
 * 示例:
 * @code
 *   menu_init(&menu_main);
 *   menu_render();
 * @endcode
 */
void menu_init(menu_t *root);

/**
 * @brief  渲染当前菜单到 OLED
 *
 * @note   自动根据 g_menu.selected / g_menu.scroll 绘制 4 行：
 *         第 0 行：标题 + ">" 指示符
 *         第 1~3 行：菜单项（带 ">" 选中标记）
 *         超过 3 个可见项时，第 3 行末尾显示 "↓" 提示
 *         编辑模式下标题闪烁 "EDIT" 前缀
 *
 *         依赖:
 *          - OLED_Clear()
 *          - OLED_ShowString(1~4, 0/1, ...)
 *          - OLED_ShowChar()
 *          - OLED_ShowNum()
 */
void menu_render(void);

/**
 * @brief  处理输入事件
 * @param  key  按键事件 (MENU_KEY_UP / DOWN / ENTER / BACK)
 *
 * @note   在 while(1) 主循环中周期性调用。
 *         用户自行轮询硬件按键/编码器，转换成 menu_key_t 后传入。
 *
 *         内部逻辑:
 *          - 普通模式:
 *             UP/DOWN : 移动光标（触发滚动）
 *             ENTER   : 进入子菜单 / 执行回调 / 进入值编辑
 *             BACK    : 返回父菜单
 *          - 编辑模式 (MENU_TYPE_VALUE 按下 ENTER 后):
 *             UP      : 值 +value_step（受 value_max 限制）
 *             DOWN    : 值 -value_step（受 value_min 限制）
 *             ENTER   : 确认并退出编辑
 *             BACK    : 取消编辑（恢复原值）
 */
void menu_input(menu_key_t key);

/**
 * @brief  向菜单末尾追加一个菜单项
 * @param  menu  目标菜单指针
 * @param  item  要追加的菜单项（值拷贝，非指针）
 *
 * @note   需要用户自行保证 menu->items 指向的数组有足够容量。
 *         此函数仅在已有数组尾端追加，不动态分配。
 *
 * @return 追加后的总项数，-1 表示失败
 */
int  menu_add_item(menu_t *menu, const menu_item_t *item);

/**
 * @brief  删除指定索引的菜单项（后续项前移）
 * @param  menu  目标菜单指针
 * @param  index 要删除的索引 (0-based)
 *
 * @note   后续项整体前移，item_count 减 1。
 *
 * @return 删除后的总项数，-1 表示越界
 */
int  menu_del_item(menu_t *menu, uint8_t index);

/**
 * @brief  修改指定索引的菜单项
 * @param  menu   目标菜单指针
 * @param  index  要修改的索引
 * @param  item   新的菜单项内容
 *
 * @note   仅覆盖 name / type / submenu / action / value_* 字段。
 *
 * @return 0 成功，-1 越界
 */
int  menu_mod_item(menu_t *menu, uint8_t index, const menu_item_t *item);

/**
 * @brief  按键扫描函数（封装硬件按键读取和长按检测）
 *
 * key1: 向下 (MENU_KEY_DOWN)
 * key2: 短按=确认 (MENU_KEY_ENTER), 长按=返回 (MENU_KEY_BACK)
 *
 * @note   在主循环 while(1) 中调用即可。
 *         函数页面激活时自动切换为"长按退出"模式。
 */
void menu_key_scan_task(void);  /**< 非阻塞按键扫描任务 (每 10ms) */

/**
 * @brief  查询是否在函数页面中
 * @return true=函数页面激活, false=菜单模式
 */
bool menu_is_func_page(void);

/**
 * @brief  函数页面非阻塞更新（主循环调用，不做重绘）
 */
void func_page_update(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OLED_menu_init();
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif /* __MENU_H__ */
