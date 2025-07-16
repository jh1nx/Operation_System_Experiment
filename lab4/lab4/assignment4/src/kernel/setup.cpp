#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "Character_Rotation.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 字符旋转器
CharacterRotation my_program;

extern "C" void setup_kernel()
{
    // 中断处理部件
    interruptManager.initialize();
    // 屏幕IO处理部件
    stdio.initialize();
    //stdio.rollUp();
    //stdio.rollUp();
    //stdio.rollUp();
    //stdio.rollUp();
    //stdio.rollUp();
    //stdio.rollUp();
    //stdio.rollUp();
    stdio.rollUp();
    // 字符回旋程序初始化
    my_program.initialize();
    
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);
    asm_enable_interrupt();
    asm_halt();
}


