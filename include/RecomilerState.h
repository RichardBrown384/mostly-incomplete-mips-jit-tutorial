#pragma once

#include <cstdint>

namespace rbrown {

class RecompilerState {
public:
    RecompilerState();
    [[nodiscard]] uint32_t GetLoadDelayRegister() const;
    [[nodiscard]] bool GetLoadDelaySlot() const;
    [[nodiscard]] bool GetLoadDelaySlotNext() const;

    void SetLoadDelayRegister(uint32_t v);
    void SetLoadDelaySlot(bool v);
    void SetLoadDelaySlotNext(bool v);
private:
    uint32_t loadDelayRegister;
    bool loadDelaySlot;
    bool loadDelaySlotNext;
};

}
