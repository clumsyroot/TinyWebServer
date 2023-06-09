#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

// 封装 POSIX 信号量: 初始化、等待和释放
class sem
{
public:
    sem()
    {
        // int sem_init(sem_t *__sem, int __pshared, unsigned int __value)
        // `sem` --> 指向要初始的信号量的指针
        // `__pshared` --> 表示信号量的共享类型，为 0 表示信号量是进程内可见的，即只能在同一进程的线程中使用，非 0 表示信号量是进程间共享的，默认为 0
        // `__value` --> 信号量的初始值，必须为非负整数，表示资源总量
        // sem_init() 返回值为 0 表示初始化成功，返回其他值表示初始化失败
        if (sem_init(&m_sem, 0, 0) != 0)
        {
            throw std::exception();
        }
    }
    sem(int num)
    {
        if (sem_init(&m_sem, 0, num) != 0)
        {
            throw std::exception();
        }
    }
    ~sem()
    {
        sem_destroy(&m_sem);
    }
    bool wait()
    {
        return sem_wait(&m_sem) == 0;
    }
    bool post()
    {
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};
class locker
{
public:
    locker()
    {
        // pthread_mutex_init() 返回 0 表示初始化成功
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }
    ~locker()
    {
        // 销毁互斥锁
        pthread_mutex_destroy(&m_mutex);
    }
    bool lock()
    {
        // pthread_mutex_lock() 获取互斥锁
        return pthread_mutex_lock(&m_mutex) == 0;
    }
    bool unlock()
    {
        // pthread_mutex_unlock() 释放互斥锁
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
    pthread_mutex_t *get()
    {
        return &m_mutex;
    }

private:
    // pthread_mutex_t 结构体类型 包含一些互斥锁相关的信息和状态
    pthread_mutex_t m_mutex;
};
class cond
{
public:
    cond()
    {
        if (pthread_cond_init(&m_cond, NULL) != 0)
        {
            // pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }
    ~cond()
    {
        pthread_cond_destroy(&m_cond);
    }
    bool wait(pthread_mutex_t *m_mutex)
    {
        int ret = 0;
        // pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, m_mutex);
        // pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    bool timewait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        int ret = 0;
        // pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
        // pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }
    bool broadcast()
    {
        return pthread_cond_broadcast(&m_cond) == 0;
    }

private:
    // static pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};
#endif
