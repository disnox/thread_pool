/*
************************************************************************
* file name: thread_pool.c
* author: 苏尚宇
* date: Wed Dec 14 08:55:39 CST 2022
* path: /mnt/e/Data/yueq/CD2211/05 系统编程/02 线程/pthread_pool/code2
* describe: NULL
************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

#include "kernel_list.h"
#include "thread_pool.h"

/*
************************************************************************
* @brief      thread_pool_main()
* @param      *arg: 任务函数的形参
* @return     void
* describe:   线程池中的线程入口函数
************************************************************************
*/  
void *thread_pool_main(void *arg)
{
	struct thread_pool *pool = (struct thread_pool *) arg;
	struct task *task;

	while (1) {
printf("\033[31m""---------Function: [%s]---------Line: [%d]---------\n""\033[m", __FUNCTION__, __LINE__);		
		// 访问任务队列前加锁，为防止取消后死锁，注册处理例程 handle
		pthread_cleanup_push(handler, (void *)&pool->lock);	//防止死锁
		// 获取锁
		pthread_mutex_lock(&pool->lock);

		// 如果任务队列为空，则阻塞等待
		while (list_empty(&pool->queue) && !pool->shutdown) {
			pthread_cond_wait(&pool->cond, &pool->lock);
printf("\033[31m""---------Function: [%s]---------Line: [%d]---------\n""\033[m", __FUNCTION__, __LINE__);
		}

		//如果任务为空，线程池被销毁，则立即释放互斥锁并退出
		if(list_empty(&pool->queue) && pool->shutdown) {
			pthread_mutex_unlock(&pool->lock);
			pthread_exit(NULL);
		}

		// 将队列中的第一个任务取出
		task = list_first_entry(&pool->queue, struct task, list);
		list_del(&task->list);
		pool->waiting_tasks--;	// 任务数量减1
printf("当前任务数为：%u\n", pool->waiting_tasks);
		// 释放锁
		pthread_mutex_unlock(&pool->lock);
		// 任务取走，解锁，并弹栈 handle（但不执行它）
		pthread_cleanup_pop(0);

		//执行任务期间拒绝取消请求
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		task->func(task->arg);	// 执行任务
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

		free(task);	//释放资源
	}
	pthread_exit(NULL);
}
/*
************************************************************************
* @brief      thread_pool_init()
* @param      *pool：线程池结构体指针
* @param      num_threads: 需要创建多少条进程
* @return     void
* describe:   初始化线程池
************************************************************************
*/  
void thread_pool_init(struct thread_pool *pool, int num_threads)
{
    int i;

    // 检查 num_threads 的值是否超过了最大值
    if (num_threads > MAX_ACTIVE_THREADS) {
        num_threads = MAX_ACTIVE_THREADS;
    }

	pool->waiting_tasks = 0;				//初始化任务队列中等待的任务个数
	pool->active_threads = num_threads;		//初始化活跃线程个数
	pool->shutdown = false;					//初始化线程池销毁标记

    INIT_LIST_HEAD(&pool->threads);	// 初始化线程队列
    INIT_LIST_HEAD(&pool->queue);	// 初始化任务队列

	// 是否初始化成功
	if (!list_empty(&pool->queue) || !list_empty(&pool->threads)) {
		fprintf(stderr, "init fail.\n");
		exit(1);
	}

    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->cond, NULL);

    // 创建线程池中的线程
    for (i = 0; i < num_threads; i++) {
        struct thread *thread = (struct thread *)malloc(sizeof(struct thread));

        INIT_LIST_HEAD(&thread->list);
        // 使用 pthread_create 创建线程
        int ret = pthread_create(&thread->id, NULL, thread_pool_main, pool);
		if (ret != 0) {
			perror("create threads error");
			exit(1);
		}

        // 将新创建的线程添加到线程池中
        list_add_tail(&thread->list, &pool->threads);
    }
}
/*
************************************************************************
* @brief      thread_pool_add_task()
* @param      *pool：线程池结构体指针
* @param      *(*func)(void *): 任务函数
* @param      *arg: 任务函数的形式参数
* @return     void
* describe:   添加任务
************************************************************************
*/  
void thread_pool_add_task(struct thread_pool *pool, void *(*func)(void *), void *arg)
{
    // 分配内存给新任务
    struct task *new_task = (struct task *) malloc(sizeof(struct task));

	if(new_task == NULL) {
		perror("allocate memory error");
		exit(1);
	}

	// 初始化任务节点
    new_task->func = func;
    new_task->arg = arg;

    // 获取锁
    pthread_mutex_lock(&pool->lock);
	// 超过最大任务

	if(pool->waiting_tasks >= MAX_WAITING_TASKS)
	{
		pthread_mutex_unlock(&pool->lock);

		fprintf(stderr, "too many tasks.\n");
		free(new_task);

		exit(1);
	}

    // 将新任务添加到任务队列的末尾
    list_add_tail(&new_task->list, &pool->queue);

	// 新任务加一
	pool->waiting_tasks++;
printf("当前任务数为：%u\n", pool->waiting_tasks);
    // 唤醒一个线程，让它开始执行新任务
    pthread_cond_signal(&pool->cond);

    // 释放锁
    pthread_mutex_unlock(&pool->lock);
}
/*
************************************************************************
* @brief      thread_pool_add_thread()
* @param      *pool：线程池结构体指针
* @param      num_threads: 需要创建多少条进程
* @return     void
* describe:   向线程池添加新线程
************************************************************************
*/  
void thread_pool_add_thread(struct thread_pool *pool, int num_threads)
{
    int i;

    // 检查 num_threads 的值是否超过了最大值
    if (num_threads > MAX_ACTIVE_THREADS - pool->active_threads) {
        num_threads = MAX_ACTIVE_THREADS - pool->active_threads;
    }

    // 创建新线程
    for (i = 0; i < num_threads; i++) {
        struct thread *thread = (struct thread *)malloc(sizeof(struct thread));

        INIT_LIST_HEAD(&thread->list);
        // 使用 pthread_create 创建线程
        int ret = pthread_create(&thread->id, NULL, thread_pool_main, pool);
		if (ret != 0) {
			perror("create threads error");
			exit(1);
		}

        // 将新创建的线程添加到线程池中
        list_add_tail(&thread->list, &pool->threads);
    }

    // 更新线程池中的活跃线程数
    pool->active_threads += num_threads;
}
/*
************************************************************************
* @brief      thread_pool_remove_thread()
* @param      *pool：线程池结构体指针
* @param      num_threads: 需要删除多少条进程
* @return     void
* describe:   删除线程池中的线程
************************************************************************
*/ 
void thread_pool_remove_thread(struct thread_pool *pool, int num_threads)
{
    int i;
    for (i = 0; i < num_threads; i++) {
        // 从线程池中获取最后一个线程
        struct thread *thread = list_last_entry(&pool->threads, struct thread, list);

        // 使用 pthread_cancel 取消线程
        int ret = pthread_cancel(thread->id);
        if (ret != 0) {
            fprintf(stderr, "pthread_cancel failed: %s\n", strerror(ret));
        }

        // 检查线程是否正在执行任务
        if (pool->waiting_tasks > 0) {
            // 如果线程正在执行任务，则等待它完成
            pthread_join(thread->id, NULL);
        }

        // 从线程池中删除线程
        list_del(&thread->list);
        free(thread);

		// 更新线程池中的线程数
        pool->active_threads--;
    }
}
/*
************************************************************************
* @brief      thread_pool_destroy()
* @param      *pool：线程池结构体指针
* @return     void
* describe:   销毁线程池
************************************************************************
*/ 
void thread_pool_destroy(struct thread_pool *pool)
{
	pool->shutdown = true;	//线程池的销毁标记
	pthread_cond_broadcast(&pool->cond);	//唤醒所有线程
	int i;
	// 销毁线程池中的所有线程
	for (i = 1; i <= pool->active_threads; i++) {
		struct thread *thread = list_entry(pool->threads.next, struct thread, list);

    	// 使用 pthread_join 等待线程结束
   		int ret = pthread_join(thread->id, NULL);
    	if(ret != 0) {
        	printf("join id[%d] error: %s\n", i, strerror(errno));
    	} else {
        	printf("[%ld] is joined, i=%d\n", thread->id, i);
    	}
   		// 从线程池中删除已退出的线程
    	list_del(&thread->list);
    	free(thread);
	}

printf("\033[31m""---------Function: [%s]---------Line: [%d]---------\n""\033[m", __FUNCTION__, __LINE__);

	// 重置任务队列
	INIT_LIST_HEAD(&pool->queue);
	// 销毁锁和条件变量
	pthread_mutex_destroy(&pool->lock);
	pthread_cond_destroy(&pool->cond);

	// 释放线程池占用的内存
	// free(pool);			
}



