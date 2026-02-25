#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
enum inst_kind {
    VOPC,
    VOP1,
    VOP2,
    VOP3,
    SMRD,
    MUBUF,
    MTBUF,
    VINTRP,
    NONE
};
enum inst_kind get_inst_kind(const char *name) {
    if (strncmp(name, "VOPC_", 5) == 0) return VOPC;
    if (strncmp(name, "VOP1_", 5) == 0) return VOP1;
    if (strncmp(name, "VOP2_", 5) == 0) return VOP2;
    if (strncmp(name, "VOP3_", 5) == 0) return VOP3;
    if (strncmp(name, "SMRD_", 5) == 0) return SMRD;
    if (strncmp(name, "MUBUF", 5) == 0) return MUBUF;
    if (strncmp(name, "MTBUF", 5) == 0) return MTBUF;
    if (strncmp(name, "VINTR", 5) == 0) return VINTRP;
    return NONE;
}

int main(int argc, char *argv[]) {
    FILE* fp = fopen(argv[1], "rt");
    char linebuf[512];
    while(fgets(linebuf, sizeof linebuf, fp)) {
        char* p;
        if ((p = strchr(linebuf, '\n')) != NULL)
            *p = '\0';
        if (strlen(linebuf) == 0 || linebuf[0] == '/')
            continue;

        char* name = linebuf;
        char* value = strrchr(linebuf, ' ');
        p = strchr(linebuf, ' ');
        if (value == NULL || p == NULL)
            continue;

        *value++ = '\0';
        *p = '\0';

        enum inst_kind kind = get_inst_kind(name);
        name = strchr(name, '_') + 1;

        char* suffix = strrchr(name, '_');

        //VOP3c just copies from VOPC
        //VOP3a just adds 256 to VOP2
        //VOP3 VOP1+384
        switch (kind) {
        case VOP1:
            printf(
                "void V_%s(VGPR vdst, unsigned src0) { VOP1_OP(384+%s, vdst, src0); }\n"
                , name, value);
            printf(
                "void V_%s(VGPR vdst, unsigned src0, unsigned src1, unsigned src2) { VOP3A_OP(384+%s, vdst, src0, src1, src2); }\n"
                , name, value);
            break;
        case VOP2:
            printf(
                "void V_%s(VGPR vdst, unsigned src0, VGPR vsrc1) { VOP2_OP(%s, vdst, src0, vsrc1); }\n"
                , name, value);
            printf(
                "void V_%s(VGPR vdst, unsigned src0, unsigned src1, unsigned src2) { VOP3A_OP(256+%s, vdst, src0, src1, src2); }\n"
                , name, value);
            break;
        case VOP3:
            printf(
                "void V_%s(VGPR vdst, unsigned src0, unsigned src1, unsigned src2) { VOP3A_OP(%s, vdst, src0, src1, src2); }\n"
                , name, value);
            printf(
                "void V_%s(VGPR vdst, unsigned sdst, unsigned src0, unsigned src1, unsigned src2) { VOP3B_OP(%s, vdst, sdst, src0, src1, src2); }\n"
                , name, value);
            printf(
                "void V_%s(unsigned sdst, unsigned src0, unsigned src1) { VOP3C_OP(%s, sdst, src0, src1); }\n"
                , name, value);
            break;
        case VOPC:
            printf(
                "void V_%s(unsigned sdst, unsigned src0, unsigned src1) { VOP3C_OP(%s, sdst, src0, src1); }\n"
                , name, value);
            break;
        case SMRD:
            if (strcmp(name, "DCACHE_INV") == 0
            || strcmp(name, "DCACHE_INV_VOL") == 0) {
                // no params
                printf("void S_%s() { SMRD_OP(%s, {}, {}, 0); }\n", name, value);
            } else {
                printf("void S_%s(SGPR_Indexed src0, SGPR_Indexed src1, unsigned imm_offset) { SMRD_OP(%s, src0, src1, imm_offset); }\n", name, value);
            }
            break;
        case MUBUF:
            printf(
                "void %s(VGPR vdata, VGPR vaddr, unsigned svsharp, unsigned imm_offset, int32_t soffset, BufFlags flags) { MUBUF_OP(%s, vdata, vaddr, svsharp, imm_offset, soffset, flags); }\n"
                , name, value);
            printf(
                "void %s(VGPR_Indexed vdata, VGPR vaddr, SGPR_Indexed svsharp, unsigned imm_offset, int32_t soffset, BufFlags flags) { MUBUF_OP(%s, vdata, vaddr, svsharp, imm_offset, soffset, flags); }\n"
                , name, value);
            break;
        case VINTRP:
            printf(
                "void V_%s(VGPR vdst, VGPR vsrc, AttributeElement a) { VINTRP_OP(%s, vdst, vsrc, a); }\n"
                , name, value);
            break;
        case NONE:
            break;
        }
    }
    fclose(fp);
}
