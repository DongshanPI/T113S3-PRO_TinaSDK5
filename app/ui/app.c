#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pthread.h>

#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>

#include "app_player.h"
#include "folder_list.h"
#include "video_list.h"
#include "app_data.h"

#include "app.h"


#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl/lvgl.h"
#endif

static lv_style_t style_scr_act;
static lv_obj_t* screen_base;
static lv_obj_t *background;
static lv_obj_t *video_list_cont;
static lv_obj_t *video_title;
static lv_obj_t *video_list;
static lv_obj_t *setting_list_cont;
static lv_obj_t *setting_list;
static lv_obj_t *show_video_list_btn;
static lv_obj_t *show_setting_list_btn;
static lv_obj_t * play_loop_sw;

static pthread_t player_thread = 0;

#define DOUBLE_CLICK_TIME_MS (500)

LV_IMG_DECLARE(video_border_1024_600)


// 全局控制变量
static pthread_t g_tplayer_thread;
static volatile bool g_running = false;   // 标记是否正在运行

static void lv_update_video_list(void);
static void setting_event_cb(lv_event_t * e);

static char* get_next_video(void) {
    // 查找当前选中的文件夹
    folder_list_t* folder_list = (folder_list_t*)app_data_get("folder_list");
    folder_node_t* selected_folder = find_selected_folder(folder_list);
    if (selected_folder == NULL) {
        printf("Error: No folder is selected in folder_list.\n");
        return;
    }

    // 查找当前在播放的视频
    video_list_t* video_list = selected_folder->folder.video_list;
    if (video_list == NULL || video_list->head == NULL) {
        printf("Error: The video list is empty.\n");
        return;
    }

    video_node_t *current = video_list->head;
    video_node_t *playing_video_node = NULL;

    // 找到当前正在播放的视频
    while (current != NULL) {
        if (current->video.is_playing) {
            playing_video_node = current;
            break;
        }
        current = current->next;
    }

    // 如果找到正在播放的视频，则将其状态置为未播放
    if (playing_video_node != NULL) {
        playing_video_node->video.is_playing = 0;
        current = playing_video_node->next; // 尝试获取下一个视频
    } else {
        // 如果没有正在播放的视频，从头开始
        current = video_list->head;
    }

    // 获取下一个视频（循环列表）
    if (current == NULL) {
        current = video_list->head; // 如果当前是最后一个视频，则回到第一个视频
    }

    if (current != NULL) {
        // 设置下一个视频为正在播放
        current->video.is_playing = 1;
        return current->video.file_path;
    } else {
        // 视频列表为空的情况
        printf("Error: No video to play.\n");
        return NULL;
    }
}


// tplayer_main 的线程包装函数
void* tplayer_thread_func(void* arg)
{
    char* file_path = (char*)arg;

    printf("Starting tplayer_main with file: %s\n", file_path);

    g_running = true;

PLAY_LOOP:
    // 执行播放逻辑，直到播放完毕或被取消
    tplayer_main(file_path);
    if(1 == (int)app_data_get("list_loop_play")){
        file_path = get_next_video();
        if (NULL == file_path)
        {
            app_data_set("playing_file_path", "null");
        }else{
            printf("Next video selected: %s\n", file_path);
            app_data_set("playing_file_path", file_path);
        }        
        goto PLAY_LOOP;
    }

    g_running = false;

    printf("tplayer_main exited.\n");
    return NULL;
}

// 启动 tplayer_main
void start_tplayer(const char* file_path)
{
    if (g_running)
    {
        printf("TPlayer is already running. Please stop it first.\n");
        return;
    }

    // 创建新线程运行 tplayer_main
    if (pthread_create(&g_tplayer_thread, NULL, tplayer_thread_func, (void*)file_path) != 0)
    {
        perror("Failed to create tplayer thread");
        return;
    }

    printf("TPlayer started.\n");
}

// 停止 tplayer_main
void stop_tplayer(void)
{
    if (!g_running)
    {
        printf("TPlayer is not running.\n");
        return;
    }

    printf("Stopping TPlayer...\n");

    app_tplayer_exit();

    while (g_running)
    {
        usleep(10*1000);
    }

    printf("TPlayer stopped.\n");
}

