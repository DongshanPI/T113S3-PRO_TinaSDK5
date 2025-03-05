#include "video_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 创建一个新的空视频列表
video_list_t *create_video_list() {
    video_list_t *list = (video_list_t *)malloc(sizeof(video_list_t));
    if (list == NULL) {
        printf("Memory allocation failed for video list!\n");
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    return list;
}

// 销毁视频列表，释放内存
int destroy_video_list(video_list_t *list) {
    if (list == NULL) {
        printf("destroy_video_list: list is NULL!\n");
        return -1;
    }

    video_node_t *current = list->head;
    video_node_t *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    free(list);
    return 0;
}

// 添加视频到列表末尾
int add_video_to_list(video_list_t *list, const video_info_t *video) {
    if (list == NULL || video == NULL) {
        printf("add_video_to_list: Invalid arguments!\n");
        return -1;
    }

    video_node_t *new_node = (video_node_t *)malloc(sizeof(video_node_t));
    if (new_node == NULL) {
        printf("Memory allocation failed for new video node!\n");
        return -1;
    }

    new_node->video = *video;
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

// 从列表中删除指定名称的视频
int remove_video_from_list(video_list_t *list, const char *name) {
    if (list == NULL || name == NULL) {
        printf("remove_video_from_list: Invalid arguments!\n");
        return -1;
    }

    video_node_t *current = list->head;
    video_node_t *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->video.name, name) == 0) {
            // 找到要删除的视频节点
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
            free(current);
            return 0; // 成功
        }
        previous = current;
        current = current->next;
    }

    printf("Video '%s' not found in the list!\n", name);
    return -1; // 未找到
}

// 打印视频列表信息
void print_video_list(const video_list_t *list) {
    if (list == NULL) {
        printf("print_video_list: list is NULL!\n");
        return;
    }

    video_node_t *current = list->head;
    while (current != NULL) {
        printf("Video Name: %s, File Path: %s, Playing: %d\n",
               current->video.name, current->video.file_path, current->video.is_playing);
        current = current->next;
    }
}

// 查找视频节点
video_node_t *find_video_by_name(const video_list_t *list, const char *name) {
    printf("Debug: Calling find_video_by_name with name='%s'\n", name);

    if (list == NULL) {
        printf("Debug: find_video_by_name failed - list is NULL.\n");
        return NULL;
    }

    if (name == NULL) {
        printf("Debug: find_video_by_name failed - name is NULL.\n");
        return NULL;
    }

    video_node_t *current = list->head;
    int index = 0; // 用于记录当前视频的位置

    while (current != NULL) {
        printf("Debug: Comparing with video at index %d - name='%s'\n", index, current->video.name);
        if (strcmp(current->video.name, name) == 0) {
            printf("Debug: Found video '%s' at index %d.\n", name, index);
            return current;
        }
        current = current->next;
        index++;
    }

    printf("Debug: Video '%s' not found in the list.\n", name);
    return NULL;
}

// 查找正在播放的视频节点
video_node_t *find_playing_video(const video_list_t *list) {
    if (list == NULL) {
        printf("Debug: find_video_by_name failed - list is NULL.\n");
        return NULL;
    }

    video_node_t *current = list->head;
    int index = 0; // 用于记录当前视频的位置

    while (current != NULL) {
        if (current->video.is_playing) {
            printf("Debug: Found video '%s' at index %d.\n", current->video.name, index);
            return current;
        }
        current = current->next;
        index++;
    }

    return NULL;
}


// 生成伪造的 20 个视频数据
void generate_fake_video_data(video_list_t *list) {
    if (list == NULL) {
        printf("generate_fake_video_data: list is NULL!\n");
        return;
    }

    for (int i = 1; i <= 20; i++) {
        video_info_t video;
        video.is_playing = (i == 5) ? 1 : 0;
        snprintf(video.name, MAX_NAME_LEN, "T-Video-%d", i);
        snprintf(video.file_path, MAX_PATH_LEN, "/path/to/video%d.mp4", i);
        if (add_video_to_list(list, &video) != 0) {
            printf("Failed to add video '%s' to the list!\n", video.name);
        }
    }
}
