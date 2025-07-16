#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;

void thread1(void *arg){
    char *p1=(char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    printf("thread 1 allocate pages:%x\n",p1);
}

void thread2(void *arg){
    char *p2=(char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    printf("thread 2 allocate pages:%x\n",p2);
}
void thread3(void *arg){
    char *p3=(char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    printf("thread 3 allocate pages:%x\n",p3);
}
void first_thread(void *arg)
{
    programManager.executeThread(thread1, nullptr, "thread1", 1);
    programManager.executeThread(thread2, nullptr, "thread2", 1);
    programManager.executeThread(thread3, nullptr, "thread3", 1);
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

    // 内存管理器
    memoryManager.openPageMechanism();
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
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}

