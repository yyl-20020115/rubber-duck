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
 
    std::map<int, int> hist{};
    for(int n=0; n<10000; ++n) {
        ++hist[(int)std::round(r->nextNormal(5,2))];
    }
    for(auto p : hist) {
        std::cout << std::setw(2)
                  << p.first << ' ' << std::string(p.second/200, '*') << '\n';
    }
}
