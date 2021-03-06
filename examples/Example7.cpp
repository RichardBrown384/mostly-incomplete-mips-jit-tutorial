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
    emitter.Call(function);
}

void EmitAdd(rbrown::EmitterX64& emitter, rbrown::R3051& processor, uint32_t opcode) {
    // Rd = Rs + Rt
    using namespace rbrown;
    const uint32_t rs = InstructionRs(opcode);
    const uint32_t rt = InstructionRt(opcode);
    const uint32_t rd = InstructionRd(opcode);
    constexpr size_t size = sizeof(uint32_t);
    Label setRegister = emitter.NewLabel();
    emitter.MovR64Imm64(RDX, processor.RegisterAddress(0));
    emitter.MovR32Disp8(RAX, RDX, static_cast<uint8_t>(rs * size));
    emitter.MovR32Disp8(RCX, RDX, static_cast<uint8_t>(rt * size));
    emitter.AddR32R32(RAX, RCX);
    emitter.Jno(setRegister);
    CallInterpreterFunction(emitter, AddressOf(WritePC), processor, 0xBADC0FFE);
    CallInterpreterFunction(emitter, AddressOf(EnterException), processor, ARITHMETIC_OVERFLOW);
    emitter.MovR64R64(RSP, RBP);
    emitter.PopR64(RBP);
    emitter.Ret();
    emitter.Bind(setRegister);
    emitter.MovDisp8R32(RDX, static_cast<uint8_t>(rd * size), RAX);
}


void Emit(rbrown::EmitterX64& emitter, rbrown::R3051& processor, uint32_t opcode) {
    using namespace rbrown;
    switch (InstructionOp(opcode)) {
        case 0x00: switch(InstructionFunction(opcode)) {
            case 0x20: return EmitAdd(emitter, processor, opcode);
            default:
                break;
        }
        default:
            break;
    }
}
}

void Example7() {

    using namespace rbrown;

    R3051 processor;
    processor.WriteRegister(1, 0x40000000u);
    processor.WriteRegister(2, 0x40000000u);

    CodeBuffer buffer(1024);

    // Prologue
    EmitterX64 emitter(buffer);
    emitter.PushR64(RBP);
    emitter.MovR64R64(RBP, RSP);

    // Instructions
    // ADD $3, $1, $2
    // 00221820
    for (const uint32_t opcode : { 0x00221820u }) {
        Emit(emitter, processor, opcode);
    }

    // Epilogue
    emitter.MovR64R64(RSP, RBP);
    emitter.PopR64(RBP);
    emitter.Ret();

    buffer.Protect();
    buffer.Call();

}