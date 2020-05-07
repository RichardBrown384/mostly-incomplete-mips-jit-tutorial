#include "MIPS.h"

namespace rbrown {

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

R3501::R3501() : registers{ 0 } {}

uint32_t R3501::ReadRegister(uint32_t r) const { return registers[r]; }

void R3501::WriteRegister(uint32_t r , uint32_t v) { registers[r] = v; }

uintptr_t R3501::RegisterAddress(uint32_t r) const {
    return reinterpret_cast<uintptr_t>(&registers[r]);
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

}