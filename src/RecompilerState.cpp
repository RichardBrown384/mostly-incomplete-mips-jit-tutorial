#include "RecomilerState.h"

namespace rbrown {

RecompilerState::RecompilerState()
        : loadDelayRegister{},
          loadDelaySlot{},
          loadDelaySlotNext{} {}

uint32_t RecompilerState::GetLoadDelayRegister() const { return loadDelayRegister; }
bool RecompilerState::GetLoadDelaySlot() const { return loadDelaySlot; }
bool RecompilerState::GetLoadDelaySlotNext() const { return loadDelaySlotNext; }

void RecompilerState::SetLoadDelayRegister(uint32_t v) { loadDelayRegister = v; }
void RecompilerState::SetLoadDelaySlot(bool v) { loadDelaySlot = v; }
void RecompilerState::SetLoadDelaySlotNext(bool v) { loadDelaySlotNext = v; }

}
