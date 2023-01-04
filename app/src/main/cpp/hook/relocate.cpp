//
// Created by xw0code on 2023-01-04.
//
#include "relocate.h"

static void
relocateInstructionInThumb(uint32_t target_addr, uint16_t *orig_instructions, int length,
                           uint16_t *trampoline_instructions, int *orig_boundaries,
                           int *trampoline_boundaries, int *count) {

}

static void relocateInstructionInArm(uint32_t target_addr, uint32_t *orig_instructions, int length,
                                     uint32_t *trampoline_instructions, int *orig_boundaries,
                                     int *trampoline_boundaries, int *count) {

}

void relocateInstruction(uint32_t target_addr, void *orig_instructions, int length,
                         void *trampoline_instructions, int *orig_boundaries,
                         int *trampoline_boundaries, int *count) {
    if (TEST_BIT0(target_addr)) {
        relocateInstructionInThumb(target_addr - 1, (uint16_t *) orig_instructions, length,
                                   (uint16_t *) trampoline_instructions, orig_boundaries,
                                   trampoline_boundaries, count);
    } else {
        relocateInstructionInArm(target_addr, (uint32_t *) orig_instructions, length,
                                 (uint32_t *) trampoline_instructions, orig_boundaries,
                                 trampoline_boundaries, count);
    }
}


