#include "app_data.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include <dirent.h>
#include <sys/types.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>

#include "folder_list.h"
#include "video_list.h"

// 定义应用数据结构
typedef struct {
    folder_list_t* folder_list; // 目录列表 内部包含 视频列表
    int list_loop_play;            // 是否循环播放
    int volume;                    // 音量
    int pause;                     // 是否暂停
    int menu_level;              // 选项层级 0节目列表、1选集列表
    char playing_file_path[128];
} app_data_t;

// 全局应用数据实例
static app_data_t g_app_data = {0};

// 扫描指定路径并初始化视频链表
static int scan_and_initialize_video_list(video_list_t *list, const char *path) {
    if (list == NULL || path == NULL) {
        printf("Error: Invalid arguments. List or path is NULL.\n");
        return -1;
    }

    // 打开指定目录
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("Error: Could not open directory %s.\n", path);
        return -1;
    }

    struct dirent *entry;
    int video_index = 1;
    video_info_t video;

    // 扫描目录，找到所有 .mp4 文件
    while ((entry = readdir(dir)) != NULL) {

        memset(&video, 0, sizeof(video_info_t));

        // 忽略当前目录和父目录
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 只处理 .mp4 文件
        if (strstr(entry->d_name, ".mp4") != NULL) {
            // 填充视频信息
            snprintf(video.name, MAX_NAME_LEN, "%s", entry->d_name);
            snprintf(video.file_path, MAX_PATH_LEN, "%s/%s", path, entry->d_name);

            // 默认第一个视频正在播放
            // video.is_playing = (video_index == 1) ? 1 : 0;

            if (0 == strcmp((char*)app_data_get("playing_file_path"), video.file_path))// 还原上次正在播放的视频
            {
                video.is_playing = 1;
            } 

            // 添加到视频列表
            if (add_video_to_list(list, &video) != 0) {
                printf("Error: Failed to add video '%s' to the list.\n", video.name);
                // 根据需要决定是否继续添加其他视频
            }

            video_index++;  // 增加视频索引
        }
    }

    closedir(dir);

    printf("Debug: Finished scanning directory %s. Total videos added: %d\n", path, video_index - 1);
    return 0;
}

