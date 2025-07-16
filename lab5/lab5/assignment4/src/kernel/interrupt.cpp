#include "interrupt.h"
#include "os_type.h"
#include "os_constant.h"
#include "asm_utils.h"
#include "stdio.h"
#include "os_modules.h"
#include "program.h"


extern void sixth_thread(void *arg) {
    printf("pid %d name \"%s\": thread6 is created! \n", programManager.running->pid, programManager.running->name);
    asm_halt();
}

extern void fifth_thread(void *arg) {
    printf("pid %d name \"%s\": thread5 is created! \n", programManager.running->pid, programManager.running->name);
    asm_halt();
}

extern void forth_thread(void *arg) {
    printf("pid %d name \"%s\": thread4 is created! \n", programManager.running->pid, programManager.running->name);
    asm_halt();
}


void third_thread(void *arg) {
    printf("pid %d name \"%s\": thread3 is created! \n", programManager.running->pid, programManager.running->name);
    asm_halt();
}

void second_thread(void *arg) {
    printf("pid %d name \"%s\": thread2 is created! \n", programManager.running->pid, programManager.running->name);
    asm_halt();
}

void first_thread(void *arg) {
    printf("pid %d name \"%s\": thread1 is created! \n", programManager.running->pid, programManager.running->name);
    asm_halt();
}

int times = 0;

InterruptManager::InterruptManager()
{
    initialize();
}

void InterruptManager::initialize()
{
    // 初始化中断计数变量
    times = 0;
    
    // 初始化IDT
    IDT = (uint32 *)IDT_START_ADDRESS;
    asm_lidt(IDT_START_ADDRESS, 256 * 8 - 1);

    for (uint i = 0; i < 256; ++i)
    {
        setInterruptDescriptor(i, (uint32)asm_unhandled_interrupt, 0);
    }

    // 初始化8259A芯片
    initialize8259A();
}

void InterruptManager::setInterruptDescriptor(uint32 index, uint32 address, byte DPL)
{
    IDT[index * 2] = (CODE_SELECTOR << 16) | (address & 0xffff);
    IDT[index * 2 + 1] = (address & 0xffff0000) | (0x1 << 15) | (DPL << 13) | (0xe << 8);
}

void InterruptManager::initialize8259A()
{
    // ICW 1
    asm_out_port(0x20, 0x11);
    asm_out_port(0xa0, 0x11);
    // ICW 2
    IRQ0_8259A_MASTER = 0x20;
    IRQ0_8259A_SLAVE = 0x28;
    asm_out_port(0x21, IRQ0_8259A_MASTER);
    asm_out_port(0xa1, IRQ0_8259A_SLAVE);
    // ICW 3
    asm_out_port(0x21, 4);
    asm_out_port(0xa1, 2);
    // ICW 4
    asm_out_port(0x21, 1);
    asm_out_port(0xa1, 1);

    // OCW 1 屏蔽主片所有中断，但主片的IRQ2需要开启
    asm_out_port(0x21, 0xfb);
    // OCW 1 屏蔽从片所有中断
    asm_out_port(0xa1, 0xff);
}

void InterruptManager::enableTimeInterrupt()
{
    uint8 value;
    // 读入主片OCW
    asm_in_port(0x21, &value);
    // 开启主片时钟中断，置0开启
    value = value & 0xfe;
    asm_out_port(0x21, value);
}

void InterruptManager::disableTimeInterrupt()
{
    uint8 value;
    asm_in_port(0x21, &value);
    // 关闭时钟中断，置1关闭
    value = value | 0x01;
    asm_out_port(0x21, value);
}

void InterruptManager::setTimeInterrupt(void *handler)
{
    setInterruptDescriptor(IRQ0_8259A_MASTER, (uint32)handler, 0);
}

// 中断处理函数(非抢占式)
// extern "C" void c_time_interrupt_handler()
// {
//     PCB *cur = programManager.running;
    
//     if(times==1){
//         int pid1=programManager.executeThread(first_thread, nullptr, "first thread", 1);
//         programManager.setThreadTicks(pid1, 50);
        
//         int pid2=programManager.executeThread(second_thread, nullptr, "second thread", 2);
//         programManager.setThreadTicks(pid2, 22);

