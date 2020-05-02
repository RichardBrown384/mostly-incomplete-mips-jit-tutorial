#include "CodeBuffer.h"
#include "Mmap.h"

namespace rbrown {

CodeBuffer::CodeBuffer(size_t len) : buffer(rbrown::xmmap::Map(len)), length(len), pos(0) {}

CodeBuffer::~CodeBuffer() {
    rbrown::xmmap::Unmap(buffer, length);
    buffer = nullptr;
    length = 0;
    pos = 0;
}

void CodeBuffer::Protect() {
    rbrown::xmmap::Protect(buffer, length);
}

void CodeBuffer::Call() {
    reinterpret_cast<void (*)()>(buffer)();
}

uintptr_t CodeBuffer::Position() const {
    return reinterpret_cast<uintptr_t>(reinterpret_cast<uint8_t*>(buffer) + pos);
}

void CodeBuffer::Byte(uint8_t b) {
    *(reinterpret_cast<uint8_t*>(buffer) + (pos++)) = b;
}

void CodeBuffer::Bytes(std::initializer_list<uint8_t> bs) {
    for (auto b: bs) { Byte(b); }
}

void CodeBuffer::Word(uint16_t v) {
    Byte(uint8_t(v));
    Byte(uint8_t(v >> 8u));
};

void CodeBuffer::DWord(uint32_t v) {
    Word(uint16_t(v));
    Word(uint16_t(v >> 16u));
}

void CodeBuffer::QWord(uint64_t v) {
    DWord(uint32_t(v));
    DWord(uint32_t(v >> 32u));
}

}