/*

随机存储测试小程序

*/

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <chrono>
#include <ctime>
#include "../skiplist.h"

#define THREADS_NUMBER 1
#define MAX_MOD_COUNT 1000000

// 用于生成 随机key值
std::mt19937 g1(std::random_device{}());
std::uniform_int_distribution<int> diss(0, MAX_MOD_COUNT * 10); // 给定随机范围

// 初始化 skiplist
SkipList<int, std::string> *skiplist = SkipList<int, std::string>::getInstance();

void *worker_test_insert(void *arg)
{
    long index = (long)arg;
    int len = MAX_MOD_COUNT / THREADS_NUMBER;
    int start = (index)*len;
    for (int i = start, count = 0; count < len; ++count, ++i)
    {
        int t = diss(g1);
        skiplist->insert(i + t % len, "test");
    }
    pthread_exit(NULL);
}

void *worker_test_get(void *arg)
{
    long index = (long)arg;
    int len = MAX_MOD_COUNT / THREADS_NUMBER;
    int start = (index)*len;
    for (int i = start, count = 0; count < len; ++count, ++i)
    {
        int t = diss(g1);
        skiplist->find(i + t % len);
    }
    pthread_exit(NULL);
}

int main()
{
    skiplist->init();
    pthread_t threads[THREADS_NUMBER]; // 创建线程池

    auto start = std::chrono::steady_clock::now();

    // int ret;
    // for (int i = 0; i < THREADS_NUMBER; ++i)
    // {
    //     std::cout << "create threads : " << i + 1 << std::endl;
    //     ret = pthread_create(threads + i, NULL, worker_test_insert, (void *)i);

    //     if (ret != 0)
    //     {
    //         std::cout << "create threads fail.. " << std::endl;
    //         exit(-1);
    //     }
    // }

    // void *rett;
    // // 设置 jion 等待 线程全部运行结束 统计耗时
    // for (int i = 0; i < THREADS_NUMBER; ++i)
    // {
    //     if (pthread_join(threads[i], &rett) != 0)
    //     {
    //         perror("pthread join");
    //         exit(3);
    //     }
    // }

    // -------------------------------------查看元素测试-------------------------
    int ret;
    for (int i = 0; i < THREADS_NUMBER; ++i)
    {
        std::cout << "create threads : " << i + 1 << std::endl;
        ret = pthread_create(threads + i, NULL, worker_test_get, (void *)i);

        if (ret != 0)
        {
            std::cout << "create threads fail.. " << std::endl;
            exit(-1);
        }
    }

    void *rett;
    // 设置 jion 等待 线程全部运行结束 统计耗时
    for (int i = 0; i < THREADS_NUMBER; ++i)
    {
        if (pthread_join(threads[i], &rett) != 0)
        {
            perror("pthread join");
            exit(3);
        }
    }

    // ------------------------------------------------------------------------

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duratime = end - start;
    std::cout << "Total time:" << duratime.count() << std::endl;

    pthread_exit(NULL);
    return 0;
}