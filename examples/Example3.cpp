#include "CodeBuffer.h"
#include "EmitterX64.h"
#include "X64.h"
#include "MIPS.h"

namespace {

void CallInterpreterFunction(
        rbrown::EmitterX64 &emitter,
        uintptr_t function,
        rbrown::R3051 &processor,
        uint32_t opcode) {
    using namespace rbrown;
    emitter.MovR64Imm64(RDI, AddressOf(processor));
    emitter.MovR32Imm32(RSI, opcode);
    emitter.Call(function);
}

void EmitAddu(rbrown::EmitterX64& emitter, rbrown::R3051& processor, uint32_t opcode) {
    using namespace rbrown;
    CallInterpreterFunction(emitter, AddressOf(InterpretAddu), processor, opcode);
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

void Example3() {

    using namespace rbrown;

    R3051 processor;
    processor.WriteRegister(1, 100);
    processor.WriteRegister(2, 72);
    processor.WriteRegister(4, 99);
    processor.WriteRegister(5, 77);

    CodeBuffer buffer(1024);

    // Prologue
    EmitterX64 emitter(buffer);
    emitter.PushR64(RBP);
    emitter.MovR64R64(RBP, RSP);

    // Instructions
    // ADDU $3, $1, $2
    // SUBU $6, $4, $5
    for (const uint32_t opcode : { 0x00221821u, 0x00853023u }) {
        Emit(emitter, processor, opcode);
    }

    // Epilogue
    emitter.MovR64R64(RSP, RBP);
    emitter.PopR64(RBP);
    emitter.Ret();

    buffer.Protect();
    buffer.Call();

}
