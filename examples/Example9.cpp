#include "RecomilerState.h"
#include "CodeBuffer.h"
#include "EmitterX64.h"
#include "X64.h"
#include "MIPS.h"

constexpr uint8_t LOAD_DELAY_VALUE_OFFSET = -4;

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

void CallLoadWord(
        rbrown::EmitterX64 &emitter,
        rbrown::R3051 &processor,
        uint32_t opcode,
        uint8_t stackOffset) {
    using namespace rbrown;
    const uint32_t rs = InstructionRs(opcode);
    const uint32_t offset = InstructionImmediateExtended(opcode);
    const size_t size = sizeof(uint32_t);
    emitter.MovR64Imm64(RDI, AddressOf(processor));
    emitter.MovR64Imm64(RSI, processor.RegisterAddress(0));
    emitter.MovR32Disp8(RSI, RSI, static_cast<uint8_t>(rs * size));
    emitter.AddR32Imm32(RSI, offset);
    emitter.LeaR64Disp8(RDX, RBP, stackOffset);
    emitter.Call(AddressOf(LoadWord));
}

void CallSetLoadDelayValue(
    rbrown::EmitterX64 &emitter,
    rbrown::R3051 &processor,
    uint32_t stackOffset) {
    using namespace rbrown;
    emitter.MovR64Imm64(RDI, AddressOf(processor));
    emitter.MovR32Disp8(RSI, RBP, stackOffset);
    emitter.Call(AddressOf(SetLoadDelayValue));
}

void WriteGuestRegisterFromStack(
        rbrown::EmitterX64 &emitter,
        rbrown::R3051 &processor,
        uint32_t rt,
        uint8_t stackOffset) {
    using namespace rbrown;
    const size_t size = sizeof(uint32_t);
    emitter.MovR32Disp8(RAX, RBP, stackOffset);
    emitter.MovR64Imm64(RCX, processor.RegisterAddress(0));
    emitter.MovDisp8R32(RCX, static_cast<uint8_t>(rt * size), RAX);
}

void EmitLw(
        rbrown::RecompilerState &state,
        rbrown::EmitterX64 &emitter,
        rbrown::R3051 &processor,
        uint32_t opcode) {
    // Load word
    using namespace rbrown;
    Label resume = emitter.NewLabel();
    const uint32_t rt = InstructionRt(opcode);
    if (state.GetLoadDelaySlot()) {
        const uint32_t dr = state.GetLoadDelayRegister();
        if (rt != dr) {
            WriteGuestRegisterFromStack(emitter, processor, dr, LOAD_DELAY_VALUE_OFFSET);
        }
        state.SetLoadDelaySlot(false);
    }
    // Restore the program counter
    CallInterpreterFunction(emitter, AddressOf(WritePC), processor, 0xBADC0FFE);
    // Call load word
    CallLoadWord(emitter, processor, opcode, LOAD_DELAY_VALUE_OFFSET);
    // Return from recompiled function in event of an exception
    emitter.TestALImm8(1u);
    emitter.Jne(resume);
    CallSetLoadDelayValue(emitter, processor, LOAD_DELAY_VALUE_OFFSET);
    CallInterpreterFunction(emitter, AddressOf(SetLoadDelayRegister), processor, 0u);
    CallInterpreterFunction(emitter, AddressOf(SetLoadDelaySlotNext), processor, false);
    CallInterpreterFunction(emitter, AddressOf(SetLoadDelaySlot), processor, false);
    emitter.MovR64R64(RSP, RBP);
    emitter.PopR64(RBP);
    emitter.Ret();
    emitter.Bind(resume);
    // Do necessary bookkeeping
    state.SetLoadDelaySlotNext(true);
    state.SetLoadDelayRegister(rt);
}

void Emit(rbrown::RecompilerState &state,
          rbrown::EmitterX64 &emitter,
          rbrown::R3051 &processor,
          uint32_t opcode) {
    using namespace rbrown;
    switch (InstructionOp(opcode)) {
        case 0x23: return EmitLw(state, emitter, processor, opcode);
        default:break;
    }
}
}

void Example9() {

    using namespace rbrown;

    R3051 processor;
    processor.WriteRegister(1, 0x00000020u);
    processor.WriteRegister(2, 0x00000070u);

    // Simulate interpreter running LW $1, $2, 64
    // Updating load delay slot as relevant
    InterpretLw(&processor, 0x8c410040);
    processor.SetLoadDelaySlot(processor.GetLoadDelaySlotNext());
    processor.SetLoadDelaySlotNext(false);

    // Set up recompiler state from interpreter state
    RecompilerState state(0u);
    state.SetLoadDelayRegister(processor.GetLoadDelayRegister());
    state.SetLoadDelaySlotNext(processor.GetLoadDelaySlotNext());
    state.SetLoadDelaySlot(processor.GetLoadDelaySlot());

    CodeBuffer buffer(1024);

    // Prologue
    EmitterX64 emitter(buffer);
    emitter.PushR64(RBP);
    emitter.MovR64R64(RBP, RSP);
    emitter.SubR64Imm8(RSP, 0x10);
    emitter.MovR32Imm32(RAX, processor.GetLoadDelayValue()); // Move the interpreter load delayed value onto the stack
    emitter.MovDisp8R32(RBP, LOAD_DELAY_VALUE_OFFSET, RAX);

    // Instructions
    // LW $1, $2, 64 : 8c410040
    // NOP           : 00000000
    for (const uint32_t opcode : {0x8c410040u, 0x00000000u}) {
        Emit(state, emitter, processor, opcode);
        if (state.GetLoadDelaySlot()) {
            const uint32_t dr = state.GetLoadDelayRegister();
            WriteGuestRegisterFromStack(emitter, processor, dr, LOAD_DELAY_VALUE_OFFSET);
        }
        bool loadDelaySlotNext = state.GetLoadDelaySlotNext();
        state.SetLoadDelaySlot(loadDelaySlotNext);
        state.SetLoadDelaySlotNext(false);
    }

    // Epilogue
    CallSetLoadDelayValue(emitter, processor, LOAD_DELAY_VALUE_OFFSET);
    CallInterpreterFunction(emitter, AddressOf(SetLoadDelayRegister), processor, state.GetLoadDelayRegister());
    CallInterpreterFunction(emitter, AddressOf(SetLoadDelaySlotNext), processor, state.GetLoadDelaySlotNext());
    CallInterpreterFunction(emitter, AddressOf(SetLoadDelaySlot), processor, state.GetLoadDelaySlot());
    emitter.AddR64Imm8(RSP, 0x10);
    emitter.MovR64R64(RSP, RBP);
    emitter.PopR64(RBP);
    emitter.Ret();

    buffer.Protect();
    buffer.Call();

}