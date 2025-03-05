#ifndef FOLDER_LIST_H
#define FOLDER_LIST_H

#include "video_list.h"
#include <stdint.h>

#define MAX_NAME_LEN 64
#define MAX_PATH_LEN 128

// 文件夹信息结构体
typedef struct {
    char dir_name[MAX_NAME_LEN];      // 文件夹名称（非完整路径）
    char full_path[MAX_PATH_LEN];     // 文件夹完整路径（绝对路径）
    int is_select;                    // 是否被选中
    video_list_t *video_list;         // 指向文件夹下的视频列表
} folder_info_t;

// 链表节点
typedef struct folder_node {
    folder_info_t folder;            // 文件夹信息
    struct folder_node *next;        // 指向下一个节点的指针
} folder_node_t;

// 文件夹列表结构体
typedef struct {
    folder_node_t *head;             // 链表头指针
    folder_node_t *tail;             // 链表尾指针
    folder_node_t *selected;         // 指向选中的文件夹
} folder_list_t;

// 函数声明
folder_list_t *create_folder_list();
int destroy_folder_list(folder_list_t *list);
int add_folder_to_list(folder_list_t *list, const folder_info_t *folder);
int remove_folder_from_list(folder_list_t *list, const char *dir_name);
void print_folder_list(const folder_list_t *list);
folder_node_t *find_folder_by_name(const folder_list_t *list, const char *dir_name);

// 新增功能
folder_node_t *find_selected_folder(const folder_list_t *list);
int select_folder(folder_list_t *list, const char *dir_name);
folder_info_t *create_folder_info(const char *dir_name, const char *full_path);
int destroy_folder_info(folder_info_t *folder);

#endif /* FOLDER_LIST_H */
