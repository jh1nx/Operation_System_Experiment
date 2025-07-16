#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

// 定义读者-写者问题的相关变量
int shared_resource = 0;     // 共享资源
int reader_count = 0;        // 当前读者数量
Semaphore mutex;             // 保护reader_count的互斥信号量
Semaphore wrt_lock;          // 写者锁
bool iswriting=0;

// 创建延迟函数，用于故意拖延时间，使读者写者问题更明显
void delay_function(int delay_time)
{
    while(delay_time--){
    	
    }
}
// 读者线程函数
void reader(void *arg)
{
    int id = (int)arg;
    while(true)
    {
        // 进入区 - 尝试获取访问权限
        printf("Reader %d is trying to enter...\n", id);
        if(iswriting){
            printf("Reader enter failed:there is writer writering...\n");
        }
        mutex.P();              // 获取对reader_count的互斥访问
        reader_count++;
        if(reader_count == 1)   // 第一个读者需要获取写锁
        {
            wrt_lock.P();
        }
        mutex.V();              // 释放对reader_count的互斥访问
        printf("Reader %d entered successfully\n", id);
        
        // 临界区 - 读取共享资源(只打印一次)
        printf("Reader %d is reading: %d\n", id, shared_resource);
        delay_function(0xfffffff); // 模拟读取操作耗时
        
        // 退出区
        printf("Reader %d is leaving...\n", id);
        mutex.P();              // 获取对reader_count的互斥访问
        reader_count--;
        if(reader_count == 0)   // 最后一个读者释放写锁
        {
            wrt_lock.V();
            printf("No reader is reading now...\n");
        }
        mutex.V();              // 释放对reader_count的互斥访问
        printf("Reader %d left successfully\n", id);
        
        delay_function(0xfffffff); // 模拟读者思考时间
    }
}

// 写者线程函数
void writer(void *arg)
{
    int id = (int)arg;
    
    while(true)
    {
        // 进入区 - 尝试获取写权限
        printf("Writer %d is trying to enter...\n", id);
        if(reader_count!=0){
            printf("Writer enter failed:there are readers reading...\n");
        }
        wrt_lock.P();           // 获取写锁
        iswriting=1;
        printf("Writer %d entered successfully\n", id);
        
        // 临界区 - 写入共享资源(只打印一次)
        shared_resource++;
        printf("Writer %d is writing: %d\n", id, shared_resource);
        delay_function(0xfffffff); // 模拟写入操作耗时
        
        // 退出区
        printf("Writer %d is leaving...\n", id);
        wrt_lock.V();           // 释放写锁
        iswriting=0;
        printf("Writer %d left successfully\n", id);
        
        delay_function(0xfffffff); // 模拟写者思考时间
    }
}

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);
    
    // 初始化信号量
    mutex.initialize(1);
    wrt_lock.initialize(1);
    
    // 初始化共享资源
    shared_resource = 0;
    
    // 创建读者和写者线程
    for(int i = 0; i < 2; i++)  // 创建2个读者
    {
        programManager.executeThread(reader, (void *)i, "reader", 1);
        delay_function(0xfffffff);
    }
    
    programManager.executeThread(writer, (void *)0, "writer", 1);
    delay_function(0xfffffff);
    
    programManager.executeThread(reader, (void *)2, "writer", 1);
    delay_function(0xfffffff);
    
    programManager.executeThread(writer, (void *)1, "writer", 1);
    delay_function(0xfffffff);
    
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
