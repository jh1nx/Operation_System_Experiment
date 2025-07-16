#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;


extern "C" void setup_kernel()
{
    // 中断处理部件
    interruptManager.initialize();
    // 屏幕IO处理部件
    stdio.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);
    //asm_enable_interrupt();
    printf("print percentage: %%\n"
           "print char \"N\": %c\n"
           "print string \"Hello World!\": %s\n"
           "print decimal \"-1234\": %d\n"
           "print hexadecimal \"0x7abcdef0\": %x\n"
           "print float \"3.1415\": %f\n"
           "print float \"3.0\": %f\n"
           "print float \"-0.01\": %f\n"
           "print float \"-4.22\": %f\n"
           "print round_off_float \"3.53\": %.f\n"
           "print round_off_float \"3.49\": %.f\n"
           "print round_off_float \"-0.01\": %.f\n"
           "print round_off_float \"-0.6\": %.f\n"
           "print index_float \"123.6\": %e\n"
           "print index_float \"1.4635\": %e\n"
           "print index_float \"0.00373\": %e\n"
           "print index_float \"-33.66\": %e\n"
           "print index_float \"-0.0654\": %e\n",
           'N', "Hello World!", -1234, 0x7abcdef0,3.1415,3.0,-0.01,-4.22,3.53,3.49,-0.01,-0.6,123.6,1.4635,0.00373,-33.66,-0.0654);
    //uint a = 1 / 0;
    asm_halt();
}

