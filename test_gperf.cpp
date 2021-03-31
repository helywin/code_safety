//
// Created by jiang on 2021/3/31.
//

#include <stdlib.h>
#include <iostream>
#include <gperftools/profiler.h>
#include <gperftools/heap-profiler.h>
#include <gperftools/heap-checker.h>


void *create(unsigned int size)
{
    return malloc(size);
}

void create_destory(unsigned int size)
{
    void *p = create(size);
    free(p);
}

void much_effort()
{
    int sum = 0;
    for (int i = 0; i < 700000000; ++i) {
        sum += i;
    }
    std::cout << "sum:" << sum << std::endl;
}

int main(void)
{
//    HeapProfilerStart("heap.perf");
//    ProfilerStart("cup.perf");
    HeapLeakChecker leakChecker("checker");
    const int loop = 4;
    char *a[loop];
    unsigned int mega = 1024 * 1024;

    for (int i = 0; i < loop; i++) {
        const unsigned int create_size = 1024 * mega;
        create(create_size);

        const unsigned int malloc_size = 1024 * mega;
        a[i] = (char *) malloc(malloc_size);

        const unsigned int create_destory_size = mega;
        create_destory(create_destory_size);
    }

    for (int i = 0; i < loop; i++) {
        free(a[i]);
    }

    much_effort();
    int s = 0;
    for (int i = 0; i < 300000000; ++i) {
        {
            s += i;
        }
    }
    std::cout << "int main s:" << s << std::endl;

//    ProfilerStop();
//    HeapProfilerStop();
    return 0;
}

