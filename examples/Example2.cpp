#include "CodeBuffer.h"
#include "EmitterX64.h"
#include "X64.h"

namespace {

struct Processor {
    [[maybe_unused]] uint32_t x;
    uint32_t y;
};

void Add(Processor* processor, uint32_t opcode) {
    processor->y += opcode;
}

}


void Example2() {

    using namespace rbrown;

    Processor processor { 9, 19 };
    const uint32_t opcode = 12u;

    CodeBuffer buffer(1024);

    EmitterX64 emitter(buffer);
    emitter.MovR64Imm64(RDI, AddressOf(processor));
    emitter.MovR32Imm32(RSI, opcode);
    emitter.SubR64Imm8(RSP, 8u);
    emitter.Call(AddressOf(Add));
    emitter.AddR64Imm8(RSP, 8u);
    emitter.Ret();

    buffer.Protect();
    buffer.Call();

}