// 点击事件回调函数
static void list_item_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        // 获取按钮的文本并打印
        const char *btn_text = lv_list_get_btn_text(video_list, obj);
        int menu_level = (int)app_data_get("menu_level");

        if (0 == menu_level){
            // 查找某个目录
            folder_list_t* folder_list = (folder_list_t*)app_data_get("folder_list");
            folder_node_t *found_video = find_folder_by_name(folder_list, btn_text);

            if (found_video != NULL) {
                printf("select folder: %s, Path: %s\n", found_video->folder.dir_name, found_video->folder.full_path);

                select_folder(folder_list, found_video->folder.dir_name);

                menu_level++;
                app_data_set("menu_level", menu_level);

                app_data_update();

                lv_update_video_list();
                // lv_obj_add_flag(video_list_cont, LV_OBJ_FLAG_HIDDEN);
            } else {
                printf("folder %s not found\n", btn_text);
            }
        }else if (1 == menu_level){

            // 查找当前选中的文件夹
            folder_list_t* folder_list = (folder_list_t*)app_data_get("folder_list");
            folder_node_t* selected_folder = find_selected_folder(folder_list);
            if (selected_folder == NULL) {
                printf("Error: No folder is selected in folder_list.\n");
                return;
            }

            // 查找某个视频
            video_list_t* video_list = selected_folder->folder.video_list;
            video_info_t *found_video = find_video_by_name(video_list, btn_text);
            
            video_info_t *playing_video = find_playing_video(video_list);

            if (found_video != NULL) {
                printf("select video: %s, Path: %s\n", found_video->name, found_video->file_path);

                stop_tplayer();

                if (playing_video)
                {
                    playing_video->is_playing = 0;
                }

                found_video->is_playing = 1;
                app_data_set("playing_file_path", found_video->file_path);

                start_tplayer(found_video->file_path);

                lv_obj_add_flag(video_list_cont, LV_OBJ_FLAG_HIDDEN);
            } else {
                printf("Video %s not found\n", btn_text);
            }
        }
    }
}

static void back_btn_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * parent_cont = lv_obj_get_parent(btn);

    int menu_level = (int)app_data_get("menu_level");

    if (menu_level <= 0)
    {
        lv_obj_add_flag(parent_cont, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    menu_level--;
    app_data_set("menu_level", menu_level);

    lv_update_video_list();
}

static void lv_create_video_list(lv_obj_t * parent) {
    // 创建 video_list_cont 组件
    video_list_cont = lv_obj_create(parent);
    lv_obj_set_size(video_list_cont, 400, 600); // 根据需要调整大小
    lv_obj_align(video_list_cont, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_flag(video_list_cont, LV_OBJ_FLAG_HIDDEN); // 初始时隐藏

    // 创建返回按钮
    lv_obj_t * back_btn = lv_btn_create(video_list_cont);
    lv_obj_set_size(back_btn, 60, 50);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 0, -10);
    lv_obj_t * back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT); 
    lv_obj_center(back_label);

    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL); // 请根据实际需要添加事件

    // 创建标题
    video_title = lv_label_create(video_list_cont);
    lv_obj_set_style_text_font(video_title, &lv_font_montserrat_30, 0);
    lv_label_set_text(video_title, "Video Title");

    lv_obj_align(video_title, LV_ALIGN_TOP_MID, 0, 0);
    
    video_list = lv_list_create(video_list_cont);
    lv_obj_set_size(video_list, lv_pct(100), lv_pct(100)); // 填满 video_list 容器
    lv_obj_align(video_list, LV_ALIGN_CENTER, 0, 50);      // 居中对齐
}

