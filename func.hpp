//
// Created by jiang on 2021/3/24.
//

#ifndef CODE_SECURITY_FUNC_HPP
#define CODE_SECURITY_FUNC_HPP

enum funcs {
    f_use_nullptr,
    f_access_system_memory,
    f_access_readonly_memory,
    f_stack_overflow,
    f_use_freed_memory,
    f_memory_out_of_bounds,
};

// 访问不存在的内存地址
void use_nullptr();

// 访问系统保护的内存地址
void access_system_memory();

// 访问只读的内存地址
void access_readonly_memory();

// 栈溢出
void stack_overflow();

// 使用free掉的内存
void use_freed_memory();

// 内存越界
void memory_out_of_bounds();

#endif //CODE_SECURITY_FUNC_HPP

