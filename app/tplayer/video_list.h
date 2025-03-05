#ifndef VIDEO_LIST_H
#define VIDEO_LIST_H

#include <stdint.h>

#define MAX_NAME_LEN 64
#define MAX_PATH_LEN 128

// 视频信息结构体
typedef struct {
    int is_playing;                // 标记视频是否正在播放
    char name[MAX_NAME_LEN];       // 视频名称
    char file_path[MAX_PATH_LEN];  // 视频文件路径
} video_info_t;

// 链表节点
typedef struct video_node {
    video_info_t video;            // 视频信息
    struct video_node *next;       // 指向下一个节点的指针
} video_node_t;

// 播放列表结构体
typedef struct {
    video_node_t *head;            // 链表头指针
    video_node_t *tail;            // 链表尾指针
} video_list_t;

// 函数声明
video_list_t *create_video_list();
int destroy_video_list(video_list_t *list);
int add_video_to_list(video_list_t *list, const video_info_t *video);
int remove_video_from_list(video_list_t *list, const char *name);
void print_video_list(const video_list_t *list);
video_node_t *find_video_by_name(const video_list_t *list, const char *name);

// 查找正在播放的视频节点
video_node_t *find_playing_video(const video_list_t *list);

// 生成伪造的 20 个视频数据
void generate_fake_video_data(video_list_t *list);

#endif /* VIDEO_LIST_H */
