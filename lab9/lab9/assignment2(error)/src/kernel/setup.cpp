#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"
#include "syscall.h"
#include "tss.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;
// 系统调用
SystemService systemService;
// Task State Segment
TSS tss;

int syscall_0(int first, int second, int third, int forth, int fifth)
{
    printf("systerm call 0: %d, %d, %d, %d, %d\n",
           first, second, third, forth, fifth);
    return first + second + third + forth + fifth;
}

void first_process()
{
    printf("\nstart process\n");
    printf("test malloc/free in process\n");
    
    char *str = (char *)malloc(32);
    if (str) 
    {
        printf("Process successfully malloc memory, address: 0x%x\n", (uint32)str);
        
        const char *message = "23336266 jhinx!";
        int len = 0;
        while (message[len]) len++; 
        
        if (len < 32) 
        { 
            for (int i = 0; i <= len; i++) 
            {
                str[i] = message[i];
            }
        }
        
        printf("Allocated memory content: %s\n", str);
        free(str);
        printf("Process successfully free memory.\n");
    } 
    else 
    {
        printf("Process malloc failed\n");
    }

    // 测试大内存分配
    printf("Trying to allocate large memory...\n");
    char *largeStr = (char *)malloc(1024 * 1024); // 1MB
    if (largeStr) 
    {
        printf("Process successfully malloc large memory, address: 0x%x\n", (uint32)largeStr);
        free(largeStr);
        printf("Process successfully free large memory.\n");
    } 
    else 
    {
        printf("Process malloc large memory failed.\n");
    }
}

void second_thread(void *arg)
{
    printf("\nsecond thread\n");
    char *str = (char *)malloc(32);
    if (str) {
        printf("Thread 2 successfully malloc memory: 0x%x\n", (uint32)str);
        free(str);
        printf("Thread 2 successfully free memory.\n");
    }
}

void first_thread(void *arg)
{
    printf("\nstart thread\n");
    printf("test malloc/free in thread.\n");
    programManager.executeThread(second_thread, nullptr, "second", 1);

    // 先在线程中测试内存分配
    char *str = (char *)malloc(32);
    if (str) {
        printf("Thread 1 successfully malloc memory: 0x%x\n", (uint32)str);
        free(str);
        printf("Thread 1 successfully free memory.\n");
    }
    asm_halt();
}

extern "C" void setup_kernel()
{

    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 初始化内核字节内存管理器
    kernelByteMemoryManager.initialize();

    // 初始化系统调用
    systemService.initialize();
    // 设置0号系统调用
    systemService.setSystemCall(0, (int)syscall_0);
    // 设置1号系统调用
    systemService.setSystemCall(1, (int)syscall_write);
    // 设置2号系统调用
    systemService.setSystemCall(2, (int)syscall_fork);
    // 设置3号系统调用
    systemService.setSystemCall(3, (int)syscall_exit);
    // 设置4号系统调用
    systemService.setSystemCall(4, (int)syscall_wait);
    // 设置5号系统调用
    systemService.setSystemCall(5, (int)syscall_malloc);
    // 设置6号系统调用
    systemService.setSystemCall(6, (int)syscall_free);

    // 内存管理器
    memoryManager.initialize();

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = ProgramStatus::RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}
