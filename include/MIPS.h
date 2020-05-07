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
uint32_t InstructionImmediate(uint32_t);
uint32_t InstructionImmediateExtended(uint32_t);

class R3501 {
public:
    R3501();
    uint32_t ReadRegister(uint32_t) const;
    void WriteRegister(uint32_t, uint32_t);
    uintptr_t RegisterAddress(uint32_t) const;
private:
    uint32_t registers[32];
};

void WriteRegisterRd(R3501*, uint32_t, uint32_t);
void WriteRegisterRt(R3501*, uint32_t, uint32_t);
uint32_t ReadRegisterRt(R3501*, uint32_t);
uint32_t ReadRegisterRs(R3501*, uint32_t);

void InterpretAddu(R3501*, uint32_t);
void InterpretSubu(R3501*, uint32_t);
void InterpretAddiu(R3501*, uint32_t);

}

#endif