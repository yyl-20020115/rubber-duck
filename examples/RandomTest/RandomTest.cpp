/**
 * @file RandomTest.cpp
 * @brief 随机变量生成对象测试程序
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-03
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include "Random.h"

using namespace rubber_duck;

int main(){
    Random * r = new Random(123456);

    std::cout << "Triang(0,2,10)随机变量生成的直方图" << '\n';
    for(int n=0; n<1000; ++n) {
        std::cout << r->nextTriang(0,2,10) << ',';
    }
    std::cout << '\n';
}
