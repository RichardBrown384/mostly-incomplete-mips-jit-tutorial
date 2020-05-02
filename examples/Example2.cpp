#include "CodeBuffer.h"
#include "EmitterX64.h"
#include "X64.h"

#include <cstdint>

namespace {

struct Processor {
    uint32_t x;
    uint32_t y;
};

void Add(Processor* processor, uint32_t instruction) {
    processor->y += instruction;
}

}


void Example2() {

    using namespace rbrown;

    Processor processor { 9, 19 };

    CodeBuffer buffer(1024);

    EmitterX64 emitter(buffer);
    emitter.MovR64Imm64(RDI, AddressOf(processor));
    emitter.MovR32Imm32(RSI, 12u);
    emitter.SubR64Imm8(RSP, 8u);
    emitter.Call(AddressOf(Add));
    emitter.AddR64Imm8(RSP, 8u);
    emitter.Ret();

    buffer.Protect();
    buffer.Call();

}