#ifndef RBROWN_EMITTERX64_H
#define RBROWN_EMITTERX64_H

#include <cstdint>
#include <memory>
#include <map>
#include <vector>

#include "Label.h"
#include "CallSite.h"

namespace rbrown {

class CodeBuffer;

class EmitterX64 {
public:
    explicit EmitterX64(CodeBuffer&);
    Label NewLabel();
    void Bind(Label&);
    void AddR32R32(uint32_t, uint32_t);
    void AddR32Imm32(uint32_t, uint32_t);
    void AddR64Imm8(uint32_t, uint8_t);
    void SubR32R32(uint32_t, uint32_t);
    void SubR64Imm8(uint32_t, uint8_t);
    void MovR32R32(uint32_t, uint32_t);
    void MovR32Disp8(uint32_t, uint32_t, uint8_t);
    void MovR32Imm32(uint32_t, uint32_t);
    void MovR64Imm64(uint32_t, uint64_t);
    void MovDisp8R32(uint32_t, uint8_t, uint32_t);
    void MovEAXAbs(uintptr_t);
    void MovAbsEAX(uintptr_t);
    void PushR64(uint32_t);
    void PopR64(uint32_t);
    void CallRel32(uint32_t);
    void Call(uintptr_t);
    void Ret();
    void Jno(const Label&);
private:
    void Bind(const CallSite&, const Label &);
private:
    CodeBuffer& buffer;
    std::map<uint64_t, std::vector<CallSite>> callSites;
    uint64_t nextLabelId;
};

template<typename T>
uintptr_t AddressOf(T& target) {
    return reinterpret_cast<uintptr_t>(std::addressof(target));
}

}

#endif
