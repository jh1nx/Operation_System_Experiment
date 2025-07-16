#include "program.h"
#include "stdlib.h"
#include "interrupt.h"
#include "asm_utils.h"
#include "stdio.h"
#include "thread.h"
#include "os_modules.h"

const int PCB_SIZE = 4096;                   // PCB的大小，4KB。
char PCB_SET[PCB_SIZE * MAX_PROGRAM_AMOUNT]; // 存放PCB的数组，预留了MAX_PROGRAM_AMOUNT个PCB的大小空间。
bool PCB_SET_STATUS[MAX_PROGRAM_AMOUNT];     // PCB的分配状态，true表示已经分配，false表示未分配。

ProgramManager::ProgramManager()
{
    initialize();
}

void ProgramManager::initialize()
{
    allPrograms.initialize();
    readyPrograms.initialize();
    running = nullptr;

    for (int i = 0; i < MAX_PROGRAM_AMOUNT; ++i)
    {
        PCB_SET_STATUS[i] = false;
    }
}

int ProgramManager::executeThread(ThreadFunction function, void *parameter, const char *name, int priority)
{
    // 关中断，防止创建线程的过程被打断
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    // 分配一页作为PCB
    PCB *thread = allocatePCB();

    if (!thread)
        return -1;

    // 初始化分配的页
    memset(thread, 0, PCB_SIZE);

    for (int i = 0; i < MAX_PROGRAM_NAME && name[i]; ++i)
    {
        thread->name[i] = name[i];
    }

    thread->status = ProgramStatus::READY;
    thread->priority = priority;
    thread->ticks = priority * 10;
    thread->ticksPassedBy = 0;
    thread->pid = ((int)thread - (int)PCB_SET) / PCB_SIZE;

    // 线程栈
    thread->stack = (int *)((int)thread + PCB_SIZE);
    thread->stack -= 7;
    thread->stack[0] = 0;
    thread->stack[1] = 0;
    thread->stack[2] = 0;
    thread->stack[3] = 0;
    thread->stack[4] = (int)function;
    thread->stack[5] = (int)program_exit;
    thread->stack[6] = (int)parameter;

    allPrograms.push_back(&(thread->tagInAllList));
    readyPrograms.push_back(&(thread->tagInGeneralList));

    // 恢复中断
    interruptManager.setInterruptStatus(status);

    return thread->pid;
}

// 基于优先级的轮转调度算法
// void ProgramManager::schedule()
// {
//     bool status = interruptManager.getInterruptStatus();
//     interruptManager.disableInterrupt();

//     if (readyPrograms.size() == 0)
//     {
//         interruptManager.setInterruptStatus(status);
//         return;
//     }

//     if (running->status == ProgramStatus::RUNNING)
//     {
//         running->status = ProgramStatus::READY;
//         running->ticks = running->priority * 10;
//         readyPrograms.push_back(&(running->tagInGeneralList));
//     }
//     else if (running->status == ProgramStatus::DEAD)
//     {
//         releasePCB(running);
//     }

//     ListItem *item = readyPrograms.front();
//     PCB *next = ListItem2PCB(item, tagInGeneralList);
//     PCB *cur = running;
//     next->status = ProgramStatus::RUNNING;
//     running = next;
//     readyPrograms.pop_front();

//     asm_switch_thread(cur, next);

//     interruptManager.setInterruptStatus(status);
// }

// FIFO调度算法
// void ProgramManager::schedule()
// {
//     bool status = interruptManager.getInterruptStatus();
//     interruptManager.disableInterrupt();

//     if (readyPrograms.size() == 0)
//     {
//         interruptManager.setInterruptStatus(status);
//         return;
//     }

//     if (running->status == ProgramStatus::RUNNING)
//     {
//         running->status = ProgramStatus::READY;
//         readyPrograms.push_back(&(running->tagInGeneralList));
//     }
//     else if (running->status == ProgramStatus::DEAD)
//     {
//         releasePCB(running);
//     }

//     ListItem *item = readyPrograms.front();
//     PCB *next = ListItem2PCB(item, tagInGeneralList);
//     PCB *cur = running;
//     next->status = ProgramStatus::RUNNING;
//     running = next;
//     readyPrograms.pop_front();

//     asm_switch_thread(cur, next);

