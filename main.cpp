#include "scarygnm.h"
#include "gnmapi.h"

struct GnmMemoryArea {
    off_t dmem_ofs = 0;
    void* dmem_ptr = nullptr;
    void* dmem_cur = nullptr;

    void init(uintptr_t size) {
        int ret;
        ret = sceKernelAllocateMainDirectMemory(size, 16 * 1024, WB_ONION, &dmem_ofs);
        printf("sceKernelAllocateMainDirectMemory->%d %d\n", ret, int(dmem_ofs));
        ret = sceKernelMapDirectMemory(&dmem_ptr, size, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_GPU_ALL, 0, dmem_ofs, 16 * 1024);
        printf("sceKernelMapDirectMemory->%d\n", ret);
        dmem_cur = dmem_ptr;
    }

    template<typename T> T* alloc(uintptr_t size, uintptr_t align) {
        dmem_cur = (void*)((uintptr_t(dmem_cur) + (align - 1)) & (~(align - 1)));
        void* ret = dmem_cur;
        dmem_cur = (void*)(uintptr_t(dmem_cur) + size);
        return (T*)ret;
    }
};

struct GnmCmdBuffer {
    explicit GnmCmdBuffer(uint* base) : base{base}, curr{base} {}

    void Init() {
        int i = Gnm::sceGnmDrawInitDefaultHardwareState350(curr, 0x100);
        curr += i;
    }

    void SetCsShader(Gnm::CsStageRegisters& regs) {
        Gnm::sceGnmSetCsShader((uint*)curr, 24, (void*)&regs);
        curr += 24;
    }

    void DispatchDirect(uint x, uint y, uint z) {
        Gnm::sceGnmDispatchDirect((uint*)curr, 9, x, y, z, 0);
        curr += 9;
    }

    uint* base;
    uint* curr;
};

int main() {
    scarygnm::Context c;
    // V_SAD_U32 v4, v0, v1, abs(v2)
    c.V_SAD_U32(c.v4, c.v0, c.v1, scarygnm::abs(c.v2));
    // V_SAD_U32 v5, v0, v1, -v3
    c.V_SAD_U32(c.v5, c.v0, c.v1, -c.v3);
    // V_SAD_U32 v6, v0, v1, -abs(v2)
    c.V_SAD_U32(c.v6, c.v0, c.v1, -c.v2);
    c.V_BUFFER_STORE_FORMAT_X(c.v0, c.v0, 0, 0, scarygnm::simm32(0), scarygnm::BUF_NONE);
    c.S_ENDPGM();

    GnmMemoryArea gpu_cmdbuf;
    GnmMemoryArea gpu_shader_cs;

    auto shader_buffer = gpu_shader_cs.alloc<void>(32, 4);

    Gnm::CsStageRegisters regs = {};
    constexpr uint32_t vgpr_count = 8;
    constexpr uint32_t sgpr_count = 8;
    constexpr uint32_t user_count = 8;
    constexpr uint32_t lds_size = 0;
    regs.computePgmLo = uintptr_t(shader_buffer) >> 8;
    regs.computePgmHi = uintptr_t(shader_buffer) >> 8 >> 32;
    regs.computeNumThreadX = 1;
    regs.computeNumThreadY = 1;
    regs.computeNumThreadZ = 1;
    regs.computePgmRsrc1.VGPRS = (vgpr_count + 3) / 4 - 1;
    regs.computePgmRsrc1.SGPRS = (sgpr_count + 7) / 8 - 1;
    regs.computePgmRsrc1.FLOAT_MODE = 192;
    regs.computePgmRsrc1.DX10_CLAMP = 1;
    regs.computePgmRsrc2.USER_SGPR = user_count;
    regs.computePgmRsrc2.LDS_SIZE = (lds_size + 511) / 512;

    GnmCmdBuffer cmdbuf(gpu_cmdbuf.alloc<uint>(128, 4));
    cmdbuf.Init();
    cmdbuf.SetCsShader(regs);
    cmdbuf.DispatchDirect(1, 1, 1);
    Gnm::sceGnmSubmitDone();
    return 0;
}

namespace Shader::IR {
    class Inst {

    };
}

