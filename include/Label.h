#pragma once

#include <cstdint>
#include <memory>

namespace rbrown {

class Label {
public:
    explicit Label(uint64_t);
    [[nodiscard]] uint64_t Id() const;
    [[nodiscard]] size_t Position() const;
    [[nodiscard]] bool Bound() const;
    void Bind(size_t);
private:
    uint64_t id;
    size_t position;
    bool bound;
};

}
