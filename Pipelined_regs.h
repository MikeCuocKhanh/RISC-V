#ifndef pipeline_regs
#define pipeline_regs
#define MAX 1024

#include <stdint.h>

typedef struct {
    uint32_t Instr;
    uint32_t PC;
    uint32_t PCPlus4;
} IF_ID_reg;

typedef struct {
    uint8_t RegWrite;
    uint8_t ResultSrc;
    uint8_t MemWrite;
    uint8_t Jump;
    uint8_t Branch;
    uint8_t ALUControl;
    uint8_t ALUSrc;

    uint32_t RD1;
    uint32_t RD2;
    
    uint32_t PC;
    uint32_t PCPlus4;

    uint8_t Rs1;
    uint8_t Rs2;
    uint8_t Rd;

    uint32_t ImmExt;
} ID_EX_reg;

typedef struct {
    uint8_t RegWrite;
    uint8_t ResultSrc;
    uint8_t MemWrite;
    
    uint32_t ALUResult;
    uint32_t WriteData;

    uint8_t Rd;

    uint32_t PCPlus4;
} EX_MEM_reg;

typedef struct {
    uint8_t RegWrite;
    uint8_t ResultSrc;

    uint32_t ALUResult;
    uint32_t RD;

    uint8_t Rd;

    uint32_t PCPlus4;
} MEM_WB_reg;

// will add variables and functions later as development go on
extern uint32_t PC;
extern uint32_t PCNext;

extern uint32_t RegFile[32];
extern uint32_t InstrMem[MAX];
extern uint32_t DataMem[MAX];

extern IF_ID_reg IF_ID_current, IF_ID_next;
extern ID_EX_reg ID_EX_current, ID_EX_next;
extern EX_MEM_reg EX_MEM_current, EX_MEM_next;
extern MEM_WB_reg MEM_WB_current, MEM_WB_next;

typedef struct Control_Unit {
    uint8_t RegWriteD;
    uint8_t ResultSrc;
    uint8_t MemWrite;
    uint8_t Jump;
    uint8_t Branch;
    uint8_t ALUControl;
    uint8_t ALUSrc;
    uint8_t ImmSrc;
};

#endif