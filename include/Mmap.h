#pragma once

#include <cstddef>

namespace rbrown::xmmap {

void* Map(size_t);

int Protect(void*, size_t);

int Unmap(void*, size_t);

}
