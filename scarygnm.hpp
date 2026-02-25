#pragma once

#include <cstdint>
#include <cassert>
#include <cstdint>
#include <vector>

namespace scarygnm {

#define H_SOP1   0b101111101 //9
#define H_SOPC   0b101111110 //9
#define H_SOPP   0b101111111 //9
#define H_VOP1   0b0111111   //7
#define H_VOPC   0b0111110   //7
#define H_VOP3   0b110100    //6
#define H_DS     0b110110    //6
#define H_MUBUF  0b111000    //6
#define H_MTBUF  0b111010    //6
#define H_EXP    0b111110    //6
#define H_VINTRP 0b110010    //6
#define H_MIMG   0b111100    //6
#define H_SMRD   0b11000     //5
#define H_SOPK   0b1011      //4
#define H_SOP2   0b10        //2
#define H_VOP2   0b0         //1 

    struct Context {
        struct TVOP3 {
            uint64_t : 8;
            uint64_t abs : 3;
            uint64_t clmp : 1;
            uint64_t : 47;
            uint64_t omod : 2;
            uint64_t neg : 3;
        };

        struct TSMRD {
            uint32_t offset : 8;
            uint32_t imm : 1;
            uint32_t count : 5;
            uint32_t : 18;
        };

        struct TMUBUF {
            uint64_t offset : 12;
            uint64_t offen : 1;
            uint64_t idxen : 1;
            uint64_t glc : 1;
            uint64_t : 1;
            uint64_t lds : 1;
            uint64_t : 37;
            uint64_t slc : 1;
            uint64_t tfe : 1;
            uint64_t count : 3;
            uint64_t size : 5;
        };

        struct TMIMG {
            uint64_t : 8;
            uint64_t dmask : 4;
            uint64_t unrm : 1;
            uint64_t glc : 1;
            uint64_t da : 1;
            uint64_t r128 : 1;
            uint64_t tfe : 1;
            uint64_t lwe : 1;
            uint64_t : 7;
            uint64_t slc : 1;
            uint64_t mod : 32;
            uint64_t : 6;
        };

        struct TDS {
            uint64_t offset0 : 8;
            uint64_t offset1 : 8;
            uint64_t : 1;
            uint64_t gds : 1;
            uint64_t dual : 1;
            uint64_t sign : 1;
            uint64_t relative : 1;
            uint64_t stride : 1;
            uint64_t size : 4;
            uint64_t : 38;
        };

        struct TEXP {
            uint64_t EN : 4;
            uint64_t TARGET : 6;
            uint64_t COMPR : 1;
            uint64_t DONE : 1;
            uint64_t VM : 1;
            uint64_t RESERVED : 51;
        };

        // from shader builder
        struct TSOP1 {
            uint32_t SSRC : 8;
            uint32_t OP : 8;
            uint32_t SDST : 7;
            uint32_t ENCODE : 9;
        };

        struct TSOPP {
            uint32_t SIMM : 16;
            uint32_t OP : 7;
            uint32_t ENCODE : 9;
        };

        struct TVOP1 {
            uint32_t SRC0 : 9;
            uint32_t OP : 8;
            uint32_t VDST : 8;
            uint32_t ENCODE : 7;
        };

        enum TB_DataFormat : uint32_t {
            INVALID,
            _8,
            _16,
            _8_8,
            _32,
            _16_16,
            _10_11_11,
            _10_10_10_2,
            _2_10_10_10,
            _8_8_8_8,
            _32_32,
            _16_16_16_16,
            _32_32_32,
            _32_32_32_32
        };
        enum TB_NumberFormat : uint32_t {
            UNORM,
            SNORM,
            USCALED,
            SSCALED,
            UINT,
            SINT,
            SNORM_OGL,
            FLOAT,
        };


        struct TMTBUF {
            uint32_t OFFSET : 12;
            uint32_t OFFEN : 1;
            uint32_t IDXEN : 1;
            uint32_t GLC : 1;
            uint32_t reserved1 : 1;
            uint32_t OP : 3;
            uint32_t DFMT : 4;
            uint32_t NFMT : 3;
            uint32_t ENCODE : 6;

