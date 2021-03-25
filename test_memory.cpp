//
// Created by jiang on 2021/3/25.
//

#include <cstdlib>
#include <memory.h>
#include <iostream>

class object
{
public:
    int *data;

    object()
    {
        data = new int[50];
    }
};

void func () {
    auto buff = new char[10];
    memcpy(buff + 5, "dddddd", 10);
    delete[] buff;
    buff[1] = 'c';
    auto obj = new object;
    delete obj;
}

int main(int argc, char *argv[])
{
    func();
    std::cout << "finished" << std::endl;
    return 0;
}