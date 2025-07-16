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

    // 设置父子进程关系
    PCB *current = running;
    thread->parent_pid = current ? current->pid : -1;
    thread->childrenList.initialize();
    thread->tagInChildrenList.next = nullptr;
    thread->tagInChildrenList.previous = nullptr;
    
    if (current) {
        current->childrenList.push_back(&(thread->tagInChildrenList));
    }

    // 设置创建时间
    extern int times;
    thread->createTime = times;

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

void ProgramManager::schedule()
{
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    if (readyPrograms.size() == 0)
    {
        interruptManager.setInterruptStatus(status);
        return;
    }

    if (running->status == ProgramStatus::RUNNING)
    {
        running->status = ProgramStatus::READY;
        running->ticks = running->priority * 10;
        readyPrograms.push_back(&(running->tagInGeneralList));
    }
    else if (running->status == ProgramStatus::DEAD)
    {
        releasePCB(running);
    }

    ListItem *item = readyPrograms.front();
    PCB *next = ListItem2PCB(item, tagInGeneralList);
    PCB *cur = running;
    next->status = ProgramStatus::RUNNING;
    running = next;
    readyPrograms.pop_front();

    asm_switch_thread(cur, next);

    interruptManager.setInterruptStatus(status);
}

void program_exit()
{
    // 关中断
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();
    
    PCB *thread = programManager.running;
    thread->status = ProgramStatus::DEAD;

    // 处理父子进程关系
    if (thread->parent_pid != -1) {
        // 如果有父进程，从父进程的子进程列表中移除自己
        PCB *parent = programManager.find_thread_ByPid(thread->parent_pid);
        if (parent) {
            parent->childrenList.erase(&(thread->tagInChildrenList));
        }
    }

    // 处理子进程
    ListItem *item = thread->childrenList.front();
    while (item) {
        // 找到子进程的PCB
        PCB *child = ListItem2PCB(item, tagInChildrenList);
        // 将子进程的父进程设为-1（成为孤儿进程）
        child->parent_pid = -1;
        
        item = item->next;
    }

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

void ProgramManager::releasePCB(PCB *program)
{
    int index = ((int)program - (int)PCB_SET) / PCB_SIZE;
    PCB_SET_STATUS[index] = false;
}

PCB *ProgramManager::find_thread_ByPid(int pid)
{
    ListItem *item = allPrograms.front();
    while (item != nullptr)
    {
        PCB *pcb = ListItem2PCB(item, tagInAllList);
        if (pcb->pid == pid)
        {
            return pcb;
        }
        item = item->next;
    }
    return nullptr;
}
