# deno-llvm-jit

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/glebbash/deno-llvm-jit)

A working example of Deno using LLVM JIT compilation.

Deno uses ffi to open `libLLVM-15git.so` and jit compile and execute a sum
function.

Deno source [sum.ts](./sum.ts), follows an equivalent C example
[sum.c](./sum.c).

## Challenges

The hard part about getting this to work was LLVMInitializeNativeTarget call
which is apparently an inline function that calls 3 macros (and that's why you
cannot find this symbol in libLLVM.so).

```c
/** LLVMInitializeNativeTarget - The main program should call this function to
    initialize the native target corresponding to the host.  This is useful
    for JIT applications to ensure that the target gets linked in correctly. */
static inline LLVMBool LLVMInitializeNativeTarget(void) {
  /* If we have a native target, initialize it to ensure it is linked in. */
#ifdef LLVM_NATIVE_TARGET
  LLVM_NATIVE_TARGETINFO();
  LLVM_NATIVE_TARGET();
  LLVM_NATIVE_TARGETMC();
  return 0;
#else
  return 1;
#endif
}
```

## Running example

```sh
deno run -A --unstable sum.ts
```

## Running the equivalent C example

```sh
make && ./sum.c
```

## How libLLVM-15git.so was built

- Warning: Building this takes a really long time (> 1h).

```sh
git clone https://github.com/llvm/llvm-project.git
```

```sh
cmake \
  -DCMAKE_INSTALL_PREFIX="../llvm-out" \
  -DCMAKE_BUILD_TYPE="MinSizeRel" \
  -DLLVM_TARGETS_TO_BUILD="X86;WebAssembly" \
  -DLLVM_BUILD_TOOLS="NO" \
  -DLLVM_BUILD_LLVM_DYLIB="YES" \
  ../llvm
```

```sh
cmake --build . --target install
```