//     interruptManager.setInterruptStatus(status);
// }

// 非抢占式最短作业优先调度算法
// void ProgramManager::schedule()
// {
//     bool status = interruptManager.getInterruptStatus();
//     interruptManager.disableInterrupt();

//     if (readyPrograms.size() == 0)
//     {
//         interruptManager.setInterruptStatus(status);
//         return;
//     }

//     if (running->status == ProgramStatus::RUNNING)
//     {
//         running->status = ProgramStatus::READY;
//         readyPrograms.push_back(&(running->tagInGeneralList));
//     }
//     else if (running->status == ProgramStatus::DEAD)
//     {
//         releasePCB(running);
//     }

//     // 寻找执行时间最短的进程
//     ListItem *item = readyPrograms.front();
//     int shortest_ticks = 0x7FFFFFFF;
//     ListItem *selectedItem = nullptr;
//     PCB *selectedPCB = nullptr;

//     // 遍历就绪队列，找到剩余时间最短的进程
//     while (item)
//     {
//         PCB *temp = ListItem2PCB(item, tagInGeneralList);
//         if (temp->ticks < shortest_ticks)
//         {
//             selectedItem = item;
//             selectedPCB = temp;
//             shortest_ticks = temp->ticks;
//         }
//         item = item->next;
//     }

//     // 从就绪队列中移除选中的进程
//     if (selectedItem)
//     {
//         readyPrograms.erase(selectedItem);
//     }
    
//     PCB *cur = running;
//     if (selectedPCB)
//     {
//         selectedPCB->status = ProgramStatus::RUNNING;
//         running = selectedPCB;
//     }

//     if (selectedPCB)
//     {
//         asm_switch_thread(cur, selectedPCB);
//     }

//     interruptManager.setInterruptStatus(status);
// }

// 抢占式最短作业优先调度算法（最短剩余时间优先）
// void ProgramManager::schedule()
// {
//     bool status = interruptManager.getInterruptStatus();
//     interruptManager.disableInterrupt();

//     if (readyPrograms.size() == 0)
//     {
//         interruptManager.setInterruptStatus(status);
//         return;
//     }

//     // 寻找剩余执行时间最短的进程
//     ListItem *item = readyPrograms.front();
//     PCB *shortestJob = nullptr;
//     int shortestRemaining = 0x7FFFFFFF; // 初始化为最大整数值
//     ListItem *selectedItem = nullptr;

//     // 遍历就绪队列，找到剩余时间最短的进程
//     while (item)
//     {
//         PCB *temp = ListItem2PCB(item, tagInGeneralList);
//         if (temp->ticks < shortestRemaining)
//         {
//             shortestJob = temp;
//             shortestRemaining = temp->ticks;
//             selectedItem = item;
//         }
//         item = item->next;
//     }

//     // 获取当前运行进程的剩余时间（如果正在运行）
//     int currentRemaining = 0x7FFFFFFF;
//     if (running->status == ProgramStatus::RUNNING)
//     {
//         currentRemaining = running->ticks;
//     }

//     // 检查是否需要抢占当前进程
//     bool needPreempt = (shortestRemaining < currentRemaining) 
//     || (running->status != ProgramStatus::RUNNING);

//     if (needPreempt)
//     {
//         // 处理当前运行进程
//         if (running->status == ProgramStatus::RUNNING)
//         {
//             running->status = ProgramStatus::READY;
//             readyPrograms.push_back(&(running->tagInGeneralList));
//         }
//         else if (running->status == ProgramStatus::DEAD)
//         {
//             releasePCB(running);
//         }

//         // 从就绪队列中移除选中的进程
//         if (selectedItem)
//         {
//             readyPrograms.erase(selectedItem);
//         }
        
//         // 切换到新进程
//         PCB *cur = running;
//         shortestJob->status = ProgramStatus::RUNNING;
//         running = shortestJob;

//         asm_switch_thread(cur, shortestJob);
//     }

//     interruptManager.setInterruptStatus(status);
// }

// 非抢占式优先级调度算法
// void ProgramManager::schedule()
// {
//     bool status = interruptManager.getInterruptStatus();
//     interruptManager.disableInterrupt();

//     if (readyPrograms.size() == 0)
//     {
//         interruptManager.setInterruptStatus(status);
//         return;
//     }

