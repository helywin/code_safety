//
// Created by jiang on 2021/3/25.
//

#include <cstdlib>
#include <memory.h>
#include <iostream>
#include <string>
#include <iomanip>

class object
{

};

class buggy_object : public object
{
public:
    int *data;

    buggy_object()
    {
        data = new int[50];
    }
};


class normal_object : public object
{
public:
    int *data;

    normal_object()
    {
        data = new int[50];
    }

    ~normal_object()
    {
        std::cout << "free data" << std::endl;
        delete[] data;
    }
};


void func()
{
    const int buff_size = 20;
    auto buff = new char[buff_size];
    memcpy(buff + 5, "dddddd", 30);
    for (int i = 0; i < buff_size + 5; ++i) {
        std::cout << buff[i] << " ";
    }
    std::cout << std::endl;
    delete[] buff;
    buff[1] = 'c';
    auto buggy = new buggy_object;
    delete buggy;
    auto normal = (object *) new normal_object;
    delete normal;
}

int main(int argc, char *argv[])
{
    func();
    int global;
    if (!global) {
        std::cout << global << std::endl;
    }
    std::cout << "finished" << std::endl;
    return 0;
}