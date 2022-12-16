/*
************************************************************************
* file name: thread_pool.h
* author: 苏尚宇
* date: Wed Dec 14 08:55:20 CST 2022
* path: /mnt/e/Data/yueq/CD2211/05 系统编程/02 线程/pthread_pool/code2
* describe: NULL
************************************************************************
*/

#ifndef _THREAD_POOL_H__
#define _THREAD_POOL_H__

#define MAX_WAITING_TASKS	1000    // 最大任务数
#define MAX_ACTIVE_THREADS	20      // 最大线程数

#include "kernel_list.h"
#include <stdbool.h>
#include <pthread.h>

// 线程池中的线程
struct thread {
    pthread_t id;           // 线程 ID
    struct list_head list;  // 小结构体
};

// 线程池
struct thread_pool {
    struct list_head threads;  	// 线程队列
    struct list_head queue;  	// 任务队列

    pthread_mutex_t lock;   	// 互斥锁
    pthread_cond_t cond;     	// 条件变量

	unsigned active_threads;	// 线程队列中的线程数量
	unsigned waiting_tasks;		// 任务链队列中等待的任务个数

	bool shutdown;
};

// 线程池中的任务
struct task {
    void *(*func)(void *);   	// 任务函数
    void *arg;              	// 任务函数的参数
    struct list_head list;  	// 小结构体
};

static void handler(void *arg)
{
	// 响应取消请求之后自动处理：释放互斥锁
	pthread_mutex_unlock((pthread_mutex_t *)arg);
}

void *thread_pool_main(void *arg);                                                      // 线程池的线程人口函数
void thread_pool_init(struct thread_pool *pool, int num_threads);                       // 初始化线程池
void thread_pool_add_task(struct thread_pool *pool, void *(*func)(void *), void *arg);  // 添加任务函数
void thread_pool_add_thread(struct thread_pool *pool, int num_threads);                 // 添加新线程
void thread_pool_remove_thread(struct thread_pool *pool, int num_threads);              // 删除线程池中的线程
void thread_pool_destroy(struct thread_pool *pool);                                     // 销毁线程池

#endif
