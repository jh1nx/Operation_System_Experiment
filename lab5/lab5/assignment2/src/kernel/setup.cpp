#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

void grand_child_thread(void *arg){
	printf("pid %d name \"%s\": I am the grand child thread of %d! ... ", 
        programManager.running->pid, programManager.running->name,programManager.running->parent_pid);
    printf("createtime: %d\n", programManager.running->createTime);
    while(1){};
}

void child_thread(void *arg){
	printf("pid %d name \"%s\": I am the child thread of %d! ... ", 
        programManager.running->pid, programManager.running->name,programManager.running->parent_pid);
    printf("createtime: %d\n", programManager.running->createTime);
    
    programManager.executeThread(grand_child_thread, nullptr, "grand child", 1);
    
    printf("now show the childrenList of the child of thread %d:",programManager.running->pid);
    ListItem *item = programManager.running->childrenList.front();
    while (item) {
        PCB *child = ListItem2PCB(item, tagInChildrenList);
        printf("%d ", child->pid);
        item = item->next;
    }
    printf("\n");
    while(1){};
}

void third_thread(void *arg) {
    printf("pid %d name \"%s\": I am the third thread! ... ", 
        programManager.running->pid, programManager.running->name);
    printf("createtime: %d\n", programManager.running->createTime);
    while(1){};
}

void second_thread(void *arg) {
    printf("pid %d name \"%s\": I am the second thread! ... ", 
        programManager.running->pid, programManager.running->name);
    printf("createtime: %d\n", programManager.running->createTime);
    
    programManager.executeThread(child_thread, nullptr, "first child", 1);
    programManager.executeThread(child_thread, nullptr, "second child", 1);
    
    printf("now show the childrenList of second_thread:");
    ListItem *item = programManager.running->childrenList.front();
    while (item) {
        PCB *child = ListItem2PCB(item, tagInChildrenList);
        printf("%d ", child->pid);
        item = item->next;
    }
    printf("\n");
    while(1){};
}
void first_thread(void *arg)
{
    // 第1个线程不可以返回
    printf("pid %d name \"%s\": I am the first_thread! ... ", 
        programManager.running->pid, programManager.running->name);
    printf("createtime: %d\n", programManager.running->createTime);
    
    programManager.executeThread(child_thread, nullptr, "first child", 1);
	programManager.executeThread(child_thread, nullptr, "second child", 1);
	programManager.executeThread(child_thread, nullptr, "third child", 1);
    
    printf("now show the childrenList of first_thread:");
    ListItem *item = programManager.running->childrenList.front();
    while (item) {
        PCB *child = ListItem2PCB(item, tagInChildrenList);
        printf("%d ", child->pid);
        item = item->next;
    }
    printf("\n");

    
    if (!programManager.running->pid)
    {
        programManager.executeThread(second_thread, nullptr, "second thread", 1);
        programManager.executeThread(third_thread, nullptr, "third thread", 1);
    }
    while(1){};
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
