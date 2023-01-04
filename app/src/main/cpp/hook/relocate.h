#ifndef _RELOCATE_H
#define _RELOCATE_H

#include <stdio.h>

#define CLEAR_BIT0(addr)    (addr & 0xFFFFFFFE)//最后一位置0
#define TEST_BIT0(addr)        (addr & 1)//测试地址最后一位是不是0
#define PAGE_START(addr)	(~(PAGE_SIZE - 1) & (addr))


void relocateInstruction(uint32_t target_addr, void *orig_instructions, int length,
                         void *trampoline_instructions, int *orig_boundaries,
                         int *trampoline_boundaries, int *count);

#endif