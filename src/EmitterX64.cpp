#include "EmitterX64.h"
#include "CodeBuffer.h"

namespace rbrown {

namespace {

uint8_t Rex(uint32_t w, uint32_t r, uint32_t x, uint32_t b) {
    return 0x40u + ((w & 1u) << 3u) + ((r & 1u) << 2u) + ((x & 1u) << 1u) + (b & 1u);
}

uint8_t ModRM(uint32_t mod, uint32_t reg, uint32_t rm) {
    return ((mod & 0x3u) << 6u) + ((reg & 0x7u) << 3u) + (rm & 0x7u);
}

}

EmitterX64::EmitterX64(CodeBuffer& buf) : buffer(buf) {}

void EmitterX64::AddR32R32(uint32_t rm, uint32_t reg) {
    const uint8_t rex = Rex(0, reg >> 3u, 0, rm >> 3u);
    const uint8_t mod = ModRM(0x3u, reg, rm);
    buffer.Bytes({ rex, 0x01u, mod });
}

void EmitterX64::AddR64Imm8(uint32_t rm, uint8_t imm8) {
    const uint8_t rex = Rex(1, 0, 0, rm >> 3u);
    const uint8_t mod = ModRM(0x3u, 0u, rm);
    buffer.Bytes({ rex, 0x83u, mod, imm8 });
}

void EmitterX64::SubR64Imm8(uint32_t rm, uint8_t imm8) {
    const uint8_t rex = Rex(1, 0, 0, rm >> 3u);
    const uint8_t mod = ModRM(0x3u, 5u, rm);
    buffer.Bytes({ rex, 0x83u, mod, imm8 });
}

void EmitterX64::MovR32R32(uint32_t rm, uint32_t reg) {
    const uint8_t rex = Rex(0, reg >> 3u, 0, rm >> 3u);
    const uint8_t mod = ModRM(0x3u, reg, rm);
    buffer.Bytes({ rex, 0x89u, mod });
}

void EmitterX64::MovR32Imm32(uint32_t rw, uint32_t imm32) {
    const uint8_t rex = Rex(0, 0, 0, rw >> 3u);
    const uint8_t code = 0xB8u + (rw & 0x7u);
    buffer.Bytes({ rex, code });
    buffer.DWord(imm32);
}

void EmitterX64::MovR64Imm64(uint32_t rw, uint64_t imm64) {
    const uint8_t rex = Rex(1, 0, 0, rw >> 3u);
    const uint8_t code = 0xB8u + (rw & 0x7u);
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

void EmitterX64::PushR64(uint32_t rd) {
    const uint8_t rex = Rex(0, 0, 0, rd >> 3u);
    const uint8_t code = 0x50u + (rd & 7u);
    buffer.Bytes({ rex, code });
}

void EmitterX64::PopR64(uint32_t rd) {
    const uint8_t rex = Rex(0, 0, 0, rd >> 3u);
    const uint8_t code = 0x58u + (rd & 7u);
    buffer.Bytes({ rex, code });
}

void EmitterX64::CallRel32(uint32_t rel32) {
    buffer.Byte(0xE8u);
    buffer.DWord(rel32);
}

void EmitterX64::Call(uintptr_t target) {
    CallRel32(target - (buffer.Position() + 5u));
}

void EmitterX64::Ret() {
    buffer.Byte(0xC3u);
}

}