//         int pid3=programManager.executeThread(third_thread, nullptr, "third thread", 4);
//         programManager.setThreadTicks(pid3, 15);
//     }

//     if(times==20){
//         int pid4=programManager.executeThread(forth_thread, nullptr, "forth thread", 1);
//         programManager.setThreadTicks(pid4, 10);
//     }

//     if(times==20){
//         int pid5=programManager.executeThread(fifth_thread, nullptr, "fifth thread", 6);
//         programManager.setThreadTicks(pid5, 18);
//     }

//     if(times==88){
//         int pid6=programManager.executeThread(sixth_thread, nullptr, "sixth thread", 5);
//         programManager.setThreadTicks(pid6, 23);
//     }

//     // 更新全局时间计数器
//     ++times;

//     if(cur->pid==0){
//     	programManager.schedule();
//     	return ;
//     }

//     if(cur->ticksPassedBy%10==0&&cur->ticks!=0){
//     	printf("pid %d name \"%s\": is running, conducting ticks %d\/%d \n", 
//                cur->pid, cur->name,cur->ticksPassedBy,cur->ticks+cur->ticksPassedBy);
//     } 
    
//     if (cur->ticks)
//     {
//         --cur->ticks;
//         ++cur->ticksPassedBy;
//     }
//     else if(cur->ticks==0&&cur->ticksPassedBy!=0)
//     {
//         printf("pid %d name \"%s\": is dead, conducting ticks %d\/%d \n", 
//                cur->pid, cur->name,cur->ticksPassedBy,cur->ticks+cur->ticksPassedBy);
//         cur->status = ProgramStatus::DEAD;
//         programManager.schedule();
//     }
// }

// 中断处理函数(抢占式)
extern "C" void c_time_interrupt_handler()
{
    PCB *cur = programManager.running;
    
    if(times==1){
        int pid1=programManager.executeThread(first_thread, nullptr, "first thread", 2);
        programManager.setThreadTicks(pid1, 50);
        
        int pid2=programManager.executeThread(second_thread, nullptr, "second thread", 3);
        programManager.setThreadTicks(pid2, 22);

        int pid3=programManager.executeThread(third_thread, nullptr, "third thread", 5);
        programManager.setThreadTicks(pid3, 15);
    }

    if(times==20){
        int pid4=programManager.executeThread(forth_thread, nullptr, "forth thread", 2);
        programManager.setThreadTicks(pid4, 10);
    }

    if(times==20){
        int pid5=programManager.executeThread(fifth_thread, nullptr, "fifth thread", 7);
        programManager.setThreadTicks(pid5, 18);
    }

    if(times==88){
        int pid6=programManager.executeThread(sixth_thread, nullptr, "sixth thread", 6);
        programManager.setThreadTicks(pid6, 23);
    }

    // 更新全局时间计数器
    ++times;

    if(cur->pid == 0){
        programManager.schedule();
        return;
    }

    if(cur->ticksPassedBy%10==0&&cur->ticks!=0){
        printf("pid %d name \"%s\": is running, remaining ticks %d/%d \n", 
               cur->pid, cur->name, cur->ticksPassedBy, cur->ticks + cur->ticksPassedBy);
    } 
    
    if (cur->ticks)
    {
        --cur->ticks;
        ++cur->ticksPassedBy;
        // 对于抢占式SJF，我们需要在每次时间片结束时都检查是否有更短作业到达
        programManager.schedule();
    }
    else if(cur->ticks==0&&cur->ticksPassedBy!=0)
    {
        printf("pid %d name \"%s\": is dead, conducting ticks %d\/%d \n", 
        cur->pid, cur->name,cur->ticksPassedBy,cur->ticks+cur->ticksPassedBy);
        cur->status = ProgramStatus::DEAD;
        programManager.schedule();
    }
}

void InterruptManager::enableInterrupt()
{
    asm_enable_interrupt();
}

void InterruptManager::disableInterrupt()
{
    asm_disable_interrupt();
}

bool InterruptManager::getInterruptStatus()
{
    return asm_interrupt_status() ? true : false;
}

// 设置中断状态
void InterruptManager::setInterruptStatus(bool status)
{
    if (status)
    {
        enableInterrupt();
    }
    else
    {
        disableInterrupt();
    }
}


