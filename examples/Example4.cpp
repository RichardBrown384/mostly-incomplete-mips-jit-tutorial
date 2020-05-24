#include "CodeBuffer.h"
#include "EmitterX64.h"
#include "X64.h"
#include "MIPS.h"

namespace {

void CallInterpreterFunction(rbrown::EmitterX64 &emitter, uintptr_t function, rbrown::R3051 &processor, uint32_t opcode) {
    using namespace rbrown;
    emitter.MovR64Imm64(RDI, AddressOf(processor));
    emitter.MovR32Imm32(RSI, opcode);
    emitter.SubR64Imm8(RSP, 8u);
    emitter.Call(function);
    emitter.AddR64Imm8(RSP, 8u);
}

void EmitAddu(rbrown::EmitterX64& emitter, rbrown::R3051& processor, uint32_t opcode) {
    // Rd = Rs + Rt
    using namespace rbrown;
    const uint32_t rs = InstructionRs(opcode);
    const uint32_t rt = InstructionRt(opcode);
    const uint32_t rd = InstructionRd(opcode);
    emitter.MovEAXAbs(processor.RegisterAddress(rs));
    emitter.MovR32R32(RCX, RAX);
    emitter.MovEAXAbs(processor.RegisterAddress(rt));
    emitter.AddR32R32(RAX, RCX);
    emitter.MovAbsEAX(processor.RegisterAddress(rd));
}

void EmitSubu(rbrown::EmitterX64& emitter, rbrown::R3051& processor, uint32_t opcode) {
    using namespace rbrown;
    CallInterpreterFunction(emitter, AddressOf(InterpretSubu), processor, opcode);
}

void Emit(rbrown::EmitterX64& emitter, rbrown::R3051& processor, uint32_t opcode) {
    using namespace rbrown;
    switch (InstructionOp(opcode)) {
        case 0x00: switch(InstructionFunction(opcode)) {
            case 0x21: return EmitAddu(emitter, processor, opcode);
            case 0x23: return EmitSubu(emitter, processor, opcode);
            default:
                break;
        }
        default:
            break;
    }
}
}

void Example4() {

    using namespace rbrown;

    R3051 processor;
    processor.WriteRegister(1, 100);
    processor.WriteRegister(2, 72);
    processor.WriteRegister(4, 99);
    processor.WriteRegister(5, 77);

    CodeBuffer buffer(1024);

    // Prologue
    EmitterX64 emitter(buffer);
    emitter.PushR64(RDI);
    emitter.PushR64(RSI);
    emitter.PushR64(RAX);
    emitter.PushR64(RCX);
    emitter.SubR64Imm8(RSP, 8);

    // Instructions
    // ADDU $3, $1, $2
    // SUBU $6, $4, $5
    for (const uint32_t opcode : { 0x00221821u, 0x00853023u }) {
        Emit(emitter, processor, opcode);
    }

    // Epilogue
    emitter.AddR64Imm8(RSP, 8);
    emitter.PopR64(RCX);
    emitter.PopR64(RAX);
    emitter.PopR64(RSI);
    emitter.PopR64(RDI);
    emitter.Ret();

    buffer.Protect();
    buffer.Call();

}
