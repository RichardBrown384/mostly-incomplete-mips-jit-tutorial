#ifndef RBROWN_EMITTERX64_H
#define RBROWN_EMITTERX64_H

#include <cstdint>
#include <memory>

namespace rbrown {

class CodeBuffer;

class EmitterX64 {
public:
    explicit EmitterX64(CodeBuffer&);
    void AddR32R32(uint32_t, uint32_t);
    void AddR64Imm8(uint32_t, uint8_t);
    void SubR64Imm8(uint32_t, uint8_t);
    void MovR32R32(uint32_t, uint32_t);
    void MovR32Imm32(uint32_t, uint32_t);
    void MovR64Imm64(uint32_t, uint64_t);
    void MovEAXAbs(uintptr_t);
    void MovAbsEAX(uintptr_t);
    void PushR64(uint32_t);
    void PopR64(uint32_t);
    void CallRel32(uint32_t);
    void Call(uintptr_t);
    void Ret();
private:
    CodeBuffer& buffer;
};

template<typename T>
uintptr_t AddressOf(T& target) {
    return reinterpret_cast<uintptr_t>(std::addressof(target));
}

}

#endif