//     if (running->status == ProgramStatus::RUNNING)
//     {
//         running->status = ProgramStatus::READY;
//         readyPrograms.push_back(&(running->tagInGeneralList));
//     }
//     else if (running->status == ProgramStatus::DEAD)
//     {
//         releasePCB(running);
//     }

//     // 寻找优先级最高的进程
//     ListItem *item = readyPrograms.front();
//     int highestPriority = 0x00000000;
//     ListItem *selectedItem = nullptr;
//     PCB *selectedPCB = nullptr;

//     // 遍历就绪队列，找到优先级最高的进程
//     while (item)
//     {
//         PCB *temp = ListItem2PCB(item, tagInGeneralList);
//         if (temp->priority > highestPriority)
//         {
//             selectedItem = item;
//             selectedPCB = temp;
//             highestPriority = temp->priority;
//         }
//         item = item->next;
//     }

//     if(selectedItem)
//     {
//         readyPrograms.erase(selectedItem);
//     }
    
//     PCB *cur = running;
//     if(selectedPCB)
//     {
//         selectedPCB->status = ProgramStatus::RUNNING;
//         running = selectedPCB;
//     }

//     if(selectedPCB)
//     {
//         asm_switch_thread(cur, selectedPCB);
//     }

//     interruptManager.setInterruptStatus(status);
// }

// 抢占式优先级调度算法
void ProgramManager::schedule()
{
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    if (readyPrograms.size() == 0)
    {
        interruptManager.setInterruptStatus(status);
        return;
    }

    // 寻找优先级最高的进程
    ListItem *item = readyPrograms.front();
    PCB *highestPriorityJob = nullptr;
    int highestPriority = 0x00000000;
    ListItem *selectedItem = nullptr;

    // 遍历就绪队列，找到优先级最高的进程
    while (item)
    {
        PCB *temp = ListItem2PCB(item, tagInGeneralList);
        if (temp->priority > highestPriority)
        {
            highestPriorityJob = temp;
            highestPriority = temp->priority;
            selectedItem = item;
        }
        item = item->next;
    }

    int currentPriority = 0x00000000;
    if (running->status == ProgramStatus::RUNNING){
        currentPriority = running->priority;
    }

    bool needPreempt = (highestPriority > currentPriority) 
    || (running->status != ProgramStatus::RUNNING);

    if (needPreempt){
        //处理当前运行进程
        if (running->status == ProgramStatus::RUNNING){
            running->status = ProgramStatus::READY;
            readyPrograms.push_back(&(running->tagInGeneralList));
        }
        else if (running->status == ProgramStatus::DEAD){
            releasePCB(running);
        }
        // 从就绪队列中移除选中的进程
        if (selectedItem){
            readyPrograms.erase(selectedItem);
        }
        // 切换到新进程
        PCB *cur = running;
        highestPriorityJob->status = ProgramStatus::RUNNING;
        running = highestPriorityJob;

        asm_switch_thread(cur, highestPriorityJob);
    }
    interruptManager.setInterruptStatus(status);
}

void program_exit()
{
    PCB *thread = programManager.running;
    thread->status = ProgramStatus::DEAD;

    if (thread->pid)
    {
        programManager.schedule();
    }
    else
    {
        interruptManager.disableInterrupt();
        printf("halt\n");
        asm_halt();
    }
}

PCB *ProgramManager::allocatePCB()
{
    for (int i = 0; i < MAX_PROGRAM_AMOUNT; ++i)
    {
        if (!PCB_SET_STATUS[i])
        {
            PCB_SET_STATUS[i] = true;
            return (PCB *)((int)PCB_SET + PCB_SIZE * i);
        }
    }

    return nullptr;
}

void ProgramManager::setThreadTicks(int pid, int ticks)
{
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    // 遍历所有程序查找指定pid
    ListItem *item = allPrograms.front();
    while (item)
    {
        PCB *program = ListItem2PCB(item, tagInAllList);
        if (program->pid == pid)
        {
            program->ticks = ticks;
            break;
        }
        item = item->next;
    }

    interruptManager.setInterruptStatus(status);
}

void ProgramManager::releasePCB(PCB *program)
{
    int index = ((int)program - (int)PCB_SET) / PCB_SIZE;
    PCB_SET_STATUS[index] = false;
}
