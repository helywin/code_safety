//
// Created by jiang on 2021/3/26.
//

#include <iostream>

void func1()
{
    std::cout << "func1" << std::endl;
}

void func2()
{
    std::cout << "func2" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc == 0) {
        func1();
    } else {
        func2();
    }
    int a = 2;
    if (a = 3) {
        a = 4;
    }
}