            uint32_t VADDR : 8;
            uint32_t VDATA : 8;
            uint32_t SRSRC : 5;
            uint32_t reserved4 : 1;
            uint32_t SLC : 1;
            uint32_t TFE : 1;
            // uint64_t count : 3;
            // uint64_t size : 5;
            uint32_t SOFFSET : 8;
        };

        enum Vop3Flags : uint32_t {
            //input (per src)
            V3F_ABS = uint32_t(1 << 31),
            V3F_NEG = 1 << 30,
            //output (dst only)
            V3F_CLAMP = uint32_t(1 << 31),
            V3F_MUL2 = 1 << 29,
            V3F_MUL4 = 2 << 29,
            V3F_DIV2 = 3 << 29
        };
        enum BufFlags : uint32_t {
            BUF_NONE = 0,
            BUF_OFFEN = 1 << 0,
            BUF_IDXEN = 1 << 1,
            BUF_GLC = 1 << 2,
            BUF_LDS = 1 << 3,
            BUF_SLC = 1 << 4,
            BUF_TFE = 1 << 5
        };
        struct VGPR {
            constexpr VGPR(unsigned value) noexcept : value{value} {}
            constexpr VGPR abs() const { return VGPR{ value | V3F_ABS }; }
            constexpr VGPR clamp() const { return VGPR{ value | V3F_CLAMP }; }
            constexpr VGPR mul2() const { return VGPR{ value | V3F_MUL2 }; }
            constexpr VGPR mul4() const { return VGPR{ value | V3F_MUL4 }; }
            constexpr VGPR div2() const { return VGPR{ value | V3F_DIV2 }; }
            constexpr VGPR operator -() const { return VGPR{ value | V3F_ABS }; }
            constexpr operator unsigned() const { return value; }
            unsigned value;
        };
        static constexpr uint32_t imm32(uint32_t value) {
            return value;
        }
        static constexpr int32_t simm32(int32_t value) {
            if ((value >= 0) && (value <= 64))
                return value + 128;
            if ((value >= -16) && (value <= -1))
                return (-value) + 192;
            return 128;
        }
        static constexpr VGPR abs(VGPR o) noexcept { return o.abs(); }
        static constexpr VGPR clamp(VGPR o) noexcept { return o.clamp(); }
        static constexpr VGPR mul2(VGPR o) noexcept { return o.mul2(); }
        static constexpr VGPR mul4(VGPR o) noexcept { return o.mul4(); }
        static constexpr VGPR div2(VGPR o) noexcept { return o.div2(); }
        static constexpr unsigned vmcnt(unsigned o) noexcept { return o; }
        static constexpr unsigned expcnt(unsigned o) noexcept { return o; }
        static constexpr unsigned lgkmcnt(unsigned o) noexcept { return o; }

        struct VGPR_Indexed { size_t start, end; };
        struct VGPR_IndexedFactory {
            constexpr VGPR_Indexed operator[](size_t a, size_t b) const { return VGPR_Indexed{a+VGPR_BASE, b+VGPR_BASE}; }
        };
        struct SGPR_Indexed { size_t start, end; };
        struct SGPR_IndexedFactory {
            constexpr SGPR_Indexed operator[](size_t a, size_t b) const { return SGPR_Indexed{a+SGPR_BASE, b+SGPR_BASE}; }
        };
        static constexpr VGPR_IndexedFactory v;
        static constexpr SGPR_IndexedFactory s;

        struct AttributeElement { size_t attr, chan; };
        struct AttributeElementFactory {
            AttributeElement x;
            AttributeElement y;
            AttributeElement z;
            AttributeElement w;
        };
#define MAKE_ATTR(N) { {N, 0}, {N, 1}, {N, 2}, {N, 3} }
        static constexpr AttributeElementFactory attr0 = MAKE_ATTR(0);
        static constexpr AttributeElementFactory attr1 = MAKE_ATTR(1);
        static constexpr AttributeElementFactory attr2 = MAKE_ATTR(2);
        static constexpr AttributeElementFactory attr3 = MAKE_ATTR(3);
        static constexpr AttributeElementFactory attr4 = MAKE_ATTR(4);
        static constexpr AttributeElementFactory attr5 = MAKE_ATTR(5);
        static constexpr AttributeElementFactory attr6 = MAKE_ATTR(6);
        static constexpr AttributeElementFactory attr7 = MAKE_ATTR(7);
        // TODO: how the fuck are these mfers encoded?
#undef MAKE_ATTR

