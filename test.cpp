#include <cassert>
#include "scarygnm.hpp"

void assert_eq(uint32_t a, uint32_t b) {
    if (a != b) {
        std::printf("%x != %x\n", a, b);
        std::abort();
    }
}

/*
    ScalarGPRMin = 0,
    ScalarGPRMax = 103,
    SignedConstIntPosMin = 129,
    SignedConstIntPosMax = 192,
    SignedConstIntNegMin = 193,
    SignedConstIntNegMax = 208,
    ConstFloatMin = 240,
    VectorGPRMin = 256,
    VectorGPRMax = 511
*/

void TEST1() {
    printf("codegen test 1\n");
    {
        //vdst = smask[thread_id:] ? vsrc1 : vsrc0
        //V_CNDMASK_B32 v4, v0, v1,       EXEC
        //V_CNDMASK_B32 v5, v0, abs(v2),  EXEC
        //V_CNDMASK_B32 v6, v0, -v2,      EXEC
        //V_CNDMASK_B32 v7, v0, -abs(v3), EXEC
        scarygnm::Context c;
        c.V_CNDMASK_B32(c.v4, c.v0, c.v1);
        c.V_CNDMASK_B32(c.v5, c.v0, c.abs(c.v2));
        c.V_CNDMASK_B32(c.v6, c.v0, -c.v2);
        c.V_CNDMASK_B32(c.v7, c.v0, -c.abs(c.v3));
    }
    // {
    //     //V_MAD_U64_U32 v[4:5], v0, v0, v[2:3]
    //     //V_MAD_U64_U32 v[6:7], v0, v1, 0
    //     scarygnm::Context c;
    //     c.V_MAD_U64_U32(c.v[4,5], c.v0, c.v0, c.v[2,3]);
    //     c.V_MAD_U64_U32(c.v[6,7], c.v0, c.v1, 0);
    // }
    {
        // 0x5E040503, //V_CVT_PKRTZ_F16_F32 v2, v3, v2
        // 0x5E000103, //V_CVT_PKRTZ_F16_F32 v0, v3, v0
        scarygnm::Context c;
        c.V_CVT_PKRTZ_F16_F32(c.v2, c.v3, c.v2);
        c.V_CVT_PKRTZ_F16_F32(c.v0, c.v3, c.v0);
        assert_eq(c.data[0], 0x5E040503);
        assert_eq(c.data[1], 0x5E000103);
    }
    {
        // 0xBEFC0300, //S_MOV_B32 M0, s0
        scarygnm::Context c;
        c.S_MOV_B32(-4, 0);
        assert_eq(c.data[0], 0xBEFC0300);
    }
    {
        // 0xBEFC0300, //S_MOV_B32 M0, s0
        // 0xC80C0000, //V_INTERP_P1_F32 v3, v0, attr0.x
        // 0xC80D0001, //V_INTERP_P2_F32 v3, v1, attr0.x
        // 0xC8080100, //V_INTERP_P1_F32 v2, v0, attr0.y
        // 0xC8090101, //V_INTERP_P2_F32 v2, v1, attr0.y
        // 0x5E040503, //V_CVT_PKRTZ_F16_F32 v2, v3, v2
        // 0xC80C0200, //V_INTERP_P1_F32 v3, v0, attr0.z
        // 0xC8000300, //V_INTERP_P1_F32 v0, v0, attr0.w
        // 0xC80D0201, //V_INTERP_P2_F32 v3, v1, attr0.z
        // 0xC8010301, //V_INTERP_P2_F32 v0, v1, attr0.w
        // 0x5E000103, //V_CVT_PKRTZ_F16_F32 v0, v3, v0
        // 0xF8001C0F, //EXP mrt0, v2, v0 compr vm done
        // 0x00000002,
        scarygnm::Context c;
        c.S_MOV_B32(0, 0);
        c.V_INTERP_P1_F32(c.v3, c.v0, c.attr0.x);
        c.V_INTERP_P1_F32(c.v3, c.v1, c.attr0.x);
        c.V_INTERP_P1_F32(c.v2, c.v0, c.attr0.y);
        c.V_INTERP_P1_F32(c.v2, c.v1, c.attr0.y);
        c.V_CVT_PKRTZ_F16_F32(c.v2, c.v3, c.v2);
        c.V_INTERP_P1_F32(c.v3, c.v0, c.attr0.z);
        c.V_INTERP_P1_F32(c.v0, c.v0, c.attr0.w);
        c.V_INTERP_P1_F32(c.v3, c.v1, c.attr0.z);
        c.V_INTERP_P1_F32(c.v0, c.v1, c.attr0.w);
        c.V_CVT_PKRTZ_F16_F32(c.v0, c.v3, c.v0);
        assert_eq(c.data[0], 0x5E040503);
        assert_eq(c.data[1], 0x5E000103);
    }
    {
        scarygnm::Context c;
        c.S_NOP();
        c.S_ENDPGM();
        assert_eq(c.data[1], 0xBF810000);
    }
    {
        scarygnm::Context c;
        c.V_MUL_LEGACY_F32(c.v4, c.v0, c.v1);
        c.V_MUL_F32(c.v5, c.v0, c.v1);
        c.V_MIN_LEGACY_F32(c.v4, c.v0, c.v1);
        c.V_MIN_F32(c.v5, c.v0, c.v1);
    }
    {
        //0x5E000300, //V_CVT_PKRTZ_F16_F32 v0, v0, v1
        //0x5E040702, //V_CVT_PKRTZ_F16_F32 v2, v2, v3
        scarygnm::Context c;
        c.V_CVT_PKRTZ_F16_F32(c.v0, c.v0, c.v1);
        c.V_CVT_PKRTZ_F16_F32(c.v2, c.v2, c.v3);
        assert_eq(c.data[0], 0x5E000300);
        assert_eq(c.data[1], 0x5E040702);
    }
    {
        scarygnm::Context c;
        c.S_LOAD_DWORDX4(c.s[4,7], c.s[2,3], 0x00);
        assert_eq(c.data[0], 0xC0820300);
    }
    {
        scarygnm::Context c;
        c.S_WAITCNT(c.lgkmcnt(0));
        assert_eq(c.data[0], 0xBF8C007F);
    }
    {
        scarygnm::Context c;
        c.BUFFER_LOAD_FORMAT_XYZ(c.v[4,7], c.v0, c.s[4,7], 0, 0, c.BUF_IDXEN);
        assert_eq(c.data[0], 0xE00C2000);
        assert_eq(c.data[1], 0x80010400);
    }
}

