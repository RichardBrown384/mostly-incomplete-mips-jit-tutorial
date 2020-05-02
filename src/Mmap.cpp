#include "Mmap.h"

#include <sys/mman.h>

namespace rbrown::xmmap {

void* Map(size_t length) {
    return mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

int Protect(void* addr, size_t length) {
    return mprotect(addr, length, PROT_READ | PROT_EXEC);
}

int Unmap(void* addr, size_t length) {
    return munmap(addr, length);
}

}