#include<iostream>
using namespace std;
#include<future>
#include"threadpool1.h"
/*
用函数的方式传入
1.pool.submitTask(sum1,10,20) pool.submitTask(sum2,10,20,30)  使用可变参模板编程
2.我们自己造了一个Result以及相关的类型，代码太多    使用C++ packaged_task(也是一种函数对象)
    使用future代替Result节省线程池代码

*/
int sum1(int a,int b)
{
    return a+b;
}

int main()
{
    ThreadPool pool;
    pool.start(4);

    future<int> r1 = pool.submitTask(sum1,10,10);
    cout<<"r1: "<<r1.get()<<endl;


    // packaged_task<int(int,int)> task(sum1);
    // // future 《=》 Result
    // future<int> res = task.get_future();
    // // task(10,20);
    // thread t(std::move(task),10,20);
    // t.detach();
    // cout<<res.get()<<endl;
    // return 0;
}