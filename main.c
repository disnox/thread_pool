/*
************************************************************************
* file name: main.c
* author: 苏尚宇
* date: Wed Dec 14 08:55:39 CST 2022
* path: /mnt/e/Data/yueq/CD2211/05 系统编程/02 线程/pthread_pool/code2
* describe: NULL
************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "thread_pool.h"

// 任务1
void *task(void *arg)
{
    printf("Task 1 is running...\n");

    // 任务1休眠一段时间
    sleep(1);

    printf("Task 1 is done.\n");

    //pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // 创建线程池
    struct thread_pool pool;
    thread_pool_init(&pool, 4);
printf("当前线程数为：%u\n", pool.active_threads);
	sleep(1);

	// 删除线程
	thread_pool_remove_thread(&pool, 2);
printf("当前线程数为：%u\n", pool.active_threads);
	sleep(1);

	// 添加线程到线程池中
	thread_pool_add_thread(&pool, 3);
printf("当前线程数为：%u\n", pool.active_threads);
	sleep(1);

    // 添加任务到线程池中
    thread_pool_add_task(&pool, task, NULL);
	sleep(1);
    thread_pool_add_task(&pool, task, NULL);
	sleep(1);
    thread_pool_add_task(&pool, task, NULL);
	sleep(1);

    // 主线程休眠一段时间，让任务完成
    sleep(1);
printf("\033[31m""---------Function: [%s]---------Line: [%d]---------\n""\033[m", __FUNCTION__, __LINE__);	
    // 清理线程池
    thread_pool_destroy(&pool);

printf("\033[31m""---------Function: [%s]---------Line: [%d]---------\n""\033[m", __FUNCTION__, __LINE__);

    return 0;
}
