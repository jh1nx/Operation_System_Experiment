#include "Character_Rotation.h"
#include "stdio.h"
#include "os_modules.h"

extern STDIO stdio;

CharacterRotation::CharacterRotation(){
    initialize();
}

void CharacterRotation::initialize(){
    position=-2;
    direction=RIGHT;
    front_color=1;
    back_color=0x20;
    counter=0;
    color_counter=0;
    current_char=0;
    
    //初始化“数组”
    chars[0]='1';
    chars[1]='3';
    chars[2]='5';
    chars[3]='7';
    chars[4]='9';
    chars[5]='0';
    chars[6]='2';
    chars[7]='4';
    chars[8]='6';
    chars[9]='8';
    
    //显示学号
    char message[]="jhinx 23336266";
    for(int i=0;message[i]!='\0';i++){
        stdio.print(12,32+i,message[i],0xEC);
    }
}

void CharacterRotation::update_position(){
    uint newPos=position;
    
    switch(direction){
        case RIGHT:
            newPos+=2;
            if(newPos==160){
                //到达右边界
                newPos-=2;
                newPos+=160;//向下移动一行
                direction=DOWN;
            }
            break;
            
        case DOWN:
            newPos+=160;
            if(newPos>=4000){
                //到达下边界
                newPos-=160;
                newPos-=2;//向左移动一列
                direction=LEFT;
            }
            break;
            
        case LEFT:
            newPos-=2;
            if(newPos==3838){
                //到达左边界
                newPos+=2;
                newPos-=160;//向上移动一行
                direction=UP;
            }
            break;
            
        case UP:
            newPos-=160;
            if(newPos==-160){
                //到达上边界
                newPos+=160;
                newPos+=2;//向右移动一列
                direction=RIGHT;
            }
            break;
    }
    
    position=newPos;
}

void CharacterRotation::show_current_char(){
    //获取当前要显示的字符
    char ch=chars[current_char];
    
    //设置颜色属性
    uint8 color=back_color+front_color;
    
    //计算屏幕位置的行列
    uint row=position/160;
    uint col=(position%160)/2;
    
    //使用stdio显示字符
    stdio.print(row,col,ch,color);
    
    //更新颜色计数器
    color_counter++;
    if (color_counter>=3){
        color_counter=0;
        front_color++;
        if(front_color>=16){
            front_color=1;
        }
    }
    
    //更新字符索引
    current_char++;
    if(current_char>=10){
        current_char=0;
    }
    
    //更新计数器和背景色
    counter++;
    if(counter%14==0){
        back_color+=0x10;
        if(back_color==0){
            back_color=0x20;
        }
    }
}

void CharacterRotation::rotate(){
    update_position();
    show_current_char();
}
