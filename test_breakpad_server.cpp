//
// Created by jiang on 2021/3/24.
//

#include <breakpad/client/linux/handler/exception_handler.h>
#include <breakpad/client/linux/crash_generation/crash_generation_server.h>
#include <breakpad/client/linux/crash_generation/client_info.h>

using namespace google_breakpad;

void showClientDumpRequest(void *context,
                           const ClientInfo *client_info,
                           const string *file_path)
{
    printf("客户端dump请求, pid: %d, path: %s\n", client_info->pid(),
           file_path->data());
}

void showClientExited(void *context, const ClientInfo *client_info)
{
    printf("Client exited:\t\t%d\n", client_info->pid());
}

static bool dumpCallback(const MinidumpDescriptor &descriptor,
                         void *context, bool succeeded)
{
    printf("Dump path: %s\n", descriptor.path());
    return succeeded;
}

int server_fd;
int client_fd;

#include <iostream>
#include <cstring>
#include <wait.h>
#include "func.hpp"

int main(int argc, char *argv[])
{
    std::setlocale(LC_MESSAGES, "zh_CN.utf8");
    const std::string dumpPath = "./";
    // 创建
    CrashGenerationServer::CreateReportChannel(&server_fd, &client_fd);
    CrashGenerationServer crash_server(
            server_fd,
            &showClientDumpRequest,
            nullptr,
            &showClientExited,
            nullptr,
            true,
            &dumpPath);
    crash_server.Start();

    pid_t child_pid, wpid;
    int status = -1;

    std::cout << "父进程 pid = " << getpid() << std::endl;

    for (int i = f_use_nullptr; i <= f_memory_out_of_bounds; ++i) {
        char cmd[50];
        sprintf(cmd, "./test_breakpad %d 2", client_fd);

        child_pid = fork();
        if (child_pid == 0) {
            std::cout << "创建子进程 " << i << std::endl;
            std::cout << "子进程 pid = " << getpid() << std::endl;
            system(cmd);
            exit(5);
        } else if (child_pid > 0) {
        } else {
            std::cerr << "fork错误: " << std::strerror(errno) << std::endl;
        }
    }

    std::cout << "父进程挂起" << std::endl;

    while ((wpid = wait(&status)) > 0) {
        std::cout << "子进程 " << wpid << " 退出状态为 " << WEXITSTATUS(status) << std::endl;
    }

    return 0;
}

