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

constexpr uint32_t RESET_EXCEPTION_VECTOR = 0xBFC00000u;
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

R3051::R3051() :
    registers { 0 },
    pc { RESET_EXCEPTION_VECTOR },
    cop0 { },
    loadDelaySlot { false },
    loadDelaySlotNext { false },
    loadDelayRegister { 0 },
    loadDelayValue { 0 } {}

uintptr_t R3051::RegisterAddress(uint32_t r) const {
    return reinterpret_cast<uintptr_t>(&registers[r]);
}

uint32_t R3051::ReadRegister(uint32_t r) const { return registers[r]; }
uint32_t R3051::ReadPC() const { return pc; }
bool R3051::GetLoadDelaySlot() const { return loadDelaySlot; }
bool R3051::GetLoadDelaySlotNext() const { return loadDelaySlotNext; }
uint32_t R3051::GetLoadDelayRegister() const { return loadDelayRegister; }
uint32_t R3051::GetLoadDelayValue() const { return loadDelayValue; }

void R3051::WriteRegister(uint32_t r, uint32_t v) { registers[r] = v; }
void R3051::WritePC(uint32_t v) { pc = v; }
void R3051::SetLoadDelaySlot(bool v) { loadDelaySlot = v; }
void R3051::SetLoadDelaySlotNext(bool v) { loadDelaySlotNext = v; }
void R3051::SetLoadDelayRegister(uint32_t v) { loadDelayRegister = v; }
void R3051::SetLoadDelayValue(uint32_t v) { loadDelayValue = v; }

COP0& R3051::Cop0() { return cop0; }

uint32_t ReadRegister(R3051* r3051, uint32_t r) { return r3051->ReadRegister(r); }
uint32_t ReadPC(R3051* r3051) { return r3051->ReadPC(); }
bool GetLoadDelaySlot(R3051* r3051) { return r3051->GetLoadDelaySlot(); }
bool GetLoadDelaySlotNext(R3051* r3051) { return r3051->GetLoadDelaySlotNext(); }
uint32_t GetLoadDelayRegister(R3051* r3051) { return r3051->GetLoadDelayRegister(); }
uint32_t GetLoadDelayValue(R3051* r3051) { return r3051->GetLoadDelayValue(); }

void WriteRegister(R3051* r3051, uint32_t r, uint32_t value) { r3051->WriteRegister(r, value); }
void WritePC(R3051* r3051, uint32_t pc) { r3051->WritePC(pc); }
void SetLoadDelaySlot(R3051* r3051, bool v) { r3051->SetLoadDelaySlot(v); }
void SetLoadDelaySlotNext(R3051* r3051, bool v) { r3051->SetLoadDelaySlotNext(v); }
void SetLoadDelayRegister(R3051* r3051, uint32_t v) { r3051->SetLoadDelayRegister(v); }
void SetLoadDelayValue(R3051* r3051, uint32_t v) { r3051->SetLoadDelayValue(v); }

void EnterException(R3051* r3051, uint32_t code) {
    const uint32_t epc = ReadPC(r3051);
    const uint32_t pc = r3051->Cop0().EnterException(code, epc, 0u);
    WritePC(r3051, pc);
}

uint32_t InstructionFunction(uint32_t opcode) { return opcode & 0x3Fu; }
uint32_t InstructionRd(uint32_t opcode) { return (opcode >> 11u) & 0x1Fu; }
uint32_t InstructionRt(uint32_t opcode) { return (opcode >> 16u) & 0x1Fu; }
uint32_t InstructionRs(uint32_t opcode) { return (opcode >> 21u) & 0x1Fu; }
uint32_t InstructionOp(uint32_t opcode) { return (opcode >> 26u) & 0x3Fu; }
uint32_t InstructionImmediate(uint32_t opcode) { return opcode & 0xFFFFu; }
uint32_t InstructionImmediateExtended(uint32_t opcode) {
    return ((opcode & 0xFFFFu) ^ 0x8000u) - 0x8000u;
}

void WriteRegisterRd(R3051* r3051, uint32_t opcode, uint32_t value) {
    WriteRegister(r3051, InstructionRd(opcode), value);
}

void WriteRegisterRt(R3051* r3051, uint32_t opcode, uint32_t value) {
    WriteRegister(r3051, InstructionRt(opcode), value);
}

void WriteRegisterDelayed(R3051* r3051, uint32_t r, uint32_t value) {
    if (GetLoadDelaySlot(r3051)) {
        const uint32_t loadDelayedRegister = GetLoadDelayRegister(r3051);
        const uint32_t loadDelayedValue = GetLoadDelayValue(r3051);
        if (loadDelayedRegister != r) {
            WriteRegister(r3051, loadDelayedRegister, loadDelayedValue);
        }
        SetLoadDelaySlot(r3051, false);
    }
    SetLoadDelaySlotNext(r3051, true);
    SetLoadDelayRegister(r3051, r);
    SetLoadDelayValue(r3051, value);
}

uint32_t ReadRegisterRt(R3051* r3051, uint32_t opcode) {
    return ReadRegister(r3051, InstructionRt(opcode));
}

uint32_t ReadRegisterRs(R3051* r3051, uint32_t opcode) {
    return ReadRegister(r3051, InstructionRs(opcode));
}

bool StoreWord(R3051* r3051, uint32_t virtualAddress, uint32_t value) {
    return true;
}

bool LoadWord(R3051* r3051, uint32_t virtualAddress, uint32_t* value) {
    return true;
}

void InterpretAddu(R3051* r3051, uint32_t opcode) {
    const uint32_t s = ReadRegisterRs(r3051, opcode);
    const uint32_t t = ReadRegisterRt(r3051, opcode);
    WriteRegisterRd(r3051, opcode, s + t);
}

void InterpretSubu(R3051* r3051, uint32_t opcode) {
    const uint32_t s = ReadRegisterRs(r3051, opcode);
    const uint32_t t = ReadRegisterRt(r3051, opcode);
    WriteRegisterRd(r3051, opcode, s - t);
}

void InterpretAddiu(R3051* r3051, uint32_t opcode) {
    const uint32_t s = ReadRegisterRs(r3051, opcode);
    const uint32_t immediate = InstructionImmediateExtended(opcode);
    WriteRegisterRt(r3051, opcode, s + immediate);
}

void InterpretAdd(R3051* r3051, uint32_t opcode) {
    const uint32_t s = ReadRegisterRs(r3051, opcode);
    const uint32_t t = ReadRegisterRt(r3051, opcode);
    const uint32_t result = s + t;
    if (OverflowAdd(s, t, result)) {
        EnterException(r3051, ARITHMETIC_OVERFLOW);
        return;
    }
    WriteRegisterRd(r3051, opcode, result);
}

void InterpretSw(R3051* r3051, uint32_t opcode) {
    const uint32_t base = ReadRegisterRs(r3051, opcode);
    const uint32_t t = ReadRegisterRt(r3051, opcode);
    const uint32_t offset = InstructionImmediateExtended(opcode);
    StoreWord(r3051, base + offset, t);
}

void InterpretLw(R3051* r3051, uint32_t opcode) {
    const uint32_t base = ReadRegisterRs(r3051, opcode);
    const uint32_t t = ReadRegisterRt(r3051, opcode);
    const uint32_t offset = InstructionImmediateExtended(opcode);
    uint32_t value;
    if (LoadWord(r3051, base + offset, &value)) {
        WriteRegisterDelayed(r3051, t, value);
    }
}
}