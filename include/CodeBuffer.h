#ifndef RBROWN_CODE_BUFFER_H
#define RBROWN_CODE_BUFFER_H

#pragma once

#include <cstddef>
#include <cstdint>
#include <initializer_list>

namespace rbrown {

class CodeBuffer {
public:
    explicit CodeBuffer(size_t);
    ~CodeBuffer();
    void Protect();
    void Call();
    [[nodiscard]] uintptr_t BufferAddress() const;
    [[nodiscard]] size_t Position() const;
    void Byte(uint8_t);
    void Byte(size_t, uint8_t);
    void Bytes(std::initializer_list<uint8_t>);
    void Word(uint16_t);
    void DWord(uint32_t);
    void QWord(uint64_t);
private:
    void* buffer;
    size_t length;
    size_t pos;
};

}
#endif
