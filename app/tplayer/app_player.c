#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>

#include <tplayer.h>

#include "app_player.h"

static volatile int g_user_exit = 0;

typedef struct DemoPlayerContext
{
    TPlayer*  mTPlayer;
    int       mSeekable;
    int       mError;
    int       mComplete;
    int       mPreparedFlag;
    sem_t     mPreparedSem;
    char      mUrl[512];
}DemoPlayerContext;

static void terminate(int sig_no)
{
    printf("Got signal %d, exiting ...\n", sig_no);
    exit(1);
}

static void install_sig_handler(void)
{
    signal(SIGBUS, terminate);
    signal(SIGFPE, terminate);
    signal(SIGHUP, terminate);
    signal(SIGILL, terminate);
    signal(SIGINT, terminate);
    signal(SIGIOT, terminate);
    signal(SIGPIPE, terminate);
    signal(SIGQUIT, terminate);
    signal(SIGSEGV, terminate);
    signal(SIGSYS, terminate);
    signal(SIGTERM, terminate);
    signal(SIGTRAP, terminate);
    signal(SIGUSR1, terminate);
    signal(SIGUSR2, terminate);
}

static int semTimedWait(sem_t* sem, int64_t time_ms)
{
    int err;
    if(time_ms == -1)
    {
        err = sem_wait(sem);
    }
    else
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += time_ms/1000;
        ts.tv_nsec += (time_ms%1000)*1000*1000;
        if(ts.tv_nsec >= 1000*1000*1000)
        {
            ts.tv_sec += ts.tv_nsec/(1000*1000*1000);
            ts.tv_nsec = ts.tv_nsec%(1000*1000*1000);
        }
        err = sem_timedwait(sem, &ts);
    }
    return err;
}

int CallbackForTPlayer(void* pUserData, int msg, int param0, void* param1)
{
    DemoPlayerContext* pDemoPlayer = (DemoPlayerContext*)pUserData;
    (void)param1;
    (void)param0;

    switch(msg)
    {
        case TPLAYER_NOTIFY_PREPARED:
        {
            pDemoPlayer->mPreparedFlag = 1;
            sem_post(&pDemoPlayer->mPreparedSem);
            printf("TPLAYER_NOTIFY_PREPARED,has prepared.\n");
            break;
        }

        case TPLAYER_NOTIFY_PLAYBACK_COMPLETE:
        {
            pDemoPlayer->mComplete = 1;
            printf("TPLAYER_NOTIFY_PLAYBACK_COMPLETE\n");
            break;
        }

        case TPLAYER_NOTIFY_SEEK_COMPLETE:
        {
            printf("TPLAYER_NOTIFY_SEEK_COMPLETE\n");
            break;
        }

        case TPLAYER_NOTIFY_MEDIA_ERROR:
        {
            pDemoPlayer->mError = 1;
            if(pDemoPlayer->mPreparedFlag == 0)
                sem_post(&pDemoPlayer->mPreparedSem);
            printf("TPLAYER_NOTIFY_MEDIA_ERROR\n");
            break;
        }

        case TPLAYER_NOTIFY_NOT_SEEKABLE:
        {
            pDemoPlayer->mSeekable = 0;
            printf("info: media source is unseekable.\n");
            break;
        }

        default:
        {
            //printf("warning: unknown callback from Tinaplayer.\n");
            break;
        }
    }
    return 0;
}

void app_tplayer_exit(void){
    g_user_exit = 1;
}

int tplayer_main(char* file_path)
{
    install_sig_handler();

    // tplayer使用的LCD图层低于LVGL和开机logo 所以需要清除一下fb0否则可能会被原有的UI遮盖。LVGL可以设置透明度的一块窗口用于视频播放，TODO！！！

    // 清空帧缓冲，这在原demo里有
    // if(access("/dev/zero",F_OK) == 0 && access("/dev/fb0",F_OK) == 0)
    // {
    //     system("dd if=/dev/zero of=/dev/fb0");//clean the framebuffer
    // }

    DemoPlayerContext demoPlayer;
    memset(&demoPlayer, 0, sizeof(DemoPlayerContext));
    demoPlayer.mSeekable = 1;
    sem_init(&demoPlayer.mPreparedSem, 0, 0);

    // 创建播放器
    demoPlayer.mTPlayer= TPlayerCreate(CEDARX_PLAYER);
    if(demoPlayer.mTPlayer == NULL)
    {
        printf("can not create tplayer, quit.\n");
        sem_destroy(&demoPlayer.mPreparedSem);
        return -1;
    }

    TPlayerSetRotate(demoPlayer.mTPlayer, TPLAYER_VIDEO_ROTATE_DEGREE_0);// 设置旋转角度 0、90、180、270

    // 设置回调
    TPlayerSetNotifyCallback(demoPlayer.mTPlayer,CallbackForTPlayer, (void*)&demoPlayer);

    // 设置数据源
    strcpy(demoPlayer.mUrl, file_path);
    
    if(TPlayerSetDataSource(demoPlayer.mTPlayer,(const char*)demoPlayer.mUrl,NULL) != 0)
    {
        printf("TPlayerSetDataSource() return fail.\n");
        goto EXIT;
    }
    else
    {
        printf("SetDataSource end\n");
    }

    // 异步准备
    if(TPlayerPrepareAsync(demoPlayer.mTPlayer) != 0)
    {
        printf("TPlayerPrepareAsync() return fail.\n");
        goto EXIT;
    }
    else
    {
        printf("preparing...\n");
    }

    // 等待准备完成或出错
    if(semTimedWait(&demoPlayer.mPreparedSem,300*1000) != 0)
    {
        printf("prepare fail,has wait 300s\n");
        goto EXIT;
    }

    if(demoPlayer.mError == 1)
    {
        printf("prepare fail\n");
        goto EXIT;
    }

    printf("prepare ok\n");

    // 设置音量(0~40)
    if(TPlayerSetVolume(demoPlayer.mTPlayer,50) != 0)
    {
        printf("TPlayerSetVolume fail.\n");
    }

    // 保持最后一帧
    TPlayerSetHoldLastPicture(demoPlayer.mTPlayer,1);

    // 设置显示区域（根据原demo中获取到的大小）
    // 假设屏幕分辨率为1024x600，如果你知道实际分辨率请替换
    // TPlayerSetDisplayRect(demoPlayer.mTPlayer, 0, 0, 1024/2, 600/2);
    TPlayerSetDisplayRect(demoPlayer.mTPlayer, 45, 35, 940, 540);

    TPlayerSeekTo(demoPlayer.mTPlayer, 3000); // skip 3S

    // 开始播放
    if(TPlayerStart(demoPlayer.mTPlayer) != 0)
    {
        printf("TPlayerStart() return fail.\n");
        goto EXIT;
    }
    else
    {
        printf("started.\n");
    }

    // 等待播放完成或出错
    while(!demoPlayer.mComplete && !demoPlayer.mError && !g_user_exit)
    {
        usleep(10*1000);
    }

    g_user_exit = 0;

    if(demoPlayer.mComplete)
        printf("Playback complete.\n");
    else if(demoPlayer.mError)
        printf("Playback error occurred.\n");

EXIT:
    if(demoPlayer.mTPlayer != NULL)
    {
        TPlayerDestroy(demoPlayer.mTPlayer);
        demoPlayer.mTPlayer = NULL;
        printf("TPlayerDestroy() successfully\n");
    }

    sem_destroy(&demoPlayer.mPreparedSem);

    return 0;
}