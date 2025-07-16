#ifndef CHARACTER_ROTATION_H
#define CHARACTER_ROTATION_H

#include "os_type.h"

// 移动方向枚举
enum Direction{
    RIGHT=0,
    DOWN=1,
    LEFT=2,
    UP=3
};

class CharacterRotation{
private:
    //光标当前位置
    uint position;
    //光标当前前进方向
    Direction direction;
    //字符前景色
    uint8 front_color;
    //字符背景色
    uint8 back_color;
    //计数器:记录当前背景色已经输出了多少个字符
    uint counter;
    //计数器:记录当前前景色已经输出了多少个字符
    uint8 color_counter;
    //当前字符索引:记录现在是“数组”中的第几个数
    uint8 current_char;
    //“数组”：用于存储伪随机数
    char chars[10];
    
public:
    CharacterRotation();
    void initialize();
    //更新位置
    void update_position();
    //显示当前字符
    void show_current_char();
    //在时钟中断中调用的主要方法
    void rotate();
};

#endif