int main() {
    TEST1();

    scarygnm::Context c;
    // TODO S_LOAD_DWORDX4
    c.S_WAITCNT(c.lgkmcnt(0), c.expcnt(0), c.vmcnt(0));
    c.BUFFER_LOAD_FORMAT_XYZ(c.v[4,7], c.v0, c.s[4,7], 0, 0, c.BUF_IDXEN);
    // TODO S_LOAD_DWORDX4
    c.S_WAITCNT(c.lgkmcnt(0), c.expcnt(0), c.vmcnt(0));
    c.BUFFER_LOAD_FORMAT_XYZ(c.v[8,11], c.v0, c.s[4,7], 0, 0, c.BUF_IDXEN);
    c.S_WAITCNT(c.lgkmcnt(0), c.expcnt(0), c.vmcnt(0));
    // TODO S_SETPC_B64
    c.S_ENDPGM();
    /*
    0xC0820300, //S_LOAD_DWORDX4 s[4:7], s[2:3], 0x00
    0xBF8C007F, //S_WAITCNT lgkmcnt(0)
    0xE00C2000, //BUFFER_LOAD_FORMAT_XYZW v[4:7], v0, s[4:7], 0, [0] IDXEN
    0x80010400,
    0xC0820304, //S_LOAD_DWORDX4 s[4:7], s[2:3], 0x04
    0xBF8C007F, //S_WAITCNT lgkmcnt(0)
    0xE00C2000, //BUFFER_LOAD_FORMAT_XYZW v[8:11], v0, s[4:7], 0, [0] IDXEN
    0x80010800,
    0xBF8C0000, //S_WAITCNT lgkmcnt(0) expcnt(0) vmcnt(0)
    0xBE802000  //S_SETPC_B64 s[0:1]
    */
}
