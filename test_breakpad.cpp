//
// Created by jiang on 2021/3/24.
//


#include <breakpad/client/linux/handler/exception_handler.h>
#include <breakpad/client/linux/crash_generation/crash_generation_server.h>
#include "func.hpp"
#include <iostream>

using namespace google_breakpad;

static bool dumpCallback(const MinidumpDescriptor &descriptor,
                         void *context, bool succeeded)
{
    printf("dump存放位置: %s\n", descriptor.path());
    return succeeded;
}

int main(int argc, char *argv[])
{
    int client_fd = -1;
    int funcNum = -1;
    // 参数数目为0时client_fd为-1，在进程内生成dump
    if (argc > 1) {
        client_fd = strtol(argv[1], nullptr, 10);
    }
    if (argc > 2) {
        funcNum = strtol(argv[2], nullptr, 10);
    }
    MinidumpDescriptor descriptor("./");
    ExceptionHandler eh(descriptor, nullptr, dumpCallback, nullptr, true,
                        client_fd);

    if (funcNum == -1) {
        std::cout << "选择函数:\n"
                     "0 use_nullptr\n"
                     "1 access_system_memory\n"
                     "2 access_readonly_memory\n"
                     "3 stack_overflow\n"
                     "4 use_freed_memory\n"
                     "5 memory_out_of_bounds\n>(0-5)";
        std::cin >> funcNum;
    }

    switch (funcNum) {
        case f_use_nullptr:
            use_nullptr();
            break;
        case f_access_system_memory:
            access_system_memory();
            break;
        case f_access_readonly_memory:
            access_readonly_memory();
            break;
        case f_stack_overflow:
            stack_overflow();
            break;
        case f_use_freed_memory:
            use_freed_memory();
            break;
        case f_memory_out_of_bounds:
            memory_out_of_bounds();
            break;
        default:
            break;
    }
    return 0;
}

