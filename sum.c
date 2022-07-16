#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#define LLVMModuleRef void*
#define LLVMTypeRef void*
#define LLVMValueRef void*
#define LLVMBasicBlockRef void*
#define LLVMBuilderRef void*
#define LLVMExecutionEngineRef void*

#define LLVMAbortProcessAction 0

void load_llvm();

void* (*LLVMModuleCreateWithName)(const char*);
void* (*LLVMInt32Type)();
void* (*LLVMFunctionType)(LLVMTypeRef, LLVMTypeRef*, unsigned, unsigned);
void* (*LLVMAddFunction)(LLVMModuleRef, const char*, LLVMTypeRef);
void* (*LLVMAppendBasicBlock)(LLVMValueRef, const char*);
void* (*LLVMCreateBuilder)();
void (*LLVMPositionBuilderAtEnd)(LLVMBuilderRef, LLVMBasicBlockRef);
void* (*LLVMBuildAdd)(LLVMBuilderRef, LLVMValueRef, LLVMValueRef, const char*);
void* (*LLVMGetParam)(LLVMValueRef, unsigned);
void* (*LLVMBuildRet)(LLVMBuilderRef, LLVMValueRef);
void* (*LLVMVerifyModule)(LLVMModuleRef, unsigned, char**);
void (*LLVMDisposeMessage)(char*);
void (*LLVMDumpModule)(LLVMModuleRef);
void (*LLVMLinkInMCJIT)();
int (*LLVMInitializeX86Target)();
int (*LLVMInitializeX86AsmPrinter)();
int (*LLVMCreateExecutionEngineForModule)(LLVMExecutionEngineRef*, LLVMModuleRef, char**);
void* (*LLVMGetFunctionAddress)(LLVMExecutionEngineRef, const char*);
void (*LLVMDisposeBuilder)(LLVMBuilderRef);
void (*LLVMDisposeExecutionEngine)(LLVMExecutionEngineRef);

int main() {
    load_llvm();

    LLVMModuleRef mod = LLVMModuleCreateWithName("my_module");

    LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMInt32Type() };
    LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 2, 0);
    LLVMValueRef sum = LLVMAddFunction(mod, "sum", ret_type);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(sum, "entry");

    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entry);
    LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sum, 0), LLVMGetParam(sum, 1), "tmp");
    LLVMBuildRet(builder, tmp);

    char *error = NULL;
    LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    LLVMDumpModule(mod);

    LLVMExecutionEngineRef engine;
    error = NULL;
    LLVMLinkInMCJIT();
    LLVMInitializeX86Target();
    LLVMInitializeX86AsmPrinter();
    if (LLVMCreateExecutionEngineForModule(&engine, mod, &error) != 0) {
        fprintf(stderr, "failed to create execution engine\n");
        abort();
    }
    if (error) {
        fprintf(stderr, "error: %s\n", error);
        LLVMDisposeMessage(error);
        exit(EXIT_FAILURE);
    }

    int (*sum_func)(int, int) = (int (*)(int, int))LLVMGetFunctionAddress(engine, "sum");
    printf("%d\n", sum_func(34, 35));

    LLVMDisposeBuilder(builder);
    LLVMDisposeExecutionEngine(engine);
}

#define ADD_DLSYM(name) (name = dlsym(handle, #name))

void load_llvm() {
    void *handle = dlopen("./libLLVM-15git.so", RTLD_LAZY | RTLD_DEEPBIND);
    if (!handle) {
        fprintf(stderr, "failed to load libLLVM\n");
        abort();
    }

    ADD_DLSYM(LLVMModuleCreateWithName);
    ADD_DLSYM(LLVMInt32Type);
    ADD_DLSYM(LLVMFunctionType);
    ADD_DLSYM(LLVMAddFunction);
    ADD_DLSYM(LLVMAppendBasicBlock);
    ADD_DLSYM(LLVMCreateBuilder);
    ADD_DLSYM(LLVMPositionBuilderAtEnd);
    ADD_DLSYM(LLVMBuildAdd);
    ADD_DLSYM(LLVMGetParam);
    ADD_DLSYM(LLVMBuildRet);
    ADD_DLSYM(LLVMVerifyModule);
    ADD_DLSYM(LLVMDisposeMessage);
    ADD_DLSYM(LLVMDumpModule);
    ADD_DLSYM(LLVMLinkInMCJIT);
    ADD_DLSYM(LLVMInitializeX86Target);
    ADD_DLSYM(LLVMInitializeX86AsmPrinter);
    ADD_DLSYM(LLVMCreateExecutionEngineForModule);
    ADD_DLSYM(LLVMGetFunctionAddress);
    ADD_DLSYM(LLVMDisposeBuilder);
    ADD_DLSYM(LLVMDisposeExecutionEngine);
}
