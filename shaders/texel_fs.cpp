typedef unsigned int uint;

uint square(uint a, uint b) {
    return a * b;
}

/*
https://git.eden-emu.dev/eden-emu/eden/pulls/3122/files
https://raw.githubusercontent.com/red-prig/fpPS4/8ce66084eb982c07873ee04338173f68d6faad98/chip/ps4_pssl.pas
https://github.com/shadps4-emu/shadPS4/blob/3bd6a0f9f80e264e4ebbe54bfdc6a3b9ba0137e2/src/shader_recompiler/frontend/instruction.h#L72
https://www.scs.stanford.edu/~zyedidia/arm64/bsl_advsimd.html
https://llvm.org/docs/AMDGPUUsage.html#target-triples
*/
