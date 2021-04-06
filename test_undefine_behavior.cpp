//
// Created by jiang on 2021/4/6.
//

#include <iostream>
#include <climits>

enum pets
{
    cat,
    dog,
    pig,
    bird,
};

#define bear 5

using std::cout;
using std::endl;

typedef void (*pfunc_void)(void);

typedef void (*pfunc_int)(int);

void func()
{
    std::cout << "non-parameter function" << endl;
}

int forget_return() {
    cout << "forget return" << endl;
}

int main(int argc, char *argv[])
{
// alignment
    int array0[] = {0, 1, 2, 3, 4};
    char *p = (char *) array0;
    cout << "unaligned data " << *(int *) (p + 1) << endl;
// bool
    bool a;
    if (a) {
        cout << "a is true" << endl;
    }
// bounds
    int array1[] = {0, 1, 2, 3, 4};
    int array2[] = {5, 6, 7, 8, 9};
    cout << "array1[5] is " << array1[5] << endl;

// enum
    pets mypet = (pets) bear;
    cout << "my pet is " << mypet << endl;
// float-cast-overflow
    int large_int = INT32_MAX - 5;
    float overflow_float = large_int;
    cout << "overflow float " << overflow_float << endl;

// function pointer
    pfunc_int f_int = (pfunc_int) func;
    f_int(1);

// implicit-unsigned-integer-truncation
// implicit-signed-integer-truncation
    short large_short = large_int;
    unsigned int large_uint = UINT32_MAX - 5;
    unsigned short large_ushort = large_uint;
    cout << "short " << large_short << " unsiged short " << large_ushort << endl;

// null
    int *null_int = 0;
    int &null = *null_int;
// pointer-overflow
    int *p_array = array1;
    p_array -= INT64_MAX;
    cout << "overflow pointer" << p_array << endl;
// return
    forget_return();
// shift
    unsigned char shift = 0x01;
    shift = shift << 10;
    shift = shift >> -1;
// signed-integer-overflow
    int overflow_int = INT32_MIN / -1;
    cout << "signed int overflow " << overflow_int << endl;
// unreachable
    if (0) {
        cout << "unreachable code" << endl;
    }
// vla-bound
    int v = 7;
    auto array3 = new int[v - 8];
}