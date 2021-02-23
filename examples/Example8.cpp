#include "CodeBuffer.h"
#include "EmitterX64.h"
#include "X64.h"
#include "MIPS.h"

namespace {

void CallInterpreterFunction(
        rbrown::EmitterX64 &emitter,
        uintptr_t function,
        rbrown::R3051 &processor,
        uint32_t arg1) {
    using namespace rbrown;
    emitter.MovR64Imm64(RDI, AddressOf(processor));
    emitter.MovR32Imm32(RSI, arg1);
    emitter.SubR64Imm8(RSP, 8u);
    emitter.Call(function);
    emitter.AddR64Imm8(RSP, 8u);
}

void EmitSw(rbrown::EmitterX64& emitter, rbrown::R3051& processor, uint32_t opcode) {
    // Store word
    using namespace rbrown;
    const uint32_t rs = InstructionRs(opcode);
    const uint32_t rt = InstructionRt(opcode);
    const uint32_t immediate = InstructionImmediateExtended(opcode);
    const size_t size = sizeof(uint32_t);
    Label resume = emitter.NewLabel();
    CallInterpreterFunction(emitter, AddressOf(WritePC), processor, 0xBADC0FFE);
    emitter.MovR64Imm64(RDI, AddressOf(processor));
    emitter.MovR64Imm64(RDX, processor.RegisterAddress(0));
    emitter.MovR32Disp8(RSI, RDX, static_cast<uint8_t>(rs * size));
    emitter.AddR32Imm32(RSI, immediate);
    emitter.MovR32Disp8(RDX, RDX, static_cast<uint8_t>(rt * size));
    emitter.SubR64Imm8(RSP, 8u);
    emitter.Call(AddressOf(StoreWord));
    emitter.AddR64Imm8(RSP, 8u);
    emitter.TestALImm8(1u);
    emitter.Jne(resume);
    emitter.Ret();
    emitter.Bind(resume);
}

void Emit(rbrown::EmitterX64& emitter, rbrown::R3051& processor, uint32_t opcode) {
    using namespace rbrown;
    switch (InstructionOp(opcode)) {
        case 0x2b: return EmitSw(emitter, processor, opcode);
        default:
            break;
    }
}
}

void Example8() {

    using namespace rbrown;

    R3051 processor;
    processor.WriteRegister(1, 0x00000020u);
    processor.WriteRegister(2, 0x00000070u);

    CodeBuffer buffer(1024);

    // Prologue
    EmitterX64 emitter(buffer);

    // Instructions
    // SW $1, $2, 64
    // ac220040
    for (const uint32_t opcode : { 0xac220040 }) {
        Emit(emitter, processor, opcode);
    }

    // Epilogue
    emitter.Ret();

    buffer.Protect();
    buffer.Call();

}