#pragma once

#include <cstddef>

namespace rbrown {

class CallSite {
public:
    explicit CallSite(size_t);
    [[nodiscard]] size_t Position() const;
private:
    size_t position;
};

}
