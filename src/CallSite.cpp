#include "CallSite.h"

namespace rbrown {

CallSite::CallSite(size_t v) : position(v) {}

size_t CallSite::Position() const { return position; }

}