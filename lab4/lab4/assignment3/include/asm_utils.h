#ifndef ASM_UTILS_H
#define ASM_UTILS_H

#include "os_type.h"

extern "C" void asm_hello_world();
extern "C" void asm_lidt(uint32 start, uint16 limit);
extern "C" void asm_unhandled_interrupt();
extern "C" void asm_halt();
extern "C" void asm_out_port(uint16 port, uint8 value);
extern "C" void asm_in_port(uint16 port, uint8 *value);
extern "C" void asm_enable_interrupt();
extern "C" int asm_interrupt_status();
extern "C" void asm_disable_interrupt();
extern "C" void asm_init_page_reg(int *directory);
extern "C" void asm_segmentation_fault(); // 新增段错误处理函数声明
#endif
