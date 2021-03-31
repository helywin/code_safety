//
// Created by jiang on 2021/3/30.
//

#include <pthread.h>
#include <iostream>
#include <thread>

int Global;

void *Thread1(void *x)
{
    Global = 42;
    return x;
}

int thread_num = 0;

void thread_1()
{
    thread_num += 1;
    std::cout << "thread num: " << thread_num << std::endl;
}

int main()
{
#if defined(__has_feature)
#  if __has_feature(thread_sanitizer)
    std::cout << "__has_feature(thread_sanitizer)" << std::endl;
#  endif
#endif
    pthread_t t;
    pthread_create(&t, nullptr, Thread1, nullptr);
    Global = 43;
    pthread_join(t, NULL);

    std::thread t1(thread_1);
    std::thread t2(thread_1);
    std::thread t3(thread_1);
    t1.join();
    t2.join();
    t3.join();
    return Global;
}