// 扫描指定路径，查找包含 .mp4 文件的目录
static int scan_directories_with_mp4(folder_list_t *folder_list, const char *root_path) {
    if (folder_list == NULL || root_path == NULL) {
        printf("Error: Invalid arguments. folder_list or root_path is NULL.\n");
        return -1;
    }

    // 打开根目录
    DIR *dir = opendir(root_path);
    if (dir == NULL) {
        printf("Error: Could not open directory %s.\n", root_path);
        return -1;
    }

    struct dirent *entry;

    // 遍历根目录
    while ((entry = readdir(dir)) != NULL) {
        // 忽略当前目录和父目录
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 构造完整路径
        char full_path[MAX_PATH_LEN];
        snprintf(full_path, sizeof(full_path), "%s/%s", root_path, entry->d_name);

        // 检查是否为目录
        struct stat path_stat;
        if (stat(full_path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
            // 打开子目录
            DIR *sub_dir = opendir(full_path);
            if (sub_dir == NULL) {
                printf("Error: Could not open subdirectory %s.\n", full_path);
                continue;
            }

            struct dirent *file_entry;
            int has_mp4 = 0;

            // 遍历子目录
            while ((file_entry = readdir(sub_dir)) != NULL) {
                // 忽略当前目录和父目录
                if (strcmp(file_entry->d_name, ".") == 0 || strcmp(file_entry->d_name, "..") == 0) {
                    continue;
                }

                // 检查是否包含 .mp4 文件
                if (strstr(file_entry->d_name, ".mp4") != NULL) {
                    has_mp4 = 1;
                    break; // 如果找到一个 .mp4 文件就可以退出循环
                }
            }

            closedir(sub_dir);

            // 如果目录包含 .mp4 文件，则将其添加到文件夹列表
            if (has_mp4) {
                // 使用 create_folder_info 创建文件夹信息
                folder_info_t *folder = create_folder_info(entry->d_name, full_path);
                if (folder == NULL) {
                    printf("Error: Failed to create folder_info for directory %s.\n", full_path);
                    continue;
                }

                // 添加到文件夹列表
                if (add_folder_to_list(folder_list, folder) != 0) {
                    printf("Error: Failed to add folder '%s' to the list.\n", folder->dir_name);
                    destroy_folder_info(folder); // 添加失败，释放文件夹信息
                    continue;
                }

                // 成功添加后，可以销毁临时对象，因为 `add_folder_to_list` 已经深拷贝
                destroy_folder_info(folder);
            }
        }
    }

    closedir(dir);
    return 0;
}

void app_data_update(void) {
    // 从全局数据获取文件夹列表
    folder_list_t* list = (folder_list_t*)app_data_get("folder_list");
    if (list == NULL) {
        printf("Error: folder_list is NULL.\n");
        return;
    }

    // 查找当前选中的文件夹
    folder_node_t* selected_folder = find_selected_folder(list);
    if (selected_folder == NULL) {
        printf("Error: No folder is selected in folder_list.\n");
        return;
    }

    printf("Debug: Selected folder is '%s', full path: '%s'\n",
           selected_folder->folder.dir_name,
           selected_folder->folder.full_path);

    // 如果当前文件夹的 video_list 已经存在，则销毁旧的 video_list
    if (selected_folder->folder.video_list != NULL) {
        printf("Debug: Destroying old video_list for folder '%s'\n", selected_folder->folder.dir_name);
        if (destroy_video_list(selected_folder->folder.video_list) != 0) {
            printf("Error: Failed to destroy old video_list for folder '%s'\n", selected_folder->folder.dir_name);
            // 根据需要决定是否继续
        }
        selected_folder->folder.video_list = NULL;
    }

    // 创建一个新的空视频列表
    selected_folder->folder.video_list = create_video_list();
    if (selected_folder->folder.video_list == NULL) {
        printf("Error: Failed to create new video_list for folder '%s'\n", selected_folder->folder.dir_name);
        return;
    }

    printf("Debug: New video_list created for folder '%s'\n", selected_folder->folder.dir_name);

    // 扫描选中文件夹并初始化视频列表
    if (scan_and_initialize_video_list(selected_folder->folder.video_list, selected_folder->folder.full_path) == -1) {
        printf("Error: Failed to initialize video list for folder '%s'\n", selected_folder->folder.dir_name);
        destroy_video_list(selected_folder->folder.video_list);
        selected_folder->folder.video_list = NULL;
        return;
    }

    printf("Debug: Video list successfully updated for folder '%s'\n", selected_folder->folder.dir_name);
    // print_video_list(selected_folder->folder.video_list);
}

void app_data_init(void){

    memset(&g_app_data, 0, sizeof(app_data_t));

    g_app_data.list_loop_play = 1; // 默认列表循环
    g_app_data.folder_list = create_folder_list();

    // 扫描根目录 "/mnt/SDCARD" 并初始化文件夹列表
    // if (scan_directories_with_mp4(g_app_data.folder_list, "/mnt/SDCARD") == 0) {
    if (scan_directories_with_mp4(g_app_data.folder_list, "/mnt/sdcard/mmcblk1p1") == 0) {
        printf("Folder list initialized successfully.\n");
        print_folder_list(g_app_data.folder_list); // 打印文件夹及其视频列表
    } else {
        printf("Failed to initialize folder list.\n");
    }
}

// 设置应用数据
void app_data_set(char *key, void *value) {
    if (strcmp(key, "folder_list") == 0) {
        g_app_data.folder_list = (folder_list_t *)value;
        printf("Set folder_list.\n");
    } else if (strcmp(key, "list_loop_play") == 0) {
        g_app_data.list_loop_play = (int)value;
        printf("Set list_loop_play to %d.\n", g_app_data.list_loop_play);
    } else if (strcmp(key, "volume") == 0) {
        g_app_data.volume = (int)value;
        printf("Set volume to %d.\n", g_app_data.volume);
    } else if (strcmp(key, "pause") == 0) {
        g_app_data.pause = (int)value;
        printf("Set pause to %d.\n", g_app_data.pause);
    } else if (strcmp(key, "menu_level") == 0) {
        g_app_data.menu_level = (int)value;
        printf("Set menu_level to %d.\n", g_app_data.menu_level);
    } else if (strcmp(key, "playing_file_path") == 0) {
        strcpy(g_app_data.playing_file_path, (char*)value);
        printf("Set playing_file_path %s .\n", g_app_data.playing_file_path);
    }

    else {
        printf("Invalid key: %s\n", key);
    }
}

// 获取应用数据
void *app_data_get(char *key) {
    if (strcmp(key, "folder_list") == 0) {
        return g_app_data.folder_list;
    } else if (strcmp(key, "list_loop_play") == 0) {
        return (void*)g_app_data.list_loop_play;
    } else if (strcmp(key, "volume") == 0) {
        return (void*)g_app_data.volume;
    } else if (strcmp(key, "pause") == 0) {
        return (void*)g_app_data.pause;
    } else if (strcmp(key, "menu_level") == 0) {
        return (void*)g_app_data.menu_level;
    } else if (strcmp(key, "playing_file_path") == 0) {
        return (void*)g_app_data.playing_file_path;
    }
    
    else {
        printf("Invalid key: %s\n", key);
        return NULL;
    }
}