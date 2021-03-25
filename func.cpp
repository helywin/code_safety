//
// Created by jiang on 2021/3/24.
//

#include "func.hpp"
#include <cstring>
#include <cstdlib>
#include <cstdio>

// 访问不存在的内存地址
void use_nullptr()
{
    volatile int *a = (int *) (nullptr);
    *a = 1;
}

// 访问系统保护的内存地址
void access_system_memory()
{
    int *a = (int *)1000;
    *a = 10;
}

// 访问只读的内存地址
void access_readonly_memory()
{
    char *ptr = "test";
    strcpy(ptr, "TEST");
}

// 栈溢出
void stack_overflow()
{
    stack_overflow();
}

// 使用free掉的内存
void use_freed_memory()
{
    char* str=(char* )malloc(100);
    if(!str)
    {
        return;
    }
    strcpy(str,"hello");
    printf("%s\n",str);
    free(str);
    strcpy(str,"abcdef");
}

void memory_out_of_bounds()
{
    char a[5];
    printf("%c\n", a[100]);
}
