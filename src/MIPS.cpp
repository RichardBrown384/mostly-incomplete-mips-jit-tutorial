#include "MIPS.h"

namespace rbrown {

namespace {

uint32_t OverflowAdd(uint32_t x, uint32_t y, uint32_t result) {
    return (~(x ^ y) & (x ^ result)) >> 31u;
}

}

constexpr uint32_t SR = 12;
constexpr uint32_t CAUSE = 13;
constexpr uint32_t EPC = 14;

constexpr uint32_t BOOT_EXCEPTION_VECTOR = 0xBFC00180u;
constexpr uint32_t EXCEPTION_VECTOR = 0x80000080u;

COP0::COP0() : registers{ 0 } {}

uint32_t COP0::ReadRegister(uint32_t r) const { return registers[r]; }

void COP0::WriteRegister(uint32_t r , uint32_t v) { registers[r] = v; }

void COP0::WriteRegisterMasked(uint32_t r, uint32_t mask, uint32_t v) {
    registers[r] = (registers[r] & ~mask) + (v & mask);
}

uint32_t COP0::EnterException(uint32_t code, uint32_t epc, uint32_t branch) {
    WriteRegisterMasked(SR, 0x0000003Fu, ReadRegister(SR) << 2u);
    WriteRegisterMasked(CAUSE, 0x8000007Cu, (branch << 31u) + ((code & 0x1Fu) << 2u));
    WriteRegister(EPC, epc);
    return BOOT_EXCEPTION_VECTOR;
}

R3501::R3501() : registers{ 0 }, pc { 0xBFC00000u }, cop0 { } {}

uint32_t R3501::ReadRegister(uint32_t r) const { return registers[r]; }

void R3501::WriteRegister(uint32_t r , uint32_t v) { registers[r] = v; }

uintptr_t R3501::RegisterAddress(uint32_t r) const {
    return reinterpret_cast<uintptr_t>(&registers[r]);
}

uint32_t R3501::ReadPC() const { return pc; }

void R3501::WritePC(uint32_t v) { pc = v; }

COP0& R3501::Cop0() { return cop0; }

uint32_t ReadPC(R3501* r3501) {
    return r3501->ReadPC();
}

void WritePC(R3501* r3501, uint32_t pc) {
    r3501->WritePC(pc);
}

void EnterException(R3501* r3501, uint32_t code) {
    const uint32_t epc = ReadPC(r3501);
    const uint32_t pc = r3501->Cop0().EnterException(code, epc, 0u);
    WritePC(r3501, pc);
}

uint32_t InstructionFunction(uint32_t opcode) {
    return opcode & 0x3Fu;
}

uint32_t InstructionRd(uint32_t opcode) {
    return (opcode >> 11u) & 0x1Fu;
}

uint32_t InstructionRt(uint32_t opcode) {
    return (opcode >> 16u) & 0x1Fu;
}

uint32_t InstructionRs(uint32_t opcode) {
    return (opcode >> 21u) & 0x1Fu;
}

uint32_t InstructionOp(uint32_t opcode) {
    return (opcode >> 26u) & 0x3Fu;
}

uint32_t InstructionImmediate(uint32_t opcode) {
    return opcode & 0xFFFFu;
}

uint32_t InstructionImmediateExtended(uint32_t opcode) {
    return ((opcode & 0xFFFFu) ^ 0x8000u) - 0x8000u;
}

void WriteRegisterRd(R3501* r3501, uint32_t opcode, uint32_t value) {
    r3501->WriteRegister(InstructionRd(opcode), value);
}

void WriteRegisterRt(R3501* r3501, uint32_t opcode, uint32_t value) {
    r3501->WriteRegister(InstructionRt(opcode), value);
}

uint32_t ReadRegisterRt(R3501* r3501, uint32_t opcode) {
    return r3501->ReadRegister(InstructionRt(opcode));
}

uint32_t ReadRegisterRs(R3501* r3501, uint32_t opcode) {
    return r3501->ReadRegister(InstructionRs(opcode));
}

void InterpretAddu(R3501* r3501, uint32_t opcode) {
    const uint32_t s = ReadRegisterRs(r3501, opcode);
    const uint32_t t = ReadRegisterRt(r3501, opcode);
    WriteRegisterRd(r3501, opcode, s + t);
}

void InterpretSubu(R3501* r3501, uint32_t opcode) {
    const uint32_t s = ReadRegisterRs(r3501, opcode);
    const uint32_t t = ReadRegisterRt(r3501, opcode);
    WriteRegisterRd(r3501, opcode, s - t);
}

void InterpretAddiu(R3501* r3501, uint32_t opcode) {
    const uint32_t s = ReadRegisterRs(r3501, opcode);
    const uint32_t immediate = InstructionImmediateExtended(opcode);
    WriteRegisterRt(r3501, opcode, s + immediate);
}

void InterpretAdd(R3501* r3501, uint32_t opcode) {
    const uint32_t s = ReadRegisterRs(r3501, opcode);
    const uint32_t t = ReadRegisterRt(r3501, opcode);
    const uint32_t result = s + t;
    if (OverflowAdd(s, t, result)) {
        EnterException(r3501, ARITHMETIC_OVERFLOW);
        return;
    }
    WriteRegisterRd(r3501, opcode, result);
}


}