namespace Shader::Backend::GNM {

struct EmitContext {
    scarygnm::Operand DefineValue(IR::Inst& inst) {
        return emit.s0;
    }
    scarygnm::Operand ScratchGpr() {
        return emit.s0;
    }
    void Translate();

    scarygnm::Context emit;
    scarygnm::Operand sgpr_zero = scarygnm::Context::s0;
    unsigned shmem_harp = 0;
};
struct NotImplementedException {
    NotImplementedException(const char *what) : what{what} {}
    const char *what = "no what";
};

void EmitContext::Translate() {
    // something idk
}

using namespace scarygnm;
// opcode name, return type, arg1 type, arg2 type, arg3 type, arg4 type, arg4 type, ...
void EmitPhi(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitIdentity(EmitContext& ctx, IR::Inst& inst, Operand a) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_ADD_I32(ret, a, ctx.sgpr_zero);
}
void EmitVoid(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitConditionRef(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitReference(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitPhiMove(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

// Special operations
void EmitPrologue(EmitContext& ctx, IR::Inst& inst) {
    ctx.emit.V_XOR_B32(ctx.sgpr_zero);
}
void EmitEpilogue(EmitContext& ctx, IR::Inst& inst) {
    ctx.emit.S_ENDPGM();
}
void EmitJoin(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitDemoteToHelperInvocation(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitEmitVertex(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitEndPrimitive(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}

// Barriers
void EmitBarrier(EmitContext& ctx, IR::Inst& inst) {
    ctx.emit.S_WAITCNT(1, 1, 1);
}
void EmitWorkgroupMemoryBarrier(EmitContext& ctx, IR::Inst& inst) {
    ctx.emit.S_WAITCNT(1, 1, 1);
}
void EmitDeviceMemoryBarrier(EmitContext& ctx, IR::Inst& inst) {
    ctx.emit.S_WAITCNT(1, 1, 1);
}

// Context getters/setters
void EmitGetRegister(EmitContext& ctx, IR::Inst& inst, Operand reg) {
    throw NotImplementedException("GNM insn");
}
void EmitSetRegister(EmitContext& ctx, IR::Inst& inst, Operand reg, Operand value) {
    throw NotImplementedException("GNM insn");
}
void EmitGetPred(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitSetPred(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetGotoVariable(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitSetGotoVariable(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetIndirectBranchVariable(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitSetIndirectBranchVariable(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitGetCbufU8(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetCbufS8(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetCbufU16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetCbufS16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetCbufU32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetCbufF32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetCbufU32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetAttribute(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetAttributeU32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSetAttribute(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetAttributeIndexed(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSetAttributeIndexed(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGetPatch(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitSetPatch(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSetFragColor(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSetSampleMask(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitSetFragDepth(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitGetZFlag(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitGetSFlag(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitGetCFlag(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitGetOFlag(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitSetZFlag(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitSetSFlag(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitSetCFlag(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitSetOFlag(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitWorkgroupId(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitLocalInvocationId(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitInvocationId(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitInvocationInfo(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitSampleId(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitIsHelperInvocation(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitSR_WScaleFactorXY(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitSR_WScaleFactorZ(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitYDirection(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitResolutionDownFactor(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitRenderArea(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}

// Undefined
void EmitUndefU1(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitUndefU8(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitUndefU16(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitUndefU32(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitUndefU64(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}

// Memory operations
void EmitLoadGlobalU8(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadGlobalS8(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadGlobalU16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadGlobalS16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadGlobal32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadGlobal64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadGlobal128(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteGlobalU8(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteGlobalS8(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteGlobalU16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteGlobalS16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteGlobal32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteGlobal64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteGlobal128(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

// Storage buffer operations
void EmitLoadStorageU8(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadStorageS8(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadStorageU16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadStorageS16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadStorage32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadStorage64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitLoadStorage128(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteStorageU8(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteStorageS8(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteStorageU16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteStorageS16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteStorage32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteStorage64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteStorage128(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

// Local memory operations
void EmitLoadLocal(EmitContext& ctx, IR::Inst& inst, Operand offset) {
    throw NotImplementedException("GNM insn");
}
void EmitWriteLocal(EmitContext& ctx, IR::Inst& inst, Operand offset, Operand value) {
    throw NotImplementedException("GNM insn");
}

// Shared memory operations
void EmitLoadSharedU8(EmitContext& ctx, IR::Inst& inst, Operand offset) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_BUFFER_LOAD_UBYTE(ret, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}
void EmitLoadSharedS8(EmitContext& ctx, IR::Inst& inst, Operand offset) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_BUFFER_LOAD_SBYTE(ret, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}
void EmitLoadSharedU16(EmitContext& ctx, IR::Inst& inst, Operand offset) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_BUFFER_LOAD_USHORT(ret, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}
void EmitLoadSharedS16(EmitContext& ctx, IR::Inst& inst, Operand offset) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_BUFFER_LOAD_SSHORT(ret, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}
void EmitLoadSharedU32(EmitContext& ctx, IR::Inst& inst, Operand offset) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_BUFFER_LOAD_DWORD(ret, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}
void EmitLoadSharedU64(EmitContext& ctx, IR::Inst& inst, Operand offset) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_BUFFER_LOAD_DWORDX2(ret, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}
void EmitLoadSharedU128(EmitContext& ctx, IR::Inst& inst, Operand offset) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_BUFFER_LOAD_DWORDX4(ret, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}
void EmitWriteSharedU8(EmitContext& ctx, IR::Inst& inst, Operand offset, Operand value) {
    ctx.emit.V_BUFFER_STORE_BYTE(value, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}
void EmitWriteSharedU16(EmitContext& ctx, IR::Inst& inst, Operand offset, Operand value) {
    ctx.emit.V_BUFFER_STORE_SHORT(value, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}
void EmitWriteSharedU32(EmitContext& ctx, IR::Inst& inst, Operand offset, Operand value) {
    ctx.emit.V_BUFFER_STORE_DWORD(value, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}
void EmitWriteSharedU64(EmitContext& ctx, IR::Inst& inst, Operand offset, Operand value) {
    ctx.emit.V_BUFFER_STORE_DWORDX2(value, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}
void EmitWriteSharedU128(EmitContext& ctx, IR::Inst& inst, Operand offset, Operand value) {
    ctx.emit.V_BUFFER_STORE_DWORDX4(value, offset, ctx.shmem_harp, scarygnm::imm32(0), 0, scarygnm::BUF_OFFEN);
}

// Vector utility
void EmitCompositeConstructU32x2(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeConstructU32x3(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b, Operand c) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeConstructU32x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractU32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractU32x3(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractU32x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertU32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertU32x3(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertU32x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeConstructF16x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeConstructF16x3(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeConstructF16x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractF16x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractF16x3(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractF16x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertF16x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertF16x3(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertF16x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeConstructF32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeConstructF32x3(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeConstructF32x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractF32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractF32x3(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractF32x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertF32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertF32x3(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertF32x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeConstructF64x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeConstructF64x3(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeConstructF64x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractF64x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractF64x3(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeExtractF64x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertF64x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertF64x3(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitCompositeInsertF64x4(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

// Select operations
void EmitSelectU1(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSelectU8(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSelectU16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSelectU32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSelectU64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSelectF16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSelectF32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSelectF64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

// Bitwise conversions
void EmitBitCastU16F16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBitCastU32F32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBitCastU64F64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBitCastF16U16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBitCastF32U32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBitCastF64U64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitPackUint2x32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitUnpackUint2x32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitPackFloat2x16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitUnpackFloat2x16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitPackHalf2x16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitUnpackHalf2x16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitPackDouble2x32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitUnpackDouble2x32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}

// Pseudo-operation, handled specially at final emit
void EmitGetZeroFromOp(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitGetSignFromOp(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitGetCarryFromOp(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitGetOverflowFromOp(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitGetSparseFromOp(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitGetInBoundsFromOp(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}

// Floating-point operations
void EmitFPAbs16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPAbs32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPAbs64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPAdd16(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPAdd32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_ADD_F32(ret, a, b);
}
void EmitFPAdd64(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_ADD_F64(ret, a, b);
}
void EmitFPFma16(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPFma32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_FMA_F32(ret, a, b);
}
void EmitFPFma64(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_FMA_F64(ret, a, b);
}
void EmitFPMax32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MAX_F32(ret, a, b);
}
void EmitFPMax64(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MAX_F64(ret, a, b);
}
void EmitFPMin32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MIN_F32(ret, a, b);
}
void EmitFPMin64(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MIN_F64(ret, a, b);
}
void EmitFPMul16(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPMul32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MUL_F32(ret, a, b);
}
void EmitFPMul64(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MUL_F64(ret, a, b);
}
void EmitFPNeg16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPNeg32(EmitContext& ctx, IR::Inst& inst, Operand a) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_SUB_F32(ret, ctx.sgpr_zero, a);
}
void EmitFPNeg64(EmitContext& ctx, IR::Inst& inst, Operand a) {
    throw NotImplementedException("GNM insn");
}
void EmitFPRecip32(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_RCP_F32(ret, src);
}
void EmitFPRecip64(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_RCP_F64(ret, src);
}
void EmitFPRecipSqrt32(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_RSQ_F32(ret, src);
}
void EmitFPRecipSqrt64(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_RSQ_F64(ret, src);
}
void EmitFPSqrt(EmitContext& ctx, IR::Inst& inst, Operand a) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_SQRT_F32(ret, a);
}
void EmitFPSin(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_SIN_F32(ret, src);
}
void EmitFPExp2(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_EXP_F32(ret, src);
}
void EmitFPCos(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_COS_F32(ret, src);
}
void EmitFPLog2(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_LOG_F32(ret, src);
}
void EmitFPSaturate16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPSaturate32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPSaturate64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPClamp16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand min, Operand max) {
    throw NotImplementedException("GNM insn");
}
void EmitFPClamp32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand min, Operand max) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MIN_F32(ret, src, max);
    ctx.emit.V_MAX_F32(ret, src, min);
}
void EmitFPClamp64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand min, Operand max) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MIN_F64(ret, src, max);
    ctx.emit.V_MAX_F64(ret, src, min);
}
void EmitFPRoundEven16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPRoundEven32(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_RNDNE_F32(ret, src);
}
void EmitFPRoundEven64(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_RNDNE_F64(ret, src);
}
void EmitFPFloor16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPFloor32(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_FLOOR_F32(ret, src);
}
void EmitFPFloor64(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_FLOOR_F64(ret, src);
}
void EmitFPCeil16(EmitContext& ctx, IR::Inst& inst, Operand src) {
    throw NotImplementedException("GNM insn");
}
void EmitFPCeil32(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CEIL_F32(ret, src);
}
void EmitFPCeil64(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CEIL_F64(ret, src);
}
void EmitFPTrunc16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPTrunc32(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_TRUNC_F32(ret, src);
}
void EmitFPTrunc64(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_TRUNC_F64(ret, src);
}

void EmitFPOrdEqual16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdEqual32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdEqual64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordEqual16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordEqual32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordEqual64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdNotEqual16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdNotEqual32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdNotEqual64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordNotEqual16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordNotEqual32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordNotEqual64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdLessThan16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdLessThan32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdLessThan64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordLessThan16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordLessThan32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordLessThan64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdGreaterThan16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdGreaterThan32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdGreaterThan64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordGreaterThan16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordGreaterThan32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordGreaterThan64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdLessThanEqual16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdLessThanEqual32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdLessThanEqual64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordLessThanEqual16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordLessThanEqual32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordLessThanEqual64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdGreaterThanEqual16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdGreaterThanEqual32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPOrdGreaterThanEqual64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordGreaterThanEqual16(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordGreaterThanEqual32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPUnordGreaterThanEqual64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitFPIsNan16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPIsNan32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFPIsNan64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}

// Integer operations
void EmitIAdd32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_ADD_I32(ret, a, b);
}
void EmitIAdd64(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitISub32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_SUB_I32(ret, a, b);
}
void EmitISub64(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitIMul32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSDiv32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitUDiv32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitINeg32(EmitContext& ctx, IR::Inst& inst, Operand a) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_NOT_B32(ret, a);
}
void EmitINeg64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitIAbs32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitIAbs64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitShiftLeftLogical32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_LSHL_B32(ret, a, b);
}
void EmitShiftLeftLogical64(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_LSHL_B64(ret, a, b);
}
void EmitShiftRightLogical32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_LSHR_B32(ret, a, b);
}
void EmitShiftRightLogical64(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_LSHR_B64(ret, a, b);
}
void EmitShiftRightArithmetic32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_ASHR_I32(ret, a, b);
}
void EmitShiftRightArithmetic64(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_ASHR_I64(ret, a, b);
}
void EmitBitwiseAnd32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_AND_B32(ret, a, b);
}
void EmitBitwiseOr32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_OR_B32(ret, a, b);
}
void EmitBitwiseXor32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_XOR_B32(ret, a, b);
}
void EmitBitFieldInsert(EmitContext& ctx, IR::Inst& inst, Operand src, Operand offset, Operand bits) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_BFI_B32(ret, src, offset, bits);
}
void EmitBitFieldSExtract(EmitContext& ctx, IR::Inst& inst, Operand src, Operand offset, Operand bits) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_BFE_I32(ret, src, offset, bits);
}
void EmitBitFieldUExtract(EmitContext& ctx, IR::Inst& inst, Operand src, Operand offset, Operand bits) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_BFE_U32(ret, src, offset, bits);
}
void EmitBitReverse32(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_BFREV_B32(ret, src);
}
void EmitBitCount32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBitwiseNot32(EmitContext& ctx, IR::Inst& inst, Operand src) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_NOT_B32(ret, src);
}

void EmitFindSMsb32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFindUMsb32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitSMin32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MIN_I32(ret, a, b);
}
void EmitUMin32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MIN_U32(ret, a, b);
}
void EmitSMax32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MAX_I32(ret, a, b);
}
void EmitUMax32(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MAX_U32(ret, a, b);
}
void EmitSClamp32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand min, Operand max) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MAX_I32(ret, src, min);
    ctx.emit.V_MIN_I32(ret, ret, max);
}
void EmitUClamp32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand min, Operand max) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_MAX_U32(ret, src, min);
    ctx.emit.V_MIN_U32(ret, ret, max);
}
void EmitSLessThan(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CMPX_LT_I32(ret, a, b);
}
void EmitULessThan(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CMPX_LT_U32(ret, a, b);
}
void EmitIEqual(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CMPX_EQ_I32(ret, a, b);
}
void EmitSLessThanEqual(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CMPX_GT_I32(ret, b, a); //invert
}
void EmitULessThanEqual(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CMPX_GT_U32(ret, b, a); //invert
}
void EmitSGreaterThan(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CMPX_GT_I32(ret, a, b);
}
void EmitUGreaterThan(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CMPX_GT_U32(ret, b, a); //invert
}
void EmitINotEqual(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CMPX_EQ_U32(ret, a, b);
    ctx.emit.V_NOT_B32(ret, ret);
}
void EmitSGreaterThanEqual(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CMPX_LT_I32(ret, b, a); //invert
}
void EmitUGreaterThanEqual(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_CMPX_LT_U32(ret, b, a); //invert
}

// Atomic operations
void EmitSharedAtomicIAdd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicSMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicUMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicSMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicUMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicInc32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicDec32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicAnd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicOr32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicXor32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicExchange32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicExchange64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitSharedAtomicExchange32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

void EmitGlobalAtomicIAdd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicSMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicUMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicSMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicUMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicInc32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicDec32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicAnd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicOr32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicXor32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicExchange32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicIAdd64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicSMin64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicUMin64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicSMax64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicUMax64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicAnd64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicOr64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicXor64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicExchange64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicIAdd32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicSMin32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicUMin32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicSMax32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicUMax32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicAnd32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicOr32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicXor32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicExchange32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicAddF32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicAddF16x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicAddF32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicMinF16x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicMinF32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicMaxF16x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitGlobalAtomicMaxF32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

void EmitStorageAtomicIAdd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicSMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicUMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicSMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicUMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicInc32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicDec32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicAnd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicOr32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicXor32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicExchange32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicIAdd64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicSMin64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicUMin64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicSMax64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicUMax64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicAnd64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicOr64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicXor64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicExchange64(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicIAdd32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicSMin32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicUMin32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicSMax32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicUMax32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicAnd32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicOr32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicXor32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicExchange32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicAddF32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicAddF16x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicAddF32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicMinF16x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicMinF32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicMaxF16x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitStorageAtomicMaxF32x2(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

// Logical operations
void EmitLogicalOr(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_OR_B32(ret, a, b);
}
void EmitLogicalAnd(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_AND_B32(ret, a, b);
}
void EmitLogicalXor(EmitContext& ctx, IR::Inst& inst, Operand a, Operand b) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_XOR_B32(ret, a, b);
}
void EmitLogicalNot(EmitContext& ctx, IR::Inst& inst, Operand a) {
    auto const ret = ctx.DefineValue(inst);
    ctx.emit.V_NOT_B32(ret, a);
}

// Conversion operations
void EmitConvertS16F16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertS16F32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertS16F64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertS32F16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertS32F32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertS32F64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertS64F16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertS64F32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertS64F64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU16F16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU16F32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU16F64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU32F16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU32F32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU32F64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU64F16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU64F32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU64F64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU64U32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU32U64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF16F32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF32F16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF32F64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF64F32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF16S8(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF16S16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF16S32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF16S64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF16U8(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF16U16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF16U32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF16U64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF32S8(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF32S16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF32S32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF32S64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF32U8(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF32U16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF32U32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF32U64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF64S8(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF64S16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF64S32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF64S64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF64U8(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF64U16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF64U32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertF64U64(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU16U32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU32U16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU8U32(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertU32U8(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertS32S8(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitConvertS32S16(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}

// Image operations
void EmitBindlessImageSampleImplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageSampleExplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageSampleDrefImplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageSampleDrefExplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageGather(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageGatherDref(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageFetch(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageQueryDimensions(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageQueryLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageGradient(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageRead(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageWrite(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

void EmitBoundImageSampleImplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageSampleExplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageSampleDrefImplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageSampleDrefExplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageGather(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageGatherDref(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageFetch(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageQueryDimensions(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageQueryLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageGradient(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageRead(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageWrite(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

void EmitImageSampleImplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitImageSampleExplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitImageSampleDrefImplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageSampleDrefExplicitLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageGather(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitImageGatherDref(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageFetch(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageQueryDimensions(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageQueryLod(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageGradient(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageRead(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageWrite(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

void EmitIsTextureScaled(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitIsImageScaled(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}

// Atomic Image operations

void EmitBindlessImageAtomicIAdd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageAtomicSMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageAtomicUMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageAtomicSMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageAtomicUMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageAtomicInc32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageAtomicDec32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageAtomicAnd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageAtomicOr32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageAtomicXor32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBindlessImageAtomicExchange32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

void EmitBoundImageAtomicIAdd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageAtomicSMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageAtomicUMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageAtomicSMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageAtomicUMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageAtomicInc32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageAtomicDec32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageAtomicAnd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageAtomicOr32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageAtomicXor32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitBoundImageAtomicExchange32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

void EmitImageAtomicIAdd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageAtomicSMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageAtomicUMin32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageAtomicSMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageAtomicUMax32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageAtomicInc32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageAtomicDec32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageAtomicAnd32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageAtomicOr32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageAtomicXor32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitImageAtomicExchange32(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}

// Warp operations
void EmitLaneId(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitVoteAll(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitVoteAny(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitVoteEqual(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitSubgroupBallot(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitSubgroupEqMask(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitSubgroupLtMask(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitSubgroupLeMask(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitSubgroupGtMask(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitSubgroupGeMask(EmitContext& ctx, IR::Inst& inst) {
    throw NotImplementedException("GNM insn");
}
void EmitShuffleIndex(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitShuffleUp(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitShuffleDown(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitShuffleButterfly(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitFSwizzleAdd(EmitContext& ctx, IR::Inst& inst, Operand src, Operand a, Operand b) {
    throw NotImplementedException("GNM insn");
}
void EmitDPdxFine(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitDPdyFine(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitDPdxCoarse(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
void EmitDPdyCoarse(EmitContext& ctx, IR::Inst& inst, Operand) {
    throw NotImplementedException("GNM insn");
}
}

