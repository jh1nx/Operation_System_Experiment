#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"

STDIO stdio;
InterruptManager interruptManager;
ProgramManager programManager;
MemoryManager memoryManager;

void memory_test_thread(void *arg) {
    // 测试1: 分配单页内存
    int addr1 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 1);
    printf("Allocated 1 page at 0x%x\n", addr1);

    int addr2 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 3);
    printf("Allocated 3 pages at 0x%x\n", addr2);
    
    int addr3 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 5);
    printf("Allocated 5 pages at 0x%x\n", addr3);
    
    int addr4 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 2);
    printf("Allocated 2 pages at 0x%x\n", addr4);
    
    
    printf("Releasing 3 page at address: 0x%x\n", addr2);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr2, 3);
            
    printf("Releasing 5 pages at address: 0x%x\n", addr3);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr3, 5);
    
    printf("Releasing 1 page at address: 0x%x\n", addr1);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr1, 1);
            
    printf("Releasing 2 pages at address: 0x%x\n", addr4);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr4, 2);   
      
    printf("\nMemory released successfully\n");
}

void first_thread(void *arg) {
    // 创建内存测试线程
    int pid = programManager.executeThread(memory_test_thread, nullptr, "mem_test", 1);
    if (pid != -1) {
        printf("Created memory test thread, PID: %d\n", pid);
    } else {
        printf("Failed to create memory test thread\n");
    }
    
    // 主线程不可以返回
    asm_halt();
}

extern "C" void setup_kernel() {
    // 初始化基本组件
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);
    
    stdio.initialize();
    programManager.initialize();
    
    // 初始化内存管理器并开启分页机制
    memoryManager.openPageMechanism();
    memoryManager.initialize();
    
    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
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

