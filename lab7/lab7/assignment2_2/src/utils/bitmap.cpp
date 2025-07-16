#include "bitmap.h"
#include "stdlib.h"
#include "stdio.h"

BitMap::BitMap()
{
}

void BitMap::initialize(char *bitmap, const int length)
{
    this->bitmap = bitmap;
    this->length = length;

    int bytes = ceil(length, 8);

    for (int i = 0; i < bytes; ++i)
    {
        bitmap[i] = 0;
    }
}

bool BitMap::get(const int index) const
{
    int pos = index / 8;
    int offset = index % 8;

    return (bitmap[pos] & (1 << offset));
}

void BitMap::set(const int index, const bool status)
{
    int pos = index / 8;
    int offset = index % 8;

    // 清0
    bitmap[pos] = bitmap[pos] & (~(1 << offset));

    // 置1
    if (status)
    {
        bitmap[pos] = bitmap[pos] | (1 << offset);
    }
}

int BitMap::allocate(const int count)
{
    if (count == 0 || count > length)
        return -1;

    int bestStart = -1;
    int bestSize = length + 1; // 初始化为一个比可能的最大空闲区域还大的值

    int index = 0;
    while (index < length)
    {
        // 跳过已分配的位
        while (index < length && get(index))
            ++index;

        // 记录空闲区域的起始位置
        int start = index;
        
        // 计算当前空闲区域的大小
        int empty = 0;
        while (index < length && !get(index))
        {
            ++empty;
            ++index;
        }

        // 如果这个空闲区域能满足请求，并且比之前找到的最佳块更小
        if (empty >= count && empty < bestSize)
        {
            bestStart = start;
            bestSize = empty;
            
            // 如果找到一个正好大小的块，可以提前返回
            if (bestSize == count)
                break;
        }
    }

    // 如果找到合适的空闲区域
    if (bestStart != -1)
    {
        // 标记为已分配
        for (int i = 0; i < count; ++i)
        {
            set(bestStart + i, true);
        }
        return bestStart;
    }

    return -1; // 没有找到合适的空闲区域
}

void BitMap::release(const int index, const int count)
{
    for (int i = 0; i < count; ++i)
    {
        set(index + i, false);
    }
}

char *BitMap::getBitmap()
{
    return (char *)bitmap;
}

int BitMap::size() const
{
    return length;
}
