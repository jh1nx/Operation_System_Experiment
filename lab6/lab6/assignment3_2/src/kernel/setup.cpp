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

const int PHILOSOPHER_NUM = 5;  // 哲学家数量
Semaphore forks[PHILOSOPHER_NUM];  // 叉子信号量
Semaphore table;  // 限制同时就餐的哲学家数量

void delay(int cycles) {
    while (cycles--);
}

void philosopher(void *arg) {
    int id = *(int *)arg;
    int left_fork = id;
    int right_fork = (id + 1) % PHILOSOPHER_NUM;
    
    while (true) {
        // 思考
        printf("Philosopher %d is thinking...\n", id);
        delay(0x7ffffff);  // 思考时间
        
        // 尝试就餐
        printf("Philosopher %d is hungry, trying to get forks...\n", id);
        table.P();  // 限制同时就餐人数
        
        forks[left_fork].P();  // 拿左叉子
        printf("Philosopher %d picked up left fork %d\n", id, left_fork);
        delay(0x3ffffff);  
        
        programManager.schedule();
        
        forks[right_fork].P();  // 拿右叉子
        printf("Philosopher %d picked up right fork %d\n", id, right_fork);
        delay(0x3ffffff);  
        
        // 就餐
        printf("Philosopher %d is eating...\n", id);
        delay(0xfffffff);  //就餐时间
        
        // 放下叉子
        forks[right_fork].V();
        printf("Philosopher %d put down right fork %d\n", id, right_fork);
        delay(0x3ffffff); 
        
        forks[left_fork].V();
        printf("Philosopher %d put down left fork %d\n", id, left_fork);
        delay(0x3ffffff); 
        
        table.V();  // 释放就餐位置
        printf("Philosopher %d finished eating and starts thinking again\n", id);
    }
}

void first_thread(void *arg) {
    // 清屏
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i) {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    // 初始化信号量
    table.initialize(PHILOSOPHER_NUM - 1);  // 最多允许PHILOSOPHER_NUM-1个哲学家同时就餐
    for (int i = 0; i < PHILOSOPHER_NUM; ++i) {
        forks[i].initialize(1);  // 每个叉子初始可用
    }

    // 创建哲学家线程
    int ids[PHILOSOPHER_NUM];
    for (int i = 0; i < PHILOSOPHER_NUM; ++i) {
        ids[i] = i;
        programManager.executeThread(philosopher, &ids[i], "philosopher", 1);
    }

    asm_halt();
}

extern "C" void setup_kernel() {
    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 2);
    if (pid == -1) {
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
