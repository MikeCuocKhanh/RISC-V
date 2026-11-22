#include <stdint.h>
#include "Pipelined_regs.h"

uint8_t stallFetch () {
}

uint8_t stallDecode () {}

uint8_t flushDecode () {}

uint8_t flushExecute () {}

uint8_t forwardExecute (uint8_t Rs) {
    uint8_t Forward = 0x00;
    if (((Rs == EX_MEM_current.Rd) & EX_MEM_current.RegWrite) & (Rs != 0)) Forward = 0x02;

    else if (((Rs == MEM_WB_current.Rd) & MEM_WB_current.RegWrite) & (Rs != 0)) Forward = 0x01;

    return Forward;
}

void hazardUnit () {

}