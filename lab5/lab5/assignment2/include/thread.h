#ifndef THREAD_H
#define THREAD_H

#include "list.h"
#include "os_constant.h"

typedef void (*ThreadFunction)(void *);

enum ProgramStatus
{
    CREATED,
    RUNNING,
    READY,
    BLOCKED,
    DEAD
};

struct PCB
{
    int *stack;                      // 栈指针，用于调度时保存esp
    char name[MAX_PROGRAM_NAME + 1]; // 线程名
    enum ProgramStatus status;       // 线程的状态
    int priority;                    // 线程优先级
    int pid;                         // 线程pid
    int ticks;                       // 线程时间片总时间
    int ticksPassedBy;               // 线程已执行时间
    ListItem tagInGeneralList;       // 线程队列标识
    ListItem tagInAllList;           // 线程队列标识
    int parent_pid;		     // 父进程 pid
    List childrenList;		     // 子进程链表（存储子进程的 PCB 指针）
    ListItem tagInChildrenList;	     // 子进程队列标识
    int createTime;		     // 进程创建时间
};

#endif
