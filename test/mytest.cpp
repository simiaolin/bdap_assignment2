//
// Created by simiao on 2021/3/23.
//

#include "mytest.h"
#include <iostream>
#include <vector>


using std::vector;
using std::cout;
using std::endl;
using Row = std::vector<std::string>;
using Data = std::vector<Row>;
//using namespace std::placeholders;

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
bool sorter(Row row1, Row row2, int col) {
    return row1.at(col) > row2.at(col);
}
void testSortConstData() {
    Row r1{"weak", "2", "yes"};
    Row r2{"strong", "4", "no"};
    Row r3{"strong", "3", "yes"};
    Row r4{"acute", "1", "yes"};
    const Data data {r1, r3, r2, r4};
    Data* temp = (Data*) &data;
//    sort(temp->begin(), temp->end(), std::bind(sorter, _1, _2, 0));
    for (auto v : data) {
        for (std::string s : v) {
            std::cout<<s<< " ";
        }
        std::cout<< "\n";
    }



}
void testSortConstVector() {
    const std::vector<int> v = {5,4,3,2,1};
    std::vector<int>*temp=(std::vector<int>*)&v;
//    sort(temp->begin(),temp->end());
    for(int a:v)
        cout<<a<<" ";
    cout<<endl;
}

int main() {
    std::string a = "A";
    std::string b = "A";

    bool c = a==b;

    return 0;
}

int testSwap ()
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

