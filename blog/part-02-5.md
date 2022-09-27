# Dynamic recompilation Part 2.5
## It's the story of a man, a woman, and a rabbit in a triangle of trouble.

### Introduction
In this part we're going to discuss a more conventional way to align our stack on a 16-byte boundary.

We're going to introduce a couple of stack manipulation instructions to our emitter and
outline at a very high level the concept of a stack frame. This will become important in the
later parts of the series.

As you might guess from the title, this part wasn't originally planned.

The material was found to be necessary in the penultimate part which deals
with recompiling one of the trickier MIPS instructions. The article was already long
and having to explain two difficult concepts at once was making the situation worse.

The later parts were written after a long break and I came to the realisation that
the earlier articles weren't entirely correct and I have a responsibility in this
series, elementary as it is, to not spread misinformation.

### Recap
Last time we got as far as calling a no-argument void returning function

```c++
void HelloWorld() {
    std::cout << "hello world" << std::endl;
}

CodeBuffer buffer(1024);

EmitterX64 emitter(buffer);
emitter.SubR64Imm8(RSP, 8);
emitter.Call(AddressOf(HelloWorld));
emitter.AddR64Imm8(RSP, 8);
emitter.Ret();

buffer.Protect();
buffer.Call();
```

by aligning the stack pointer, RSP, on a 16-byte boundary immediately
prior to the call by subtracting 8 and then restoring the value of RSP immediately afterward.

### Framing the problem
While this works in the given context, it doesn't follow convention and might
introduce or cause problems later. For example, debuggers rely on convention 
to be able to provide information about stack frames to developers.
Making our code hard to debug is definitely not a good thing.

As it happens, we've already seen the convention in the previous part. In the
disassembly of the **Call** method we can see the function prologue and function epilogue.

```text
(lldb) disassemble
tutorial`rbrown::CodeBuffer::Call:
0x10bf51700 <+0>:  pushq  %rbp
0x10bf51701 <+1>:  movq   %rsp, %rbp
0x10bf51704 <+4>:  subq   $0x10, %rsp
...
0x10bf51712 <+18>: addq   $0x10, %rsp
0x10bf51716 <+22>: popq   %rbp
0x10bf51717 <+23>: retq
```

The prologue pushes **RBP** onto the stack, moves the current value of **RSP** into **RBP**
before subtracting 16 from **RSP**.

The epilogue adds 16 to **RSP** and pops **RBP** off the stack before returning to the caller.

### Why do functions do this?
**RBP** is sometimes referred to as the _frame pointer_ and when our function is called
this register contains the caller's frame pointer. To establish our function's own
frame pointer using **RBP** we need to store the original value somewhere. 
The prologue achieves this by pushing **RBP** onto the stack.

The nice side effect of putting an 8-byte register like **RBP** onto the stack
is that **RSP** is also decremented by 8 and our stack is now 16-byte aligned.

The function can now set up its own frame pointer by copying the contents of 
**RSP** into **RBP**. **RBP** then remains constant for the duration of the function.
This means that if we ever want to know the original value of **RSP** we can 
do so by consulting **RBP**.

This is helpful because C or C++ functions store their local variables in the 
stack and over the lifetime of the function **RSP** can change as additional 
registers are popped or pushed. Having a constant reference point helps the 
function determine the address of a local variable in memory more easily.

The prologue subtracts 16 from **RSP** to reserve space for local variables. 
In effect, this marks the memory between **RSP** and **RBP** for the function's 
exclusive use. The local variables are then referenced relative to the frame pointer **RBP**.

At the end of the function we need to restore both **RBP** and **RSP** to their
original values to that the caller can safely reference its own stack frame. 
As with all things stack related, we need to perform the operations in reverse on the way out. 
The epilogue adds 16 to **RSP** to free up the stack space and then pops the old 
value of **RBP** from the stack. Alternatively, instead of adding 16 to **RSP** 
we can restore the original value by moving RBP into RSP. 

_Note: As usual, this isn't the entire story. GCC has a command line switch which instructs it
to not use frame pointers. This has two benefits; one, **RBP** is freed up for general use and on 
a register scarce architecture like x86–64 this can have a performance improvement
and two, the generated code is smaller. However, let's go with convention._

### Yours, conventionally
Let's now set about the task of being able to emit a correct function epilogue and prologue.

To begin with, we're going to need to add two very important instructions[^1][^2] to our **Emitter** class.
These will allow us to interact with the stack and move registers to and from it

```c++
void EmitterX64::PushR64(uint32_t rd) {
    // PUSH rd
    const uint8_t rex = Rex(0, 0, 0, rd >> 3u);
    const uint8_t code = 0x50u + (rd & 7u);
    buffer.Bytes({ rex, code });
}

void EmitterX64::PopR64(uint32_t rd) {
    // POP rd
    const uint8_t rex = Rex(0, 0, 0, rd >> 3u);
    const uint8_t code = 0x58u + (rd & 7u);
    buffer.Bytes({ rex, code });
}
```

We emit the **REX** prefixes for both instructions since we want to be able to
push or pop any of the 16 registers. Interestingly, **REX.W** should have no 
effect here since we always want to push or pop a 64-bit register.

We're then going to need to be able to move 64-bit registers around within the host processor,
so we'll need the following instruction

```c++
void EmitterX64::MovR64R64(uint32_t rm, uint32_t reg) {
    // MOV rm, reg
    const uint8_t rex = Rex(1u, reg >> 3u, 0, rm >> 3u);
    const uint8_t mod = ModRM(3u, reg, rm);
    buffer.Bytes({ rex, 0x89u, mod });
}
```

Putting this all together we can now revise our original solution slightly to get

```c++
void HelloWorld() {
    std::cout << "hello world" << std::endl;
}

CodeBuffer buffer(1024);

EmitterX64 emitter(buffer);
emitter.PushR64(RBP);
emitter.MovR64R64(RBP, RSP);

emitter.Call(AddressOf(HelloWorld));

emitter.MovR64R64(RSP, RBP);
emitter.PopR64(RBP);
emitter.Ret();

buffer.Protect();
buffer.Call();
```

Since we have no need for local variables at this time, we don't subtract 
a multiple of 16 from RSP in the prologue to reserve stack space for our function.

### Conclusion
In this article we discussed stack frame conventions and how to modify our code
accordingly to use this convention. We also touched upon local variables that might become important later.

Next time we'll discuss a little about passing arguments to these functions to 
make them more useful and discuss the concepts of caller and callee save registers.

### References

[^1]: [Intel Software Developer's Manual][intel-reference-manual]
[^2]: [Felix Cloutier's Intel Online Reference][intel-online-reference-cloutier]

[intel-reference-manual]: https://software.intel.com/content/www/us/en/develop/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4.html
[intel-online-reference-cloutier]: https://www.felixcloutier.com/x86/