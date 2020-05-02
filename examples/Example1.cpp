#include "CodeBuffer.h"
#include "EmitterX64.h"
#include "X64.h"

#include <iostream>

namespace {

void HelloWorld() {
    std::cout << "hello world" << std::endl;
}

}

void Example1() {

    // RSP on Darwin has to be 16 byte aligned
    // Prior to calling our RSP is aligned
    // but CALLing a function pushes the 8-byte RIP onto the stack

    using namespace rbrown;

    CodeBuffer buffer(1024);

    EmitterX64 emitter(buffer);
    emitter.SubR64Imm8(RSP, 8);
    emitter.Call(AddressOf(HelloWorld));
    emitter.AddR64Imm8(RSP, 8);
    emitter.SubR64Imm8(RSP, 8);
    emitter.Call(AddressOf(HelloWorld));
    emitter.AddR64Imm8(RSP, 8);
    emitter.SubR64Imm8(RSP, 8);
    emitter.Call(AddressOf(HelloWorld));
    emitter.AddR64Imm8(RSP, 8);
    emitter.Ret();

    buffer.Protect();
    buffer.Call();

}