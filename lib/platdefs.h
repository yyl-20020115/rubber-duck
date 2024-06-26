
/**
 * @file platdefs.h
 * @brief 编译器和平台相关的检查和定义
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2023-03-30
 * 
 * @copyright Copyright(C) 2023 liqun
 */

//==========================================================================
//   platdefs.h  -  header from
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================
#ifndef __PLATDEFS_H_
#define __PLATDEFS_H_

#include <cstddef>

#ifndef _WIN32
#  if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#    define _WIN32
#  endif
#endif

#endif

