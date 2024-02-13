#include "RecomilerState.h"

namespace rbrown {

RecompilerState::RecompilerState(uint32_t startPc)
    : loadDelayRegister{},
      loadDelaySlot{},
      loadDelaySlotNext{},
      branchTarget{},
      branchDelaySlot{},
      branchDelaySlotNext{},
      pc{ startPc } {}

uint32_t RecompilerState::GetLoadDelayRegister() const { return loadDelayRegister; }
bool RecompilerState::GetLoadDelaySlot() const { return loadDelaySlot; }
bool RecompilerState::GetLoadDelaySlotNext() const { return loadDelaySlotNext; }

uint32_t RecompilerState::GetBranchTarget() const { return branchTarget; }
bool RecompilerState::GetBranchDelaySlot() const { return branchDelaySlot; }
bool RecompilerState::GetBranchDelaySlotNext() const { return branchDelaySlotNext; }

uint32_t RecompilerState::GetPC() const { return pc; }

void RecompilerState::SetLoadDelayRegister(uint32_t v) { loadDelayRegister = v; }
void RecompilerState::SetLoadDelaySlot(bool v) { loadDelaySlot = v; }
void RecompilerState::SetLoadDelaySlotNext(bool v) { loadDelaySlotNext = v; }

void RecompilerState::SetBranchTarget(uint32_t v) { branchTarget = v; }
void RecompilerState::SetBranchDelaySlot(bool v) { branchDelaySlot = v; }
void RecompilerState::SetBranchDelaySlotNext(bool v) { branchDelaySlotNext = v; }

void RecompilerState::SetPC(uint32_t v) { pc = v; }

}
