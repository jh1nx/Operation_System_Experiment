#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "memory.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 内存管理器
MemoryManager memoryManager;


extern "C" void setup_kernel()
{
    // 中断管理器
    interruptManager.initialize();

    // 输出管理器
    stdio.initialize();

    // 内存管理器
    memoryManager.openPageMechanism();
    
    // 尝试方法1：访问1MB以上内存	11111
    //*(int*)0x100000 = 1;
    
    // 尝试方法2：使用汇编直接触发页错误			11111
    // 通过直接调用会强制引发缺页异常的汇编指令
    //asm(
        //"movl $0x500000, %eax\n" // 5MB，可能未映射
        //"movl $1, (%eax)\n"      // 写入操作以触发错误
    //);
    
    // 尝试方法3：设置一个无效的指针并解引用	1111
    //int* badPtr = (int*)0xA0000000; // 远离已映射区域的地址
    //*badPtr = 123;
    
    // 尝试方法4：强制清零CR3寄存器（这将导致无效的页表基址）
    asm(
        "movl $0, %eax\n"
        "movl %eax, %cr3\n" // 设置无效的页目录表基址
    );
    
    // 尝试方法5：使用函数指针调用无效地址的函数		1111
    //typedef void (*FuncPtr)();
    //FuncPtr badFunction = (FuncPtr)0x12345678;
    //badFunction();
    
    // 尝试方法6：访问内核代码区域并尝试修改（只读区域）
    //uint8 *kernelCode = (uint8 *)0x20000; // 内核代码的起始地址
    //*kernelCode = 0x90; // 尝试修改内核代码（NOP指令）
    
    // 尝试方法7：尝试访问4GB虚拟地址空间边界
    //*(int*)(0xFFFFEFFF) = 1;
    
    // 尝试方法8：尝试从用户态切换到特权指令（在用户态执行特权指令）
    //asm(
      //  "pushf\n"          // 保存标志寄存器
        //"cli\n"            // 清除中断标志（特权指令）
        //"hlt\n"            // 停机（特权指令）
    //);
    
    // 尝试方法9：非对齐访问（某些架构会因此触发异常）
    //int *unaligned = (int*)0x10001;
    //*unaligned = 0x12345678;
    
    //尝试方法10：数组越界访问
    //int arr[5];
    //arr[254216] = 42;  // 越界访问    bianjie:254216
    
    //尝试方法11：修改只读内存
    //char *str = "hello";  // 字符串常量存储在只读段
    //str[0] = 'H';         // 尝试修改只读内存
    
    
    //尝试方法12：访问非法地址		1111
    //int *ptr = (int *)0x12345678;
    //*ptr = 42;   	
    
    //尝试方法13：执行非代码段		1111
    //void (*func)() = (void (*)())0xdeadbeef;
    //func();	// 跳转到非法地址执行
    
    // 尝试方法14：尝试访问其他进程的内存（通常会被操作系统阻止）	111
    //int *ptr = (int *)0x7ff000000;  // 假设是其他进程的地址
    //*ptr = 42;
    
    
    asm_halt(); // 这行代码不会执行，因为之前的代码会触发段错误
}

