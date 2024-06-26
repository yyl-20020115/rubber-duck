/**
 * @file Error.h
 * @brief 运行错误打印函数
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-03
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#ifndef __ERROR_H
#define __ERROR_H

namespace rubber_duck{

#define UNFRNG        0
#define NEGNMLMN      1
#define NEGNMLSD      2
#define NEGLNMLMN     3
#define NEGLNMLSD     4
#define NEGARGS       5
#define EQUZERO       6 
#define PROBVAL       7
#define RNGTRGMD      8 
#define DISCOUNT      9 
#define HISTUPPER     10 
#define HISTCELLS     11
#define CDFERROR      12
#define PROCUSCH      13
#define PROCSCH       14
#define NEGALPHA      15
#define TABMONO       16
#define TABINC        17
#define TABYRNG       18
#define NULLTAB       19

#define EVENTLISTNULL    0

/**
 * @brief 根据错误类型打印运行错误并退出仿真运行
 * @param  n      错误类型
 */
void err ( int n ) ;

/**
 * @brief 根据警告类型打印警告信息
 * @param  n      警告类型
 */
void warn ( int n );

}

#endif