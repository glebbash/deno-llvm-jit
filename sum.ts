const ptr = "pointer";
const LLVMAbortProcessAction = 0;

const llvm = loadLLVM();

const mod = llvm.LLVMModuleCreateWithName(cstr("my_module"));

const param_types = [llvm.LLVMInt32Type(), llvm.LLVMInt32Type()];
const ret_type = llvm.LLVMFunctionType(
  llvm.LLVMInt32Type(),
  ptrs(param_types),
  2,
  0,
);
const sum = llvm.LLVMAddFunction(mod, cstr("sum"), ret_type);

const entry = llvm.LLVMAppendBasicBlock(sum, cstr("entry"));

const builder = llvm.LLVMCreateBuilder();
llvm.LLVMPositionBuilderAtEnd(builder, entry);
const tmp = llvm.LLVMBuildAdd(
  builder,
  llvm.LLVMGetParam(sum, 0),
  llvm.LLVMGetParam(sum, 1),
  cstr("tmp"),
);
llvm.LLVMBuildRet(builder, tmp);

let error = new BigUint64Array(1);
llvm.LLVMVerifyModule(mod, LLVMAbortProcessAction, error);
llvm.LLVMDisposeMessage(error[0]);

llvm.LLVMDumpModule(mod);

const engine = new BigUint64Array(1);
error = new BigUint64Array(1);

llvm.LLVMLinkInMCJIT();

// LLVMInitializeNativeTarget(); is a macro for following 3 calls:
llvm.LLVMInitializeX86TargetInfo();
llvm.LLVMInitializeX86Target();
llvm.LLVMInitializeX86TargetMC();

// LLVMInitializeNativeAsmPrinter(); is a macro for following call:
llvm.LLVMInitializeX86AsmPrinter();

if (llvm.LLVMCreateExecutionEngineForModule(engine, mod, error) != 0) {
  console.error("failed to create execution engine");
  Deno.exit();
}
if (error[0]) {
  console.error("error:", fromCString(error[0]));
  llvm.LLVMDisposeMessage(error[0]);
  Deno.exit(1);
}

const sum_func = new Deno.UnsafeFnPointer(
  llvm.LLVMGetFunctionAddress(engine[0], cstr("sum")),
  {
    parameters: ["i32", "i32"],
    result: "i32",
  },
);
console.log(sum_func.call(34, 35));

llvm.LLVMDisposeBuilder(builder);
llvm.LLVMDisposeExecutionEngine(engine[0]);

// utils

function cstr(str: string) {
  return Uint8Array.from(toCharCodes(str + "\0"));
}

function toCharCodes(str: string): number[] {
  return [...str].map((_, i) => str.charCodeAt(i));
}

function fromCString(pointer: bigint) {
  return new Deno.UnsafePointerView(pointer).getCString();
}

function ptrs(pointers: bigint[]) {
  return Deno.UnsafePointer.of(
    BigInt64Array.from(pointers),
  );
}

function loadLLVM() {
  const lib = Deno.dlopen("./libLLVM-15git.so", {
    LLVMModuleCreateWithName: {
      parameters: [ptr],
      result: ptr,
    },
    LLVMInt32Type: {
      parameters: [],
      result: ptr,
    },
    LLVMFunctionType: {
      parameters: [ptr, ptr, "i32", "i32"],
      result: ptr,
    },
    LLVMAddFunction: {
      parameters: [ptr, ptr, ptr],
      result: ptr,
    },
    LLVMAppendBasicBlock: {
      parameters: [ptr, ptr],
      result: ptr,
    },
    LLVMCreateBuilder: {
      parameters: [],
      result: ptr,
    },
    LLVMPositionBuilderAtEnd: {
      parameters: [ptr, ptr],
      result: ptr,
    },
    LLVMBuildAdd: {
      parameters: [ptr, ptr, ptr, ptr],
      result: ptr,
    },
    LLVMGetParam: {
      parameters: [ptr, "i32"],
      result: ptr,
    },
    LLVMBuildRet: {
      parameters: [ptr, ptr],
      result: ptr,
    },
    LLVMVerifyModule: {
      parameters: [ptr, "i32", ptr],
      result: ptr,
    },
    LLVMDisposeMessage: {
      parameters: [ptr],
      result: "void",
    },
    LLVMDumpModule: {
      parameters: [ptr],
      result: "void",
    },
    LLVMLinkInMCJIT: {
      parameters: [],
      result: "void",
    },
    LLVMInitializeX86TargetInfo: {
      parameters: [],
      result: "i32",
    },
    LLVMInitializeX86Target: {
      parameters: [],
      result: "i32",
    },
    LLVMInitializeX86TargetMC: {
      parameters: [],
      result: "i32",
    },
    LLVMInitializeX86AsmPrinter: {
      parameters: [],
      result: "i32",
    },
    LLVMCreateExecutionEngineForModule: {
      parameters: [ptr, ptr, ptr],
      result: "i32",
    },
    LLVMGetFunctionAddress: {
      parameters: [ptr, ptr],
      result: "function",
    },
    LLVMDisposeBuilder: {
      parameters: [ptr],
      result: "void",
    },
    LLVMDisposeExecutionEngine: {
      parameters: [ptr],
      result: "void",
    },
  });

  return lib.symbols;
}
