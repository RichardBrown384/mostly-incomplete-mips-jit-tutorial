#include "Label.h"

namespace rbrown {

Label::Label(uint64_t v) : id(v), position(0), bound(false) {}

uint64_t Label::Id() const { return id; }

size_t Label::Position() const { return position; }

bool Label::Bound() const { return bound; }

void Label::Bind(size_t v) {
    if (bound) {
        return;
    }
    position = v;
    bound = true;
}

}

