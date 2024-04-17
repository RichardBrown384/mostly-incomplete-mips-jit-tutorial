#include "EmitterX64.h"
#include "CodeBuffer.h"

namespace rbrown {

namespace {

uint8_t Rex(uint32_t w, uint32_t r, uint32_t x, uint32_t b) {
    return static_cast<uint8_t>(0x40u + ((w & 1u) << 3u) + ((r & 1u) << 2u) + ((x & 1u) << 1u) + (b & 1u));
}

uint8_t ModRM(uint32_t mod, uint32_t reg, uint32_t rm) {
    return static_cast<uint8_t>(((mod & 3u) << 6u) + ((reg & 7u) << 3u) + (rm & 7u));
}

}

EmitterX64::EmitterX64(CodeBuffer& buf) : buffer{ buf }, callSites{ }, nextLabelId {0}  {}

Label EmitterX64::NewLabel() {
    return static_cast<Label>(nextLabelId++);
}

void EmitterX64::Bind(Label& label) {
    if (label.Bound()) {
        return;
    }
    label.Bind(buffer.Position());
    const uint64_t id = label.Id();
    for (const CallSite& site: callSites[id]) {
        FixUpCallSite(site, label);
    }
    callSites.erase(id);
}

void EmitterX64::FixUpCallSite(const CallSite& site, const Label& label) {
    buffer.Byte(site.Position() - 1u, static_cast<uint8_t>(label.Position() - site.Position()));
}

void EmitterX64::Jno(const Label& label) {
    buffer.Bytes({ 0x71u, 0x00u });
    const size_t position = buffer.Position();
    if (label.Bound()) {
        FixUpCallSite(static_cast<CallSite>(position), label);
    } else {
        callSites[label.Id()].emplace_back(position);
    }
}

void EmitterX64::Jne(const Label& label) {
    buffer.Bytes({ 0x75u, 0x00u });
    const size_t position = buffer.Position();
    if (label.Bound()) {
        FixUpCallSite(static_cast<CallSite>(position), label);
    } else {
        callSites[label.Id()].emplace_back(position);
    }
}

void EmitterX64::Js(const Label& label) {
    buffer.Bytes({ 0x78u, 0x00u });
    const size_t position = buffer.Position();
    if (label.Bound()) {
        FixUpCallSite(static_cast<CallSite>(position), label);
    } else {
        callSites[label.Id()].emplace_back(position);
    }
}

void EmitterX64::Jmp(const Label& label) {
    buffer.Bytes({ 0xEBu, 0x00u });
    const size_t position = buffer.Position();
    if (label.Bound()) {
        FixUpCallSite(static_cast<CallSite>(position), label);
    } else {
        callSites[label.Id()].emplace_back(position);
    }
}

void EmitterX64::TestALImm8(uint8_t imm8) {
    buffer.Bytes({ 0xA8u, imm8 });
}

void EmitterX64::CmpR32Imm8(uint32_t rm, uint8_t imm8) {
    const uint8_t rex = Rex(0u, 0u, 0u, rm >> 3u);
    const uint8_t mod = ModRM(3u, 7u, rm);
    buffer.Bytes({rex, 0x83u, mod, imm8});
}

void EmitterX64::AddR32R32(uint32_t rm, uint32_t reg) {
    const uint8_t rex = Rex(0u, reg >> 3u, 0u, rm >> 3u);
    const uint8_t mod = ModRM(3u, reg, rm);
    buffer.Bytes({ rex, 0x01u, mod });
}

void EmitterX64::AddR32Imm32(uint32_t rm, uint32_t imm32) {
    const uint8_t rex = Rex(0u, 0u, 0u, rm >> 3u);
    const uint8_t mod = ModRM(3u, 0u, rm);
    buffer.Bytes({ rex, 0x81u, mod });
    buffer.DWord(imm32);
}

void EmitterX64::AddR64Imm8(uint32_t rm, uint8_t imm8) {
    const uint8_t rex = Rex(1u, 0u, 0u, rm >> 3u);
    const uint8_t mod = ModRM(3u, 0u, rm);
    buffer.Bytes({ rex, 0x83u, mod, imm8 });
}

void EmitterX64::SubR32R32(uint32_t rm, uint32_t reg) {
    const uint8_t rex = Rex(0u, reg >> 3u, 0u, rm >> 3u);
    const uint8_t mod = ModRM(3u, reg, rm);
    buffer.Bytes({ rex, 0x29u, mod });
}

void EmitterX64::SubR64Imm8(uint32_t rm, uint8_t imm8) {
    const uint8_t rex = Rex(1u, 0u, 0u, rm >> 3u);
    const uint8_t mod = ModRM(3u, 5u, rm);
    buffer.Bytes({ rex, 0x83u, mod, imm8 });
}

void EmitterX64::MovR32R32(uint32_t rm, uint32_t reg) {
    const uint8_t rex = Rex(0u, reg >> 3u, 0u, rm >> 3u);
    const uint8_t mod = ModRM(3u, reg, rm);
    buffer.Bytes({ rex, 0x89u, mod });
}

void EmitterX64::MovR32Disp8(uint32_t reg, uint32_t rm, uint8_t disp8) {
    const uint8_t rex = Rex(0u, reg >> 3u, 0u, rm >> 3u);
    const uint8_t mod = ModRM(1u, reg, rm);
    buffer.Bytes({ rex, 0x8Bu, mod, disp8 });
}

void EmitterX64::MovDisp8R32(uint32_t rm, uint8_t disp8, uint32_t reg) {
    const uint8_t rex = Rex(0u, reg >> 3u, 0u, rm >> 3u);
    const uint8_t mod = ModRM(1u, reg, rm);
    buffer.Bytes({ rex, 0x89u, mod, disp8 });
}

void EmitterX64::MovR32Imm32(uint32_t rw, uint32_t imm32) {
    const uint8_t rex = Rex(0u, 0u, 0u, rw >> 3u);
    const uint8_t code = static_cast<const uint8_t>(0xB8u + (rw & 7u));
    buffer.Bytes({ rex, code });
    buffer.DWord(imm32);
}

void EmitterX64::MovR64R64(uint32_t rm, uint32_t reg) {
    const uint8_t rex = Rex(1u, reg >> 3u, 0u, rm >> 3u);
    const uint8_t mod = ModRM(3u, reg, rm);
    buffer.Bytes({ rex, 0x89u, mod });
}

void EmitterX64::MovR64Imm64(uint32_t rw, uint64_t imm64) {
    const uint8_t rex = Rex(1u, 0u, 0u, rw >> 3u);
    const uint8_t code = static_cast<const uint8_t>(0xB8u + (rw & 7u));
    buffer.Bytes({ rex, code });
    buffer.QWord(imm64);
}

void EmitterX64::MovEAXAbs(uintptr_t address) {
    buffer.Byte(0xA1u);
    buffer.QWord(address);
}

void EmitterX64::MovAbsEAX(uintptr_t address) {
    buffer.Byte(0xA3u);
    buffer.QWord(address);
}

void EmitterX64::LeaR64Disp8(uint32_t reg, uint32_t rm, uint8_t disp8) {
    const uint8_t rex = Rex(1u, reg >> 3u, 0u, rm >> 3u);
    const uint8_t mod = ModRM(1u, reg, rm);
    buffer.Bytes({rex, 0x8D, mod, disp8});
}

void EmitterX64::PushR64(uint32_t rd) {
    const uint8_t rex = Rex(0u, 0u, 0u, rd >> 3u);
    const uint8_t code = static_cast<const uint8_t>(0x50u + (rd & 7u));
    buffer.Bytes({ rex, code });
}

void EmitterX64::PopR64(uint32_t rd) {
    const uint8_t rex = Rex(0u, 0u, 0u, rd >> 3u);
    const uint8_t code = static_cast<const uint8_t>(0x58u + (rd & 7u));
    buffer.Bytes({ rex, code });
}

void EmitterX64::CallRel32(uint32_t rel32) {
    buffer.Byte(0xE8u);
    buffer.DWord(rel32);
}

void EmitterX64::Call(uintptr_t target) {
    CallRel32(static_cast<uint32_t>(target - (buffer.BufferAddress() + buffer.Position() + 5u)));
}

void EmitterX64::Ret() {
    buffer.Byte(0xC3u);
}

}