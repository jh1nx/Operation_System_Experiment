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
    int pid = fork();
    int retval;

    if (pid)
    {
        printf("father process1 exit, pid: %d\n",programManager.running->pid);
        exit(123934);
    }
    else
    {
        uint32 tmp = 0xffffff;
        while (tmp)
            --tmp;
        printf("child process1 exit, pid: %d\n", programManager.running->pid);
        exit(-123);
    }
}

void second_process()
{
    uint32 tmp = 0x1ffffff;
    while (tmp)
        --tmp;
 
    int pid = fork();
    int retval;

    if (pid)
    {
        pid = fork();
        if (pid)
        {
            while ((pid = wait(&retval)) != -1)
            {
                printf("wait for a child process, pid: %d, return value: %d\n", pid, retval);
            }

            printf("all child process exit, programs: %d\n", programManager.allPrograms.size());
            
            asm_halt();
        }
        else
        {
            uint32 tmp = 0xffffff;
            while (tmp)
                --tmp;
            printf("grandchild process2 exit, pid: %d\n", programManager.running->pid);
            exit(2333);
        }
    }
    else
    {
        uint32 tmp = 0xffffff;
        while (tmp)
            --tmp;
        printf("child process2 exit, pid: %d\n", programManager.running->pid);
        exit(6266);
    }
}

void init_process()
{
    int retval;
    int pid;
    while (true)
    {
        while ((pid = wait(&retval)) != -1)
        {
            printf("recycle orphan process, pid: %d, return value: %d\n", pid, retval);
        }
    }
}

void first_thread(void *arg)
{

    printf("start process\n");
    programManager.executeProcess((const char *)init_process, 3);
    programManager.executeProcess((const char *)first_process, 3);
    programManager.executeProcess((const char *)second_process, 3);
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
