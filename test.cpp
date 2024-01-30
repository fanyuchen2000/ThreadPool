#include <iostream>
#include <chrono>
#include <thread>

#include "threadpool.h"

/*
有些场景，是希望能够获取线程执行任务得返回值
举例：
1+.............+30000的和
thread1 1+...+10000
thread2 10001+...+20000
...
main thread: 给每个线程分配计算得区间，并等待他们算完返回结果，合并最终结果即可
*/

using ULong = unsigned long long;

class MyTask : public Task
{
public:
    MyTask(int begin, int end) : begin_(begin), end_(end)
    {
    }
    // 问题一:怎么设计run函数的返回值，可以表示任意类型
    Any run()
    {
        std::cout << "tid" << std::this_thread::get_id() << " begin " << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        ULong sum = 0;
        for (ULong i = begin_; i <= end_; i++)
            sum += i;
        std::cout << "tid" << std::this_thread::get_id() << " end " << std::endl;
        return sum;
    }

private:
    int begin_;
    int end_;
};

int main()
{
    {
        ThreadPool pool;
        pool.setMood(PoolMood::MOOD_CACHED);
        pool.start(2);
        Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 10000000));
        pool.submitTask(std::make_shared<MyTask>(20000001, 30000000));
        pool.submitTask(std::make_shared<MyTask>(20000001, 30000000));
        pool.submitTask(std::make_shared<MyTask>(20000001, 30000000));
        pool.submitTask(std::make_shared<MyTask>(20000001, 30000000));
        pool.submitTask(std::make_shared<MyTask>(20000001, 30000000));
        pool.submitTask(std::make_shared<MyTask>(20000001, 30000000));
        ULong sum1 = res1.get().cast_<ULong>();
        std::cout << "sum1:" << sum1 << std::endl;
    }

    std::cout << "main over " << std::endl;
    getchar();
    return 0;
}

int main1()
{
    // 问题：ThreadPool对象析构以后，怎么样线程池相关的线程资源全部回收？
    {
        ThreadPool pool;
        // 用户自己设置线程池的工作模式
        pool.setMood(PoolMood::MOOD_CACHED);
        // 开始启动线程池
        pool.start(4);

        // 如何设计这里的Rerult机制呢
        Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 10000000));
        Result res2 = pool.submitTask(std::make_shared<MyTask>(10000001, 20000000));
        Result res3 = pool.submitTask(std::make_shared<MyTask>(20000001, 30000000));
        pool.submitTask(std::make_shared<MyTask>(20000001, 30000000));

        pool.submitTask(std::make_shared<MyTask>(20000001, 30000000));
        pool.submitTask(std::make_shared<MyTask>(20000001, 30000000));

        ULong sum1 = res1.get().cast_<ULong>(); // get返回了一个Any类型，怎么转成具体的类型呢
        ULong sum2 = res2.get().cast_<ULong>();
        ULong sum3 = res3.get().cast_<ULong>();
        // Master - Slave线程模型
        // Master线程用来分解任务，然后给各个Salve线程分配任务
        // 等待各个Slave线程执行完任务，返回结果
        // Master线程合并各个任务结果，输出
        std::cout << (sum1 + sum2 + sum3) << std::endl;

        // pool.submitTask(std::make_shared<MyTask>());
        // pool.submitTask(std::make_shared<MyTask>());
        // pool.submitTask(std::make_shared<MyTask>());
        // pool.submitTask(std::make_shared<MyTask>());

        getchar();
        // std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}