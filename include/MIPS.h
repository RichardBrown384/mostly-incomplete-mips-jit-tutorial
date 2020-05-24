#ifndef RBROWN_MIPS_H
#define RBROWN_MIPS_H

#pragma once

#include <cstdint>

namespace rbrown {

constexpr uint32_t ARITHMETIC_OVERFLOW = 12u;

class COP0 {
public:
    COP0();
    [[nodiscard]] uint32_t ReadRegister(uint32_t) const;
    void WriteRegister(uint32_t, uint32_t);
    void WriteRegisterMasked(uint32_t, uint32_t, uint32_t);

    uint32_t EnterException(uint32_t, uint32_t, uint32_t);
private:
    uint32_t registers[32];
};

class R3501 {
public:
    R3501();

    [[nodiscard]] uint32_t ReadRegister(uint32_t) const;
    void WriteRegister(uint32_t, uint32_t);

    [[nodiscard]] uintptr_t RegisterAddress(uint32_t) const;

    [[nodiscard]] uint32_t ReadPC() const;
    void WritePC(uint32_t);

    COP0& Cop0();

private:
    uint32_t registers[32];
    uint32_t pc;
    COP0 cop0;
};

uint32_t ReadPC(R3501*);
void WritePC(R3501*, uint32_t);

void EnterException(R3501*, uint32_t);

uint32_t InstructionFunction(uint32_t);
uint32_t InstructionRd(uint32_t);
uint32_t InstructionRt(uint32_t);
uint32_t InstructionRs(uint32_t);
uint32_t InstructionOp(uint32_t);
uint32_t InstructionImmediate(uint32_t);
uint32_t InstructionImmediateExtended(uint32_t);

void WriteRegisterRd(R3501*, uint32_t, uint32_t);
void WriteRegisterRt(R3501*, uint32_t, uint32_t);
uint32_t ReadRegisterRt(R3501*, uint32_t);
uint32_t ReadRegisterRs(R3501*, uint32_t);

void InterpretAddu(R3501*, uint32_t);
void InterpretSubu(R3501*, uint32_t);
void InterpretAddiu(R3501*, uint32_t);
void InterpretAdd(R3501*, uint32_t);

}

#endif