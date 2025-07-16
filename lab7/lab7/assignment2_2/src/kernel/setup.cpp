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
    int addr1 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 5);
    printf("Allocated 5 page at 0x%x\n", addr1);

    int addr2 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 2);
    printf("Allocated 2 pages at 0x%x\n", addr2);
    
    int addr3 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 4);
    printf("Allocated 4 pages at 0x%x\n", addr3);
    
    int addr4 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 3);
    printf("Allocated 3 pages at 0x%x\n", addr4);
    
    
    printf("Releasing 5 page at address: 0x%x\n", addr1);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr1, 5);
            
    printf("Releasing 4 pages at address: 0x%x\n", addr3);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr3, 4);
    
    int addr5 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 3);
    printf("Allocated 3 page at 0x%x\n", addr5);
      
    int addr6 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 7);
    printf("Allocated 7 page at 0x%x\n", addr6);
    
    int addr7 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 2);
    printf("Allocated 2 page at 0x%x\n", addr7);
    
    printf("Releasing 7 page at address: 0x%x\n", addr6);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, addr6, 7);
    
    int addr8 = memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 6);
    printf("Allocated 6 page at 0x%x\n", addr8);
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