        std::vector<uint32_t> data;
        void VOP1_OP(unsigned op, VGPR vdst, unsigned src0) {
            assert(false); //unk format
        }
        void VOP3A_OP(unsigned op, VGPR vdst, unsigned src0, unsigned src1, unsigned src2) {
            assert(vdst.value >= VGPR_BASE);
            struct TVOP3a {
                uint32_t VDST : 8;
                uint32_t ABS : 3;
                uint32_t CLAMP : 1;
                uint32_t reserved : 5;
                uint32_t OP : 9;
                uint32_t ENCODE : 6;

                uint32_t SRC0 : 9;
                uint32_t SRC1 : 9;
                uint32_t SRC2 : 9;
                uint32_t OMOD : 2;
                uint32_t NEG : 3;
            } cmd = {};
            cmd.ENCODE = H_VOP3;
            cmd.reserved = 0;
            cmd.OP = op;
            cmd.VDST = (vdst.value & 0x1FF) - VGPR_BASE;
            cmd.SRC0 = src0;
            cmd.SRC1 = src1;
            cmd.SRC2 = src2;
            cmd.ABS = (((src0 & V3F_ABS) != 0) << 0) | (((src1 & V3F_ABS) != 0) << 1) | (((src2 & V3F_ABS) != 0) << 2);
            cmd.CLAMP = (vdst.value & V3F_CLAMP) != 0;
            cmd.OMOD = vdst.value >> 29;
            cmd.NEG = (((src0 & V3F_NEG) != 0) << 0) | (((src1 & V3F_NEG) != 0) << 1) | (((src2 & V3F_NEG) != 0) << 2);
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[1]);
        }
        void VOP3B_OP(unsigned op, VGPR vdst, unsigned sdst, unsigned src0, unsigned src1, unsigned src2) {
            assert(vdst.value >= VGPR_BASE);
            struct TVOP3b {
                uint32_t VDST : 8;
                uint32_t SDST : 7;
                uint32_t reserved : 2;
                uint32_t OP : 9;
                uint32_t ENCODE : 6;

                uint32_t SRC0 : 9;
                uint32_t SRC1 : 9;
                uint32_t SRC2 : 9;
                uint32_t OMOD : 2;
                uint32_t NEG : 3;
            } cmd = {};
            cmd.ENCODE = H_VOP3;
            cmd.reserved = 0;
            cmd.OP = op;
            cmd.VDST = (vdst.value & 0x1FF) - VGPR_BASE;
            cmd.SDST = sdst;
            cmd.SRC0 = src0;
            cmd.SRC1 = src1;
            cmd.SRC2 = src2;
            cmd.OMOD = vdst.value >> 29;
            cmd.NEG = (((src0 & V3F_NEG) != 0) << 0) | (((src1 & V3F_NEG) != 0) << 1) | (((src2 & V3F_NEG) != 0) << 2);
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[1]);
        }
        void VOP3C_OP(unsigned op, unsigned sdst, unsigned src0, unsigned src1) {
            struct TVOP3a {
                uint32_t VDST : 8;
                uint32_t ABS : 3;
                uint32_t CLAMP : 1;
                uint32_t reserved : 5;
                uint32_t OP : 9;
                uint32_t ENCODE : 6;
                uint32_t SRC0 : 9;
                uint32_t SRC1 : 9;
                uint32_t SRC2 : 9;
                uint32_t OMOD : 2;
                uint32_t NEG : 3;
            } cmd = {};
            cmd.ENCODE = H_VOP3;
            cmd.OP = op;
            cmd.VDST = sdst;
            cmd.SRC0 = src0;
            cmd.SRC1 = src1;
            cmd.SRC2 = 0;
            cmd.ABS   = (((src0 & V3F_ABS) != 0) << 0) | (((src1 & V3F_ABS) != 0) << 1);
            cmd.CLAMP = (sdst & V3F_CLAMP) != 0;
            cmd.OMOD = sdst >> 29;
            cmd.NEG  = (((src0 & V3F_NEG) != 0) << 0) | (((src1 & V3F_NEG) != 0) << 1);
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[1]);
        }
        void VOP2_OP(unsigned op, VGPR vdst, unsigned src0, VGPR vsrc1) {
            assert(vdst.value >= VGPR_BASE && vsrc1.value >= VGPR_BASE);
            struct TVOP2 {
                uint32_t SRC0 : 9;
                uint32_t VSRC1 : 8;
                uint32_t VDST : 8;
                uint32_t OP : 6;
                uint32_t ENCODE : 1;
            } cmd = {};
            cmd.ENCODE = H_VOP2;
            cmd.OP = op;
            cmd.VDST  = vdst.value - VGPR_BASE;
            cmd.SRC0  = src0;
            cmd.VSRC1 = vsrc1.value - VGPR_BASE;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void MUBUF_OP(unsigned op, VGPR vdata, VGPR vaddr, unsigned svsharp, unsigned imm_offset, int32_t soffset, BufFlags flags) {
            assert(vdata.value >= VGPR_BASE && vaddr.value >= VGPR_BASE);
            struct TMUBUF {
                uint32_t OFFSET : 12; //11:0
                uint32_t OFFEN : 1; //12
                uint32_t IDXEN : 1; //13
                uint32_t GLC : 1; //14
                uint32_t ADDR64 : 1; //15
                uint32_t LDS : 1; //16
                uint32_t : 1; //17
                uint32_t OP : 7; //[15:8]???
                uint32_t : 1; //25
                uint32_t ENCODE : 6; //[31:26]
                uint32_t VADDR : 8; //[39:32]
                uint32_t VDATA : 8; //[47:40]
                uint32_t SRSRC : 5; //[52:48]
                uint32_t : 1; //53
                uint32_t SLC : 1; //54
                uint32_t TFE : 1; //55
                uint32_t SOFFSET : 6; //[63:56]
                uint32_t I_DONT_KNOW : 2; //WHY THE FUCK IS THIS-
            } cmd = {};
            static_assert(sizeof(TMUBUF) == 8);
            cmd.ENCODE = H_MUBUF;
            cmd.OP = op;
            cmd.VDATA = (vdata.value - VGPR_BASE);
            cmd.VADDR = (vaddr.value - VGPR_BASE);
            cmd.SRSRC = (svsharp / 4);
            cmd.OFFSET = imm_offset;
            cmd.SOFFSET = soffset;
            cmd.OFFEN = (flags & BUF_OFFEN) != 0;
            cmd.IDXEN = (flags & BUF_IDXEN) != 0;
            cmd.GLC = (flags & BUF_GLC) != 0;
            cmd.LDS = (flags & BUF_LDS) != 0;
            cmd.SLC = (flags & BUF_SLC) != 0;
            cmd.TFE = (flags & BUF_TFE) != 0;
            cmd.I_DONT_KNOW = 0x2;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[1]);
        }
        void MUBUF_OP(unsigned op, VGPR_Indexed vdata, VGPR vaddr, SGPR_Indexed svsharp, unsigned imm_offset, int32_t soffset, BufFlags flags) {
            MUBUF_OP(op, vdata.start, vaddr, svsharp.start, imm_offset, soffset, flags);
        }
        void VINTRP_OP(unsigned op, VGPR vdst, VGPR vsrc, AttributeElement a) {
            struct TVINTRP {
                uint32_t VSRC : 8;
                uint32_t CHAN : 2;
                uint32_t ATTR : 6;
                uint32_t OP : 2;
                uint32_t VDST : 8;
                uint32_t ENCODE : 6;
            } cmd = {};
            static_assert(sizeof(cmd) == 4);
            cmd.ENCODE = H_VINTRP;
            cmd.OP = op;
            cmd.VDST = vdst.value - VGPR_BASE;
            cmd.VSRC = vsrc.value - VGPR_BASE;
            cmd.CHAN = a.chan;
            cmd.ATTR = a.attr;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void SMRD_OP(unsigned op, SGPR_Indexed sdst, SGPR_Indexed src1, unsigned imm_offset) {
            struct TSMRD {
                uint32_t OFFSET : 8;
                uint32_t IMM : 1;
                uint32_t SBASE : 6;
                uint32_t SDST : 7;
                uint32_t OP : 5;
                uint32_t ENCODE : 5;
            } cmd = {};
            static_assert(sizeof(cmd) == 4);
            assert((src1.start & 1) == 0); //SGBASE * 2
            cmd.ENCODE = H_SMRD;
            cmd.OP = op;
            cmd.OFFSET = imm_offset;
            cmd.SBASE = src1.start / 2;
            cmd.IMM = 1;
            cmd.SDST = sdst.start;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }

        void S_NOP() {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x00;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_ENDPGM() {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x01;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_ICACHE_INV() {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x13;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_INCPERFLEVEL(uint16_t simm) {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x14;
            cmd.SIMM = simm;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_SENDMSG(uint16_t simm) {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x10;
            cmd.SIMM = simm;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_SENDMSGHALT(uint16_t simm) {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x11;
            cmd.SIMM = simm;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_SETHALT() {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x0D;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_SETKILL() {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x0B;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_SETPRIO(uint16_t simm) {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x0F;
            cmd.SIMM = simm;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_SLEEP(uint16_t simm) {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x0E;
            cmd.SIMM = simm;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_TRAP(uint16_t simm) {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x12;
            cmd.SIMM = simm;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_TTRACEDATA() {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x16;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_WAITCNT(uint32_t lgkmcnt, uint32_t expcnt = 0x07, uint32_t vmcnt = 0x0f) {
            TSOPP cmd = {};
            cmd.ENCODE = H_SOPP;
            cmd.OP = 0x0C;
            struct Twaitcnt_simm {
                uint32_t vmcnt : 4;
                uint32_t expcnt : 3;
                uint32_t reserved1 : 1;
                uint32_t lgkmcnt : 4;
                uint32_t reserved2 : 4;
            } bits = {};
            bits.vmcnt = vmcnt;
            bits.expcnt = expcnt;
            bits.lgkmcnt = lgkmcnt;
            cmd.SIMM = *reinterpret_cast<uint16_t*>(&bits);
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_MOV_B32(unsigned sdst, unsigned ssrc) {
            TSOP1 cmd = {};
            cmd.ENCODE = H_SOP1;
            cmd.OP = 0x03;
            cmd.SDST = sdst;
            cmd.SSRC = ssrc;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void S_MOV_B64(unsigned sdst, unsigned ssrc) {
            TSOP1 cmd = {};
            cmd.ENCODE = H_SOP1;
            cmd.OP = 0x04;
            cmd.SDST = sdst;
            cmd.SSRC = ssrc;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }
        void V_READFIRSTLANE_B32(unsigned sdst, unsigned vsrc) {
            TVOP1 cmd = {};
            cmd.ENCODE = H_VOP1;
            cmd.OP = 0x02;
            cmd.VDST = sdst;
            cmd.SRC0 = vsrc;
            data.push_back(reinterpret_cast<uint32_t*>(&cmd)[0]);
        }

        static constexpr uint32_t SGPR_BASE = 0;
        static constexpr uint32_t VGPR_BASE = 256;

        static inline unsigned s0 = (SGPR_BASE+0);
        static inline unsigned s1 = (SGPR_BASE+1);
        static inline unsigned s2 = (SGPR_BASE+2);
        static inline unsigned s3 = (SGPR_BASE+3);
        static inline unsigned s4 = (SGPR_BASE+4);
        static inline unsigned s5 = (SGPR_BASE+5);
        static inline unsigned s6 = (SGPR_BASE+6);
        static inline unsigned s7 = (SGPR_BASE+7);
        static inline unsigned s8 = (SGPR_BASE+8);
        static inline unsigned s9 = (SGPR_BASE+9);
        static inline unsigned s10 = (SGPR_BASE+10);
        static inline unsigned s11 = (SGPR_BASE+11);
        static inline unsigned s12 = (SGPR_BASE+12);
        static inline unsigned s13 = (SGPR_BASE+13);
        static inline unsigned s14 = (SGPR_BASE+14);
        static inline unsigned s15 = (SGPR_BASE+15);

        // Holds the low Dword of the flat-scratch memory descriptor.
        static inline unsigned FLAT_SCR_LO = 104;
        static inline unsigned FLAT_SCR_HI = 105;
        static inline unsigned VCC_LO = 106;
        static inline unsigned VCC_HI = 107;
        static inline unsigned TBA_LO = 108;
        static inline unsigned TBA_HI = 109;
        static inline unsigned TMA_LO = 110;
        static inline unsigned TMA_HI = 111;
        // Constant 0
        static inline unsigned FCONST_0 = 128;
        static inline unsigned ICONST_0 = 128;
        // 129-192 -> int 1 to 64
        // 193-208 -> int -1 to -16
        // Can the constant be encoded in the SGPR number?
        [[nodiscard]] inline bool CanSGPR_ICONST(int c) noexcept {
            return c == 0 || (c < 0 && c >= -16) || (c > 0 && c <= 64);
        }
        // Convert a constant into a SGPR ICONST
        [[nodiscard]] inline unsigned SGPR_ICONST(int c) noexcept {
            return c < 0 ? 192 - (c + 1) : 128 + c;
        }
        // +0.5
        static inline unsigned FCONST_0_5 = 240;
        // -0.5
        static inline unsigned FCONST_M0_5 = 241;
        static inline unsigned FCONST_1_0 = 242;
        static inline unsigned FCONST_M1_0 = 243;
        static inline unsigned FCONST_2_0 = 244;
        static inline unsigned FCONST_M2_0 = 245;
        static inline unsigned FCONST_4_0 = 246;
        static inline unsigned FCONST_M4_0 = 247;

        static inline unsigned M0 = 124;
        static inline unsigned EXEC_LO = 126;
        static inline unsigned EXEC_HI = 127;
        static inline unsigned VCCZ = 251;
        static inline unsigned EXECZ = 252;
        static inline unsigned SCC = 253;

        static inline VGPR v0 = VGPR(VGPR_BASE+0);
        static inline VGPR v1 = VGPR(VGPR_BASE+1);
        static inline VGPR v2 = VGPR(VGPR_BASE+2);
        static inline VGPR v3 = VGPR(VGPR_BASE+3);
        static inline VGPR v4 = VGPR(VGPR_BASE+4);
        static inline VGPR v5 = VGPR(VGPR_BASE+5);
        static inline VGPR v6 = VGPR(VGPR_BASE+6);
        static inline VGPR v7 = VGPR(VGPR_BASE+7);
        static inline VGPR v8 = VGPR(VGPR_BASE+8);
        static inline VGPR v9 = VGPR(VGPR_BASE+9);
        static inline VGPR v10 = VGPR(VGPR_BASE+10);
        static inline VGPR v11 = VGPR(VGPR_BASE+11);
        static inline VGPR v12 = VGPR(VGPR_BASE+12);
        static inline VGPR v13 = VGPR(VGPR_BASE+13);
        static inline VGPR v14 = VGPR(VGPR_BASE+14);
        static inline VGPR v15 = VGPR(VGPR_BASE+15);
        static inline VGPR v16 = VGPR(VGPR_BASE+16);
        static inline VGPR v17 = VGPR(VGPR_BASE+17);
        static inline VGPR v18 = VGPR(VGPR_BASE+18);
        static inline VGPR v19 = VGPR(VGPR_BASE+19);
        static inline VGPR v20 = VGPR(VGPR_BASE+20);
        static inline VGPR v21 = VGPR(VGPR_BASE+21);
        static inline VGPR v22 = VGPR(VGPR_BASE+22);
        static inline VGPR v23 = VGPR(VGPR_BASE+23);

        // scary shit
#include "scarygnm_generated.hpp"

    };
}
