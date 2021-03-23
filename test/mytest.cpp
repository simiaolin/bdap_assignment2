//
// Created by simiao on 2021/3/23.
//

#include "mytest.h"
#include <iostream>


// 函数定义
void swap(int& x, int& y)
{
    int temp;
    temp = x; /* 保存地址 x 的值 */
    x = y;    /* 把 y 赋值给 x */
    y = temp; /* 把 x 赋值给 y  */

    return;
}

void swap2(int* x, int* y)
{
    int* temp;
    temp = x; /* 保存地址 x 的值 */
    x = y;    /* 把 y 赋值给 x */
    y = temp; /* 把 x 赋值给 y  */

    return;
}


int main ()
{
    // 局部变量声明
    int a = 100;
    int b = 200;

    std::cout << "交换前，a 的值：" << a << std::endl;
    std::cout << "交换前，b 的值：" << b << std::endl;

    std::cout << "交换前，a 的地址：" << &a << std::endl;
    std::cout << "交换前，b 的地址：" << &b << std::endl;

    /* 调用函数来交换值 */
    swap(a, b);

    std::cout << "&交换后，a 的值：" << a << std::endl;
    std::cout << "&交换后，b 的值：" << b << std::endl;

    std::cout << "交换后，a 的地址：" << &a << std::endl;
    std::cout << "交换后，b 的地址：" << &b << std::endl;

    int* p1 = &a;
    int* p2 = &b;

    swap2(p1, p2);
    std::cout << "*交换后，a 的值：" << *p1 << std::endl;
    std::cout << "*交换后，b 的值：" << *p2 << std::endl;

    return 0;
}