static void lv_update_video_list(void) {

    int menu_level = (int)app_data_get("menu_level");
    char* label_icon = LV_SYMBOL_DIRECTORY;

    folder_list_t* folder_list = (folder_list_t*)app_data_get("folder_list"); // 查找当前选中的文件夹
    if (folder_list == NULL) {
        printf("Error: folder_list is NULL.\n");
        return;
    }

    // 更新标题
    folder_node_t* selected_folder = find_selected_folder(folder_list);
    if (selected_folder == NULL || 0 == menu_level) {
        lv_label_set_text(video_title, "/");
    }else{
        lv_label_set_text(video_title, selected_folder->folder.dir_name);
    }

    // 清空之前的按钮和解绑之前按钮绑定的回调
    lv_obj_clean(video_list);  // 清空video_list中的所有控件

    if (0 == menu_level)
    {
        folder_node_t * current = folder_list->head;

        char label_text[64]; // 用于保存按钮的文本

        // 循环遍历链表，添加视频项到列表
        while (current != NULL) {
            snprintf(label_text, sizeof(label_text), "%s", current->folder.dir_name);
            
            // 创建按钮
            lv_obj_t * btn = lv_list_add_btn(video_list, label_icon, label_text);
            if (btn == NULL) {
                printf("Failed to create button for video: %s\n", label_text);
                break;  // 创建失败，退出循环
            }
            lv_obj_set_height(btn, 50);
            lv_obj_set_style_text_font(btn, &lv_font_montserrat_24, 0);

            // 如果视频正在播放，设置背景颜色为蓝色
            if (current->folder.is_select) {
                lv_obj_set_style_bg_color(btn, lv_color_hex(0x0000FF), 0);  // 蓝色背景
                lv_obj_set_style_bg_opa(btn, LV_OPA_10, 0); // 设置完全覆盖
            }

            // 将当前节点的地址作为用户数据传递
            lv_obj_add_event_cb(btn, list_item_event_handler, LV_EVENT_CLICKED, current);

            // 移动到下一个节点
            current = current->next;
        }
    }

    else if (1 == menu_level){
        label_icon = LV_SYMBOL_VIDEO;

        folder_node_t* selected_folder = find_selected_folder(folder_list);
        if (selected_folder == NULL) {
            printf("Error: No folder is selected in folder_list.\n");
            return;
        }

        // 检查 video_list 是否存在
        if (selected_folder->folder.video_list == NULL) {
            printf("Error: Selected folder's video_list is NULL.\n");
            return;
        }

        video_node_t *current = selected_folder->folder.video_list->head;
        char label_text[64]; // 用于保存按钮的文本

        // 循环遍历链表，添加视频项到列表
        while (current != NULL) {
            snprintf(label_text, sizeof(label_text), "%s", current->video.name);  // 使用视频名称填充文本
            
            // 创建按钮
            lv_obj_t * btn = lv_list_add_btn(video_list, label_icon, label_text);
            if (btn == NULL) {
                printf("Failed to create button for video: %s\n", label_text);
                break;  // 创建失败，退出循环
            }
            lv_obj_set_height(btn, 50);
            lv_obj_set_style_text_font(btn, &lv_font_montserrat_24, 0);

            // 如果视频正在播放，设置背景颜色为蓝色
            if (current->video.is_playing) {

                lv_obj_set_style_bg_color(btn, lv_color_hex(0x0000FF), 0);  // 蓝色背景
                lv_obj_set_style_bg_opa(btn, LV_OPA_50, 0); // 设置完全覆盖
            }

            // 将当前节点的地址作为用户数据传递
            lv_obj_add_event_cb(btn, list_item_event_handler, LV_EVENT_CLICKED, current);

            // 移动到下一个节点
            current = current->next;
        }
    }
}

