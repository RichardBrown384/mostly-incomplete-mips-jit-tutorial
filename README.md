# The Mostly Incomplete MIPS JIT Tutorial
Series of examples outlining how to build a x64 JIT for a MIPS processor.

There is an accompanying series of blog articles [here][gh-pages-blog].

## About
This repository comprises a series of exercises written while trying to learn about how one might go about trying to 
dynamically recompile MIPS instructions for the x64 architecture.

There are already some good tutorials online on how one might go and do this for languages like 
[brainfuck][bendersky-jit-tutorial] and I encourage you to read them. The emphasis here is to provide and outline of 
you might evolve an interpreting emulator into a JIT.

This repository does not currently contain a fully working JIT for a MIPS processor.

### Disclaimers
Currently, the examples only work on architectures that conform to the System V AMD64 calling conventions. This means 
the examples should work on MacOS and Linux but not on Windows.

## Examples

### Example 1
In this example we try and figure out how to call a free C++ function from our JIT.

This is so that if we haven't implemented a function in our JIT we can call the interpreted version.

We learn about the x64 16-byte stack [alignment][x64-stack-alignment] requirements.

The discussion [here][x64-call-absolute] about direct vs indirect calls is also worth reading.

### Example 2
In this example we go a step further and pass arguments to our free C++ function.

### Example 3
In this example we introduce a simplified MIPS processor and introduce what is called a _threaded interpreter_. 
This is to say we do a single pass over the instructions and simply call the underlying interpreted function.

### Example 4
In this example we now attempt to re-implement one of the simpler instructions directly by reading and writing the 
guest registers to and from RAM.

### Example 5
In this example we reattempt the previous example using register relative indirect addressing.

### Example 6
In this example we attempt a trivial I-Type instruction.

### Example 7
In this example we attempt to deal with arithmetic overflow.

### Example 8
In this example we attempt to store a register in memory.

## References

1. [Compiler explorer](https://godbolt.org)
1. [Calling conventions](https://www.agner.org/optimize/calling_conventions.pdf)
1. [x64 instruction encoding](https://wiki.osdev.org/X86-64_Instruction_Encoding)
1. [x64 instruction reference](https://www.felixcloutier.com/x86/)
1. [Online Assembler & Disassembler](https://defuse.ca/online-x86-assembler.htm)

***

[gh-pages-blog]: https://richardbrown384.github.io/mostly-incomplete-mips-jit-tutorial/
[bendersky-jit-tutorial]: https://eli.thegreenplace.net/2017/adventures-in-jit-compilation-part-2-an-x64-jit/
[x64-stack-alignment]: https://users.rust-lang.org/t/jit-stack-protection-woes/6710
[x64-call-absolute]: https://stackoverflow.com/questions/19552158/call-an-absolute-pointer-in-x86-machine-code?noredirect=1&lq=1
