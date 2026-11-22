#include <stdint.h>
#include "Pipelined_regs.h"

static uint32_t ALU (uint8_t ALUControl, uint32_t SrcA, uint32_t SrcB) {
    uint32_t ALUResult = 0x00;
    switch(ALUControl) {
        //add
        case 0x00 :
            ALUResult = SrcA + SrcB;  //Carrier flag is not neccesary since RISC-V hardware (for subset instructions) only keeps the lower 32-bit in case of overflowing
            break;

        //sub
        case 0x01:
            ALUResult = SrcA - SrcB;
            break;

        //and
        case 0x02:
            ALUResult = SrcA & SrcB;
            break;

        //or
        case 0x03:
            ALUResult = SrcA | SrcB;
            break;
        
        //slt
        case 0x05:
            ALUResult = ((int32_t) SrcA < (int32_t) SrcB) ? 1 : 0;
            break;

        default: break;
    }

    return ALUResult;
}

static void controlUnit (uint8_t opcode, uint8_t funct3, uint8_t funct7_5th) {
    controlSignals = (Control_Unit) {0};
    uint8_t ALUOp = 0x00;
    switch (opcode) {
        //lw
        case 0x03:
            controlSignals.RegWrite = 0x01;
            controlSignals.ALUSrc = 0x01;
            controlSignals.ResultSrc = 0x01;
            break;
        
        //sw
        case 0x23:
            controlSignals.ImmSrc = 0x01;
            controlSignals.ALUSrc = 0x01;
            controlSignals.MemWrite = 0x01;
            break;

        //R-type
        case 0x33:
            controlSignals.RegWrite = 0x01;
            ALUOp = 0x02;
            break;
        
        //beq
        case 0x63:
            controlSignals.ImmSrc = 0x02;
            controlSignals.Branch = 0x01;
            ALUOp = 0x01;
            break;

        //I-type ALU
        case 0x13:
            controlSignals.RegWrite = 0x01;
            controlSignals.ALUSrc = 0x01;
            ALUOp = 0x02;
            break;

        //jal    
        case 0x6F:
            controlSignals.RegWrite = 0x01;
            controlSignals.ImmSrc = 0x03;
            controlSignals.ResultSrc = 0x02;
            controlSignals.Jump = 0x01;
            break;

        default: break;
    }
    
    uint8_t opcode_5th = (opcode >> 5) & 0x01;
    uint8_t op_funct7_5th = (opcode_5th << 1) | funct7_5th;

    switch (ALUOp) {
        case 0x01:
            controlSignals.ALUControl = 0x01;
            break;

        case 0x02:
            switch (funct3) {
                case 0x00:
                    if (op_funct7_5th == 0x03) {
                        controlSignals.ALUControl = 0x01;
                    }
                    break;
                
                case 0x02:
                    controlSignals.ALUControl = 0x05;
                    break;

                case 0x06:
                    controlSignals.ALUControl = 0x03;
                    break;
                
                case 0x07:
                    controlSignals.ALUControl = 0x02;
                    break;
                
                default: break;
            }
            break;
        
        default: break;
    }
    return;
}