static void lv_create_setting_list(lv_obj_t * parent) {
    // 创建 setting_list_cont 组件
    setting_list_cont = lv_obj_create(parent);
    lv_obj_set_size(setting_list_cont, 400, 600); // 根据需要调整大小
    lv_obj_align(setting_list_cont, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_flag(setting_list_cont, LV_OBJ_FLAG_HIDDEN); // 初始时隐藏

    lv_obj_t* title = lv_label_create(setting_list_cont);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_32, 0); // 选择你喜欢的字体和字号
    lv_label_set_text(title, "CatTV Setting");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);

    setting_list = lv_list_create(setting_list_cont);
    lv_obj_set_size(setting_list, lv_pct(100), lv_pct(100)); // 填满 video_list 容器
    lv_obj_align(setting_list, LV_ALIGN_CENTER, 0, 50);      // 居中对齐

    // 清空之前的控件
    // lv_obj_clean(setting_list);

    // 循环播放
    lv_obj_t * btn = lv_list_add_btn(setting_list, LV_SYMBOL_LOOP, "List Loop");
    lv_obj_set_height(btn, 80);
    lv_obj_set_style_text_font(btn, &lv_font_montserrat_28, 0);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICKABLE); // 移除按钮的可点击属性，使列表项不可点击

    // 为按钮设置 Flex 布局，使图标、标签、switch 横向排列
    lv_obj_set_layout(btn, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // 创建 switch 控件并添加到按钮中（switch 本身可点击切换）
    play_loop_sw = lv_switch_create(btn);
    lv_obj_set_style_bg_color(play_loop_sw, lv_color_hex(0xAAAAAA), LV_PART_INDICATOR | LV_STATE_DEFAULT); // 关闭时灰色
    lv_obj_set_style_bg_opa(play_loop_sw, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(play_loop_sw, lv_color_hex(0x34C759), LV_PART_INDICATOR | LV_STATE_CHECKED); // 较暗的绿色
    lv_obj_set_style_bg_opa(play_loop_sw, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_size(play_loop_sw, 80, 40);

    // 如需处理switch的事件，可在此添加event_cb
    lv_obj_add_event_cb(play_loop_sw, setting_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

static void setting_event_cb(lv_event_t * e)
{
    lv_obj_t * sw = lv_event_get_target(e);
    bool state = lv_obj_has_state(sw, LV_STATE_CHECKED);

    if(state) {
        // Switch 打开时的处理逻辑
        app_data_set("list_loop_play", 1);
    } else {
        // Switch 关闭时的处理逻辑
        app_data_set("list_loop_play", 0);
    }
}

static void lv_update_setting_list(void) {

    if (!setting_list)
    {
        return;
    }

    if (play_loop_sw)
    {
        int list_loop_play = (int)app_data_get("list_loop_play");
        if (list_loop_play)
        {
            lv_obj_add_state(play_loop_sw, LV_STATE_CHECKED);
        }else{
            lv_obj_clear_state(play_loop_sw, LV_STATE_CHECKED);
        }
    }
    
}

// 事件处理器实现
static void show_video_list_btn_event_handler(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);

    // 获取点击次数
    static uint32_t last_click_time = 0;
    uint32_t now = lv_tick_get();
    uint32_t diff = now - last_click_time;

    if (diff < DOUBLE_CLICK_TIME_MS) { // 如果两次点击间隔小于500ms，认为是双击
        // 切换 video_list_cont 的显示状态
        if (lv_obj_has_flag(video_list_cont, LV_OBJ_FLAG_HIDDEN)) {

            // 更新播放列表
            lv_update_video_list();

            lv_obj_clear_flag(video_list_cont, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(video_list_cont, LV_OBJ_FLAG_HIDDEN);
        }
        // 重置点击时间
        last_click_time = 0;
    } else {
        // 更新最后一次点击时间
        last_click_time = now;
    }
}

// 事件处理器实现
static void show_setting_list_btn_event_handler(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);

    // 获取点击次数
    static uint32_t last_click_time = 0;
    uint32_t now = lv_tick_get();
    uint32_t diff = now - last_click_time;

    if (diff < DOUBLE_CLICK_TIME_MS) { // 如果两次点击间隔小于500ms，认为是双击
        // 切换 setting_list_cont 的显示状态
        if (lv_obj_has_flag(setting_list_cont, LV_OBJ_FLAG_HIDDEN)) {

            // 更新设置列表属性
            lv_update_setting_list();

            lv_obj_clear_flag(setting_list_cont, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(setting_list_cont, LV_OBJ_FLAG_HIDDEN);
        }
        // 重置点击时间
        last_click_time = 0;
    } else {
        // 更新最后一次点击时间
        last_click_time = now;
    }
}

static void lv_create_show_list_btn(lv_obj_t * parent){

    show_video_list_btn = lv_btn_create(background);
    lv_obj_remove_style_all(show_video_list_btn);
    lv_obj_set_size(show_video_list_btn, 300, 300);
    lv_obj_align(show_video_list_btn, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_opa(show_video_list_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(show_video_list_btn, 0, 0);
    lv_obj_set_style_border_opa(show_video_list_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(show_video_list_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_outline_opa(show_video_list_btn, LV_OPA_TRANSP, 0);

    show_setting_list_btn = lv_btn_create(background);
    lv_obj_remove_style_all(show_setting_list_btn);
    lv_obj_set_size(show_setting_list_btn, 300, 300);
    lv_obj_align(show_setting_list_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_bg_opa(show_setting_list_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(show_setting_list_btn, 0, 0);
    lv_obj_set_style_border_opa(show_setting_list_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(show_setting_list_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_outline_opa(show_setting_list_btn, LV_OPA_TRANSP, 0);

    lv_obj_add_event_cb(show_video_list_btn, show_video_list_btn_event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_add_event_cb(show_setting_list_btn, show_setting_list_btn_event_handler, LV_EVENT_CLICKED, NULL);
}

// 应用测试函数
int app_cattv_init(void) {

    screen_base = lv_scr_act();

    // 设置背景为透明
    lv_disp_get_default()->driver->screen_transp = 1;  // 开启透明窗口

    if (style_scr_act.prop_cnt == 0) {
        lv_style_init(&style_scr_act);
        lv_style_set_bg_opa(&style_scr_act, LV_OPA_TRANSP); // 设置背景为透明
        lv_obj_add_style(screen_base, &style_scr_act, 0); // 应用风格
    }
    
    // 背景图
    background = lv_img_create(lv_scr_act());
    lv_img_set_src(background, &video_border_1024_600);  // 使用背景图片
    lv_obj_align(background, LV_ALIGN_TOP_LEFT, 0, 0);    // 确保图片与父对象对齐

    lv_create_show_list_btn(background);

    lv_create_video_list(background);

    lv_create_setting_list(background);


    // start_tplayer("/mnt/sdcard/mmcblk1p1/test3/001.mp4"); // debug

    return 0;
}
 
/*
TODO:
1. 开发列表循环播放模式；
2. 完善当前正在播放视频和当前选中列表的动态更新和显示优化(居中?顶部?);
*/