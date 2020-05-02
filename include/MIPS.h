#ifndef RBROWN_MIPS_H
#define RBROWN_MIPS_H

#pragma once

#include <cstdint>

namespace rbrown {

uint32_t InstructionFunction(uint32_t);
uint32_t InstructionRd(uint32_t);
uint32_t InstructionRt(uint32_t);
uint32_t InstructionRs(uint32_t);
uint32_t InstructionOp(uint32_t);

class R3501 {
public:
    R3501();
    uint32_t ReadRegister(uint32_t) const;
    void WriteRegister(uint32_t, uint32_t);
    uintptr_t RegisterAddress(uint32_t) const;
private:
    uint32_t registers[32];
};

void WriteRegisterRd(R3501* r3501, uint32_t opcode, uint32_t value);
uint32_t ReadRegisterRt(R3501* r3501, uint32_t opcode);
uint32_t ReadRegisterRs(R3501* r3501, uint32_t opcode);

void InterpretAddu(R3501* r3501, uint32_t opcode);
void InterpretSubu(R3501* r3501, uint32_t opcode);

}

#endif