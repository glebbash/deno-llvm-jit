# deno-llvm-jit

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/glebbash/deno-llvm-jit)

Deno LLVM JIT example

All dependencies can be found in .devcontainer/Dockerfile.base

## Running example

```sh
deno run -A --unstable sum.ts
```

## Running C example

```sh
make

./sum       # using dynamic library
./sum-no-dl # without dynamic library
```

## How libLLVM-15git.so was built

- Warning: Building this takes a really long time (1-2h).

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
