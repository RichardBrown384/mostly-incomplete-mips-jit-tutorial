#include "RecomilerState.h"
#include "CodeBuffer.h"
#include "EmitterX64.h"
#include "X64.h"
#include "MIPS.h"

// The convention is that we always want to branch
// We therefore set the value at RBP - 8 to be 1
// And only clear it if we don't want to branch

constexpr uint8_t BRANCH_DECISION_OFFSET = -8;

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

void EmitBltzal(
    rbrown::RecompilerState &state,
    rbrown::EmitterX64 &emitter,
    rbrown::R3051 &processor,
    uint32_t opcode) {
    using namespace rbrown;
    // Extract fields from instruction
    const uint32_t rs = InstructionRs(opcode);
    const uint32_t offset = InstructionImmediateExtended(opcode) << 2u;
    const size_t size = sizeof(uint32_t);

    // Load the address of the registers into RDX
    emitter.MovR64Imm64(RDX, processor.RegisterAddress(0));

    // Write the PC plus 8 to R31
    emitter.MovR32Imm32(RAX, state.GetPC() + 8u);
    emitter.MovDisp8R32(RDX, static_cast<uint8_t>(31u * size), RAX);

    // Load RS into RAX
    emitter.MovR32Disp8(RAX, RDX, static_cast<uint8_t>(rs * size));

    // Compare RAX with zero
    emitter.CmpR32Imm8(RAX, 0u);

    // If RAX was negative skip clearing the BRANCH_DECISION flag
    Label resume = emitter.NewLabel();
    emitter.Js(resume);
    emitter.MovR32Imm32(RAX, 0u); // EOR RAX, RAX would be better
    emitter.MovDisp8R32(RBP, BRANCH_DECISION_OFFSET, RAX);
    emitter.Bind(resume);

    // Do necessary bookkeeping
    state.SetBranchDelaySlotNext(true);
    state.SetBranchTarget(state.GetPC() + 4u + offset);
}

void Emit(rbrown::RecompilerState &state,
          rbrown::EmitterX64 &emitter,
          rbrown::R3051 &processor,
          uint32_t opcode) {
    using namespace rbrown;
    switch (InstructionOp(opcode)) {
        case 0x01u:
            switch (InstructionRt(opcode)) {
                case 0x10u: return EmitBltzal(state, emitter, processor, opcode);
                default: break;
            }
        default:break;
    }
}

}

void Example10() {

    using namespace rbrown;

    R3051 processor;
    processor.WriteRegister(8, 0x00000001u);

    // Set up recompiler state
    RecompilerState state(200u);

    CodeBuffer buffer(1024);

    // Prologue
    EmitterX64 emitter(buffer);
    emitter.PushR64(RBP);
    emitter.MovR64R64(RBP, RSP);
    emitter.SubR64Imm8(RSP, 0x10);
    emitter.MovR32Imm32(RAX, 1u); // Move 1 into the branch decision variable to indicate we want to branch
    emitter.MovDisp8R32(RBP, BRANCH_DECISION_OFFSET, RAX);

    // Instructions
    // BLTZAL R8, 120 : 0510001e
    // NOP            : 00000000
    for (const uint32_t opcode : {0x0510001eu, 0x00000000u}) {
        Emit(state, emitter, processor, opcode);
        state.SetPC(state.GetPC() + 4u);
        if (state.GetBranchDelaySlot()) {
            break;
        }
        bool branchDelaySlotNext = state.GetBranchDelaySlotNext();
        state.SetBranchDelaySlot(branchDelaySlotNext);
        state.SetBranchDelaySlotNext(false);
    }

    // Load the branch decision into RAX
    Label resume = emitter.NewLabel();
    Label epilogue = emitter.NewLabel();
    emitter.MovR32Disp8(RAX, RBP, BRANCH_DECISION_OFFSET);

    // Fix up the processor's program counter appropriately
    emitter.CmpR32Imm8(RAX, 1u);
    emitter.Jne(resume);
    CallInterpreterFunction(emitter, AddressOf(WritePC), processor, state.GetBranchTarget());
    emitter.Jmp(epilogue);
    emitter.Bind(resume);
    CallInterpreterFunction(emitter, AddressOf(WritePC), processor, state.GetPC());
    emitter.Bind(epilogue);

    // Epilogue
    emitter.AddR64Imm8(RSP, 0x10);
    emitter.MovR64R64(RSP, RBP);
    emitter.PopR64(RBP);
    emitter.Ret();

    buffer.Protect();
    buffer.Call();

}