#pragma once

#include <cstdint>

namespace rbrown {

class RecompilerState {
public:
    explicit RecompilerState(uint32_t startPc);
    [[nodiscard]] uint32_t GetLoadDelayRegister() const;
    [[nodiscard]] bool GetLoadDelaySlot() const;
    [[nodiscard]] bool GetLoadDelaySlotNext() const;
    [[nodiscard]] uint32_t GetBranchTarget() const;
    [[nodiscard]] bool GetBranchDelaySlot() const;
    [[nodiscard]] bool GetBranchDelaySlotNext() const;
    [[nodiscard]] uint32_t GetPC() const;

    void SetLoadDelayRegister(uint32_t v);
    void SetLoadDelaySlot(bool v);
    void SetLoadDelaySlotNext(bool v);
    void SetBranchTarget(uint32_t v);
    void SetBranchDelaySlot(bool v);
    void SetBranchDelaySlotNext(bool v);
    void SetPC(uint32_t);
private:
    uint32_t loadDelayRegister;
    bool loadDelaySlot;
    bool loadDelaySlotNext;
    uint32_t branchTarget;
    bool branchDelaySlot;
    bool branchDelaySlotNext;
    uint32_t pc;
};

}
