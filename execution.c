#include "Pipelined_regs.h"
#include <stdint.h>

void controlUnit (uint8_t opcode, uint8_t funct3, uint8_t funct7_5th) {
    
}

uint32_t immExtend (uint8_t ImmSrc) {
    uint32_t ImmExt = 0x00;   
    switch (ImmSrc) {
        case 0x00:
            if (((IF_ID_current.Instr >> 31) & 0x01)) {
                ImmExt = 0xFFFFF000 | (IF_ID_current.Instr >> 20);
            }
            else ImmExt = IF_ID_current.Instr >> 20;
            break;
        
        case  0x01:
            if (((IF_ID_current.Instr >> 31) & 0x01)) {
                ImmExt = 0xFFFFF000 | (((IF_ID_current.Instr >> 25) & 0x7F) << 5) | ((IF_ID_current.Instr >> 7) & 0x1F);
            }
            else ImmExt = (((IF_ID_current.Instr >> 25) & 0x7F) << 5) | ((IF_ID_current.Instr >> 7) & 0x1F);
            break;

        case 0x02:
            if (((IF_ID_current.Instr >> 31) & 0x01)) {
                ImmExt = 0xFFFFF000 | (((IF_ID_current.Instr >> 7) & 0x01) << 11) | (((IF_ID_current.Instr >> 25) & 0x3F) << 5) | (((IF_ID_current.Instr >> 8) & 0x0F) << 1) | 0x00;
            }
            else ImmExt = (((IF_ID_current.Instr >> 7) & 0x01) << 11) | (((IF_ID_current.Instr >> 25) & 0x3F) << 5) | (((IF_ID_current.Instr >> 8) & 0x0F) << 1) | 0x00;
            break;

        case 0x03:
            if (((IF_ID_current.Instr >> 31) & 0x01)) {
                ImmExt = 0xFFF00000 | (((IF_ID_current.Instr >> 12) & 0xFF) << 12) | (((IF_ID_current.Instr >> 20) & 0x01) << 11) | (((IF_ID_current.Instr >> 21) & 0x3FF) << 1) | 0x00;
            }
            else ImmExt = (((IF_ID_current.Instr >> 12) & 0xFF) << 12) | (((IF_ID_current.Instr >> 20) & 0x01) << 11) | (((IF_ID_current.Instr >> 21) & 0x3FF) << 1) | 0x00;
            break;

        default: break;
    }

    return ImmExt;
}

void execute_IF (uint8_t PCSrc, uint8_t StallF, uint32_t PCTarget) {
    uint32_t index = PC / 4;
    uint32_t RD;

    if (index >= MAX) RD = 0;
    else RD = InstrMem[index];

    uint32_t PCPLus4 = PC + 4;                     //modify later as a half adder from in ALU Unit.

    if (StallF) {
        PCNext = PC;
        IF_ID_next = IF_ID_current;
    }
    else {
        IF_ID_next.Instr = RD;
        IF_ID_next.PC = PC;
        IF_ID_next.PCPlus4 = PCPLus4;

        PCNext = (PCSrc) ? PCTarget : PCPLus4;
    }
}

void execute_ID () {
    uint8_t opcode = IF_ID_current.Instr & 0x7F;
    uint8_t rd = (IF_ID_current.Instr >> 7) & 0x1F;
    uint8_t funct3 = (IF_ID_current.Instr >> 12) & 0X07;
    uint8_t rs1 = (IF_ID_current.Instr >> 15) & 0x1F;
    uint8_t rs2 = (IF_ID_current.Instr >> 20) & 0x1F;
    uint8_t funct7_5th = (IF_ID_current.Instr >> 30) &0x01;    //(Instr >> 25) & 0x7F for funct7        //for subset instructions only use the 5th bit

    
}

void execute_EX () {}

void execute_MEM () {}

void execute_WB () {}