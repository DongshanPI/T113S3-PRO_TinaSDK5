#include "folder_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// 创建一个新的空文件夹列表
folder_list_t *create_folder_list() {
    folder_list_t *list = (folder_list_t *)malloc(sizeof(folder_list_t));
    if (list == NULL) {
        printf("Memory allocation failed for folder list!\n");
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->selected = NULL;
    return list;
}

// 创建文件夹信息并初始化视频列表
folder_info_t *create_folder_info(const char *dir_name, const char *full_path) {
    if (dir_name == NULL || full_path == NULL) {
        printf("create_folder_info: Invalid arguments!\n");
        return NULL;
    }

    folder_info_t *folder = malloc(sizeof(folder_info_t));
    if (folder == NULL) {
        printf("Memory allocation failed for folder_info_t!\n");
        return NULL;
    }

    strncpy(folder->dir_name, dir_name, MAX_NAME_LEN - 1);
    folder->dir_name[MAX_NAME_LEN - 1] = '\0';

    strncpy(folder->full_path, full_path, MAX_PATH_LEN - 1);
    folder->full_path[MAX_PATH_LEN - 1] = '\0';

    folder->is_select = 0;
    folder->video_list = create_video_list();
    if (folder->video_list == NULL) {
        free(folder);
        return NULL;
    }

    return folder;
}

// 销毁文件夹信息及其视频列表
int destroy_folder_info(folder_info_t *folder) {
    if (folder == NULL) {
        return -1;
    }

    if (destroy_video_list(folder->video_list) != 0) {
        printf("Failed to destroy video list for folder '%s'!\n", folder->dir_name);
        // 继续释放 folder 即使视频列表销毁失败
    }
    free(folder);
    return 0;
}

// 销毁文件夹列表，释放内存
int destroy_folder_list(folder_list_t *list) {
    if (list == NULL) {
        printf("destroy_folder_list: list is NULL!\n");
        return -1;
    }

    folder_node_t *current = list->head;
    folder_node_t *next_node;
    while (current != NULL) {
        next_node = current->next;
        destroy_folder_info(&current->folder);
        free(current);
        current = next_node;
    }
    free(list);
    return 0;
}

// 添加文件夹到列表末尾
int add_folder_to_list(folder_list_t *list, const folder_info_t *folder) {
    if (list == NULL || folder == NULL) {
        printf("add_folder_to_list: Invalid arguments!\n");
        return -1;
    }

    folder_node_t *new_node = (folder_node_t *)malloc(sizeof(folder_node_t));
    if (new_node == NULL) {
        printf("Memory allocation failed for new folder node!\n");
        return -1;
    }

    // 深拷贝文件夹信息
    new_node->folder = *folder; // 复制基本信息
    // 创建独立的视频列表
    new_node->folder.video_list = create_video_list();
    if (new_node->folder.video_list == NULL) {
        free(new_node);
        printf("Failed to create video list for new folder!\n");
        return -1;
    }

    // 如果原文件夹有视频列表，进行深拷贝
    video_node_t *current_video = folder->video_list->head;
    while (current_video != NULL) {
        if (add_video_to_list(new_node->folder.video_list, &current_video->video) != 0) {
            printf("Failed to copy video '%s' to new folder!\n", current_video->video.name);
            destroy_folder_info(&new_node->folder);
            free(new_node);
            return -1;
        }
        current_video = current_video->next;
    }

    new_node->next = NULL;

    if (list->tail == NULL) {
        // 如果列表为空，设置头尾指针
        list->head = new_node;
        list->tail = new_node;
    } else {
        // 否则，将新节点添加到尾部
        list->tail->next = new_node;
        list->tail = new_node;
    }

    return 0; // 成功
}

// 从列表中删除指定名称的文件夹
int remove_folder_from_list(folder_list_t *list, const char *dir_name) {
    if (list == NULL || dir_name == NULL) {
        printf("remove_folder_from_list: Invalid arguments!\n");
        return -1;
    }

    folder_node_t *current = list->head;
    folder_node_t *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->folder.dir_name, dir_name) == 0) {
            // 找到要删除的文件夹节点
            if (previous == NULL) {
                // 删除的是头节点
                list->head = current->next;
                if (list->head == NULL) {
                    // 如果删除后列表为空，尾指针也需要清空
                    list->tail = NULL;
                }
            } else {
                previous->next = current->next;
                if (current->next == NULL) {
                    // 如果删除的是尾节点，更新尾指针
                    list->tail = previous;
                }
            }

            // 如果删除的是选中的文件夹，重置 selected 指针
            if (list->selected == current) {
                list->selected = NULL;
            }

            destroy_folder_info(&current->folder);
            free(current);
            return 0; // 成功
        }
        previous = current;
        current = current->next;
    }

    printf("Folder '%s' not found in the list!\n", dir_name);
    return -1; // 未找到
}

// 打印文件夹列表信息
void print_folder_list(const folder_list_t *list) {
    if (list == NULL) {
        printf("print_folder_list: list is NULL!\n");
        return;
    }

    folder_node_t *current = list->head;
    while (current != NULL) {
        printf("Folder Name: %s, Full Path: %s, Is Selected: %d\n",
               current->folder.dir_name,
               current->folder.full_path,
               current->folder.is_select);
        print_video_list(current->folder.video_list); // 打印视频列表
        current = current->next;
    }
}

// 查找文件夹节点
folder_node_t *find_folder_by_name(const folder_list_t *list, const char *dir_name) {
    if (list == NULL || dir_name == NULL) {
        return NULL;
    }

    folder_node_t *current = list->head;
    while (current != NULL) {
        if (strcmp(current->folder.dir_name, dir_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// 查找选中的文件夹
folder_node_t *find_selected_folder(const folder_list_t *list) {
    if (list == NULL) {
        return NULL;
    }

    return list->selected;
}

// 设置文件夹为选中状态
int select_folder(folder_list_t *list, const char *dir_name) {
    if (list == NULL || dir_name == NULL) {
        printf("select_folder: Invalid arguments!\n");
        return -1;
    }

    folder_node_t *node = find_folder_by_name(list, dir_name);
    if (node == NULL) {
        printf("Folder '%s' not found in the list!\n", dir_name);
        return -1;
    }

    if (list->selected != NULL) {
        list->selected->folder.is_select = 0;
    }

    node->folder.is_select = 1;
    list->selected = node;

    return 0; // 成功
}
