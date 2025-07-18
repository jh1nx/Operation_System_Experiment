#ifndef STDLIB_H
#define STDLIB_H

#include "os_type.h"

template<typename T>
void swap(T &x, T &y);

/*
 * 将一个非负整数转换为指定进制表示的字符串。
 * num: 待转换的非负整数。
 * mod: 进制。
 * numStr: 保存转换后的字符串，其中，numStr[0]保存的是num的高位数字，以此类推。
 */

void itos(char *numStr, uint32 num, uint32 mod);
void ftos(char *numStr, double num);
void ftos_only_intpart(char *numStr, double num);
void ftoe(char *numStr, double num);
#endif
