#include "CodeBuffer.h"
#include "EmitterX64.h"
#include "X64.h"
#include "MIPS.h"

namespace {

void EmitAddu(rbrown::EmitterX64& emitter, rbrown::R3501& processor, uint32_t opcode) {
    // Rd = Rs + Rt
    using namespace rbrown;
    const uint32_t rs = InstructionRs(opcode);
    const uint32_t rt = InstructionRt(opcode);
    const uint32_t rd = InstructionRd(opcode);
    const size_t size = sizeof(uint32_t);
    emitter.MovR64Imm64(RDX, processor.RegisterAddress(0));
    emitter.MovR32Disp8(RAX, RDX, rs * size);
    emitter.MovR32Disp8(RCX, RDX, rt * size);
    emitter.AddR32R32(RAX, RCX);
    emitter.MovDisp8R32(RDX, rd * size, RAX);
}

void EmitSubu(rbrown::EmitterX64& emitter, rbrown::R3501& processor, uint32_t opcode) {
    // Rd = Rs - Rt
    using namespace rbrown;
    const uint32_t rs = InstructionRs(opcode);
    const uint32_t rt = InstructionRt(opcode);
    const uint32_t rd = InstructionRd(opcode);
    const size_t size = sizeof(uint32_t);
    emitter.MovR64Imm64(RDX, processor.RegisterAddress(0));
    emitter.MovR32Disp8(RAX, RDX, rs * size);
    emitter.MovR32Disp8(RCX, RDX, rt * size);
    emitter.SubR32R32(RAX, RCX);
    emitter.MovDisp8R32(RDX, rd * size, RAX);
}

void EmitAddiu(rbrown::EmitterX64& emitter, rbrown::R3501& processor, uint32_t opcode) {
    // Rt = Rs + Imm
    using namespace rbrown;
    const uint32_t rs = InstructionRs(opcode);
    const uint32_t rt = InstructionRt(opcode);
    const uint32_t immediate = InstructionImmediateExtended(opcode);
    const size_t size = sizeof(uint32_t);
    emitter.MovR64Imm64(RDX, processor.RegisterAddress(0));
    emitter.MovR32Disp8(RAX, RDX, rs * size);
    emitter.AddR32Imm32(RAX, immediate);
    emitter.MovDisp8R32(RDX, rt * size, RAX);
}

void Emit(rbrown::EmitterX64& emitter, rbrown::R3501& processor, uint32_t opcode) {
    using namespace rbrown;
    switch (InstructionOp(opcode)) {
        case 0x00: switch(InstructionFunction(opcode)) {
            case 0x21: return EmitAddu(emitter, processor, opcode);
            case 0x23: return EmitSubu(emitter, processor, opcode);
            default:
                break;
        }
        case 0x09: return EmitAddiu(emitter, processor, opcode);
        default:
            break;
    }
}
}

void Example6() {

    using namespace rbrown;

    R3501 processor;
    processor.WriteRegister(1, 100);
    processor.WriteRegister(2, 72);
    processor.WriteRegister(4, 99);
    processor.WriteRegister(5, 77);
    processor.WriteRegister(10, 8900);

    CodeBuffer buffer(1024);

    // Prologue
    EmitterX64 emitter(buffer);
    emitter.PushR64(RDI);
    emitter.PushR64(RSI);
    emitter.PushR64(RAX);
    emitter.PushR64(RCX);
    emitter.PushR64(RDX);

    // Instructions
    // ADDU $3, $1, $2
    // SUBU $6, $4, $5
    // ADDIU $11, $10, 2000
    for (const uint32_t opcode : { 0x00221821u, 0x00853023u, 0x254B07D0u }) {
        Emit(emitter, processor, opcode);
    }

    // Epilogue
    emitter.PopR64(RDX);
    emitter.PopR64(RCX);
    emitter.PopR64(RAX);
    emitter.PopR64(RSI);
    emitter.PopR64(RDI);
    emitter.Ret();

    buffer.Protect();
    buffer.Call();

}
