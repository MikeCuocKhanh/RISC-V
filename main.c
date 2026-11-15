#include <stdint.h>
#include <stdio.h>
#include "Pipelined_regs.h"

//connect files here gcc

uint32_t PC;
uint32_t PCNext;
uint32_t PCTarget;

uint32_t RegFile[32];
uint32_t InstrMem[MAX];
uint32_t DataMem[MAX];

IF_ID_reg IF_ID_current, IF_ID_next;
ID_EX_reg ID_EX_current, ID_EX_next;
EX_MEM_reg EX_MEM_current, EX_MEM_next;
MEM_WB_reg MEM_WB_current, MEM_WB_next;

Control_Unit controlSignals;
uint8_t Zero;
uint8_t PCSrc;

int cycle;

void initCPU () {
    cycle = 0;

    PC = 0;
    PCNext = 0;

    for (int i = 0; i < 32; i++) RegFile[i] = 0;

    for (int i = 0; i < MAX; i++) {
        InstrMem[i] = 0;
        DataMem[i] = 0;
    }

    IF_ID_current = (IF_ID_reg) {0};
    IF_ID_next = (IF_ID_reg) {0};

    ID_EX_current = (ID_EX_reg) {0};
    ID_EX_next = (ID_EX_reg) {0};

    EX_MEM_current = (EX_MEM_reg) {0};
    EX_MEM_next = (EX_MEM_reg) {0};
    
    MEM_WB_current = (MEM_WB_reg) {0};
    MEM_WB_next = (MEM_WB_reg) {0};

    controlSignals = (Control_Unit) {0};
    Zero = 0x00;
    PCSrc = 0x00;
}

void updateState () {
    PC = PCNext;
    IF_ID_current = IF_ID_next;
    ID_EX_current = ID_EX_next;
    EX_MEM_current = EX_MEM_next;
    MEM_WB_current = MEM_WB_next;
}

int main() {
    initCPU();
    
    while (1) {
        cycle++;
        
        //calling executions
        //calling hazard control
        
        updateState();
    }

    return 0;
}