static uint32_t immExtend (uint8_t ImmSrc) {
    uint32_t ImmExt = 0x00;   
    switch (ImmSrc) {
        case 0x00:
            if (((IF_ID_current.Instr >> 31) & 0x01)) {
                ImmExt = 0xFFFFF000 | (IF_ID_current.Instr >> 20);
            }
            else ImmExt = IF_ID_current.Instr >> 20;
            break;
        
        case 0x01:
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

void execute_IF () {
    uint32_t index = PC / 4;
    uint32_t RD;

    if (index >= MAX) RD = 0;
    else RD = InstrMem[index];

    uint32_t PCPlus4 = ALU(0x00, PC, 0x04);                     

    /*if (StallF) {
        PCNext = PC;
        IF_ID_next = IF_ID_current;                  later in Hazard Unit, leave this here to remind myself
    }
    else {*/
        IF_ID_next.Instr = RD;
        IF_ID_next.PC = PC;
        IF_ID_next.PCPlus4 = PCPlus4;

        PCNext = (PCSrc) ? PCTarget : PCPlus4;
    //}

    return;
}

void execute_ID (uint8_t Rd, uint8_t RegWrite, uint32_t Result) {
    uint8_t opcode = IF_ID_current.Instr & 0x7F;
    uint8_t rd = (IF_ID_current.Instr >> 7) & 0x1F;
    uint8_t funct3 = (IF_ID_current.Instr >> 12) & 0X07;
    uint8_t rs1 = (IF_ID_current.Instr >> 15) & 0x1F;
    uint8_t rs2 = (IF_ID_current.Instr >> 20) & 0x1F;
    uint8_t funct7_5th = (IF_ID_current.Instr >> 30) &0x01;    //(Instr >> 25) & 0x7F for funct7        //for subset instructions only use the 5th bit

    controlUnit (opcode, funct3, funct7_5th);

    if (RegWrite && Rd) /*Cause Rd cannot be 0x00 (x0)*/{
        RegFile[Rd] = Result;
    }
    
    ID_EX_next.RegWrite = controlSignals.RegWrite;
    ID_EX_next.ResultSrc = controlSignals.ResultSrc;
    ID_EX_next.MemWrite = controlSignals.MemWrite;
    ID_EX_next.Jump = controlSignals.Jump;
    ID_EX_next.Branch = controlSignals.Branch;
    ID_EX_next.ALUControl = controlSignals.ALUControl;
    ID_EX_next.ALUSrc = controlSignals.ALUSrc;

    ID_EX_next.RD1 = RegFile[rs1];
    ID_EX_next.RD2 = RegFile[rs2];

    ID_EX_next.PC = IF_ID_current.PC;
    ID_EX_next.PCPlus4 = IF_ID_current.PCPlus4;

    ID_EX_next.Rs1 = rs1;
    ID_EX_next.Rs2 = rs2;
    ID_EX_next.Rd = rd;

    ID_EX_next.ImmExt = immExtend (controlSignals.ImmSrc);

    return;
}

void execute_EX (uint8_t forwardAE, uint8_t forwardBE, uint32_t Result) {
    Zero = 0x00;
    EX_MEM_next.RegWrite = ID_EX_current.RegWrite;
    EX_MEM_next.ResultSrc = ID_EX_current.ResultSrc;
    EX_MEM_next.MemWrite = ID_EX_current.MemWrite;

    uint32_t SrcA = 0x00;
    uint32_t SrcB = 0x00;
    uint32_t WriteData = 0x00;  //check later! 0x00 is x0 (non-write). Just to remind myself.

    switch (forwardAE) {
        case 0x00:
            SrcA = ID_EX_current.RD1;
            break;
        case 0x01:
            SrcA = Result;
            break;
        case 0x02:
            SrcA = EX_MEM_current.ALUResult;
            break;
        default: break;
    }
    switch (forwardBE) {
        case 0x00:
            WriteData = ID_EX_current.RD2;
            break;
        case 0x01:
            WriteData = Result;
            break;
        case 0x02:
            WriteData = EX_MEM_current.ALUResult;
            break;
        default: break;
    }

    switch (ID_EX_current.ALUSrc) {
        case 0x00:
            SrcB = WriteData;
            break;
        case 0x01:
            SrcB = ID_EX_current.ImmExt; 
            break;
        default: break;
    }

    PCTarget = ALU (0x00, ID_EX_current.PC, ID_EX_current.ImmExt);    

    EX_MEM_next.ALUResult = ALU (ID_EX_current.ALUControl, SrcA, SrcB);
    if(!EX_MEM_next.ALUResult) Zero = 0x01;

    PCSrc = ID_EX_current.Jump | (ID_EX_current.Branch & Zero);

    EX_MEM_next.WriteData = WriteData;
    EX_MEM_next.Rd = ID_EX_current.Rd;
    EX_MEM_next.PCPlus4 = ID_EX_current.PCPlus4;
    return;
}

void execute_MEM () {
    MEM_WB_next.RegWrite = EX_MEM_current.RegWrite;
    MEM_WB_next.ResultSrc = EX_MEM_current.ResultSrc;

    MEM_WB_next.ALUResult = EX_MEM_current.ALUResult;
    
    uint32_t index = EX_MEM_current.ALUResult / 4;
    if (EX_MEM_current.MemWrite) {
        if (index < MAX) DataMem[index] = EX_MEM_current.WriteData;
    }

    if (index < MAX) MEM_WB_next.RD = DataMem[index];
    else MEM_WB_next.RD = 0x00;
    
    MEM_WB_next.Rd = EX_MEM_current.Rd;
    MEM_WB_next.PCPlus4 = EX_MEM_current.PCPlus4;
}

void execute_WB () {
    switch (MEM_WB_current.ResultSrc) {
        case 0x00:
            Result = MEM_WB_current.ALUResult;
            break;
        case 0x01:
            Result = MEM_WB_current.RD;
            break;
        case 0x02:
            Result = MEM_WB_current.PCPlus4;
            break;
        default: break;
    }
}