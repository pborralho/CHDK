#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>

#include <capstone.h>

#include "stubs_load.h"
#include "firmware_load_ng.h"

// borrowed from chdk_dasm.h, chdk_dasm.c
//------------------------------------------------------------------------------------------------------------
typedef unsigned int t_address;
typedef unsigned int t_value;

/* -----------------------------------------------------------------
 * Linked List Routines
 * ----------------------------------------------------------------- */
 
struct lnode {                    // linked list node - Storage memory address / memory data pairs
    struct lnode *next;
    t_address address;
    t_value data ;
}; 

struct llist {                    // Head of linked list
    struct lnode *head;
    int size;
};

/* -----------------------------------------------------------------
 * struct llist * new_list(void)
 * ----------------------------------------------------------------- */
struct llist * new_list()
{ 
    struct llist *lst;

    lst = (struct llist *) malloc(sizeof(struct llist));
    if (lst == NULL) {
        printf("\n **new_list() : malloc error");
        return NULL;
    }
    lst->head = 0;
    lst->size = 0;

    return lst;
}

void free_list(struct llist *lst)
{
    if (lst)
    {
        struct lnode *p, *n;
        p = lst->head;
        while (p)
        {
            n = p->next;
            free(p);
            p = n;
        }
        free(lst);
    }
}

/* -----------------------------------------------------------------
 * struct new_node * new_node(void * item, t_value data)
 * ----------------------------------------------------------------- */
struct lnode * new_node(t_address address, t_value data) {
    struct lnode *node;

    node = (struct lnode *) malloc (sizeof (struct lnode));
    if (node == NULL) {
        printf("\n **new_node() : malloc error");
        return NULL;
    }
    node->address = address;
    node->data = data;
    node->next = 0;

    return node;
}  

/* -----------------------------------------------------------------
 * int l_search(struct llist *ls, void *address) {
 * ----------------------------------------------------------------- */
struct lnode * l_search(struct llist *ls, t_address address) {
    struct lnode *node;

    node = ls->head;
    while ( node != NULL && node->address != address ) {
        node = node->next ;
    }
    if (node == NULL) {
        return 0; /* 'item' not found */
    }

    return node;
}

/* -----------------------------------------------------------------
 * int l_insert(struct llist *ls, void *item)
 * ----------------------------------------------------------------- */
int l_insert(struct llist *ls, t_address address, t_value data)
{
    struct lnode *node;

    if( l_search(ls, address)) return -1 ;
    node = new_node(address, data);
    if (node == NULL) return 0;
    node->next = ls->head;
    ls->head = node;  
    (ls->size)++;

    return 1;
}

void l_remove(struct llist *ls, t_address addr)
{
    if (ls)
    {
        struct lnode *p, *l;
        l = 0;
        p = ls->head;
        while (p)
        {
            if (p->address == addr)
            {
                if (l)
                    l->next = p->next;
                else
                    ls->head = p->next;
                (ls->size)--;
                return;
            }
            l = p;
            p = p->next;
        }
    }
}
 
/* -----------------------------------------------------------------
 *  Create Linked Lists
 * ----------------------------------------------------------------- */
 
void usage(void) {
    fprintf(stderr,"usage capdis [options] <file> <load address>\n"
                    "options:\n"
                    " -c=<count> disassemble at most <count> instructions\n"
                    " -s=<address> start disassembling at <address>. LSB controls ARM/thumb mode\n"
                    " -e=<address> stop disassembling at <address>\n"
                    " -o=<offset> start disassembling at <offset>. LSB controls ARM/thumb mode\n"
                    " -f=<chdk|objdump> format as CHDK inline ASM, or similar to objdump (default clean ASM)\n"
                    " -armv5 make firmware_load treat firmware as armv5\n"
                    " -v increase verbosity\n"
                    " -d-const add details about pc relative constant LDRs\n"
                    " -d-bin print instruction hex dump\n"
                    " -d-addr print instruction addresses\n"
                    " -d-ops print instruction operand information\n"
                    " -d-groups print instruction group information\n"
                    " -d all of the -d-* above\n"
                    " -noloc don't generate loc_xxx: labels and B loc_xxx\n"
                    " -nosub don't generate BL sub_xxx\n"
                    " -noconst don't generate LDR Rd,=0xFOO\n"
                    " -adrldr convert ADR Rd,#x and similar to LDR RD,=... (default with -f=chdk)\n"
                    " -noadrldr don't convert ADR Rd,#x and similar to LDR RD,=...\n"
              );
    exit(1);
}

// map friendly names to arm_op_type_enum
static const char *arm_op_type_name(int op_type) {
    switch(op_type) {
        case ARM_OP_INVALID:
            return "invalid";
        case ARM_OP_REG:
            return "reg";
        case ARM_OP_IMM:
            return "imm";
        case ARM_OP_MEM:
            return "mem";
        case ARM_OP_FP:
            return "fp";
        case ARM_OP_CIMM:
            return "cimm";
        case ARM_OP_PIMM:
            return "pimm";
        case ARM_OP_SETEND:
            return "setend";
        case ARM_OP_SYSREG:
            return "sysreg";
    }
    return "unk";
}
// hack for inline C vs ASM style
const char *comment_start = ";";

static void describe_insn_ops(csh handle, cs_insn *insn) {
    printf("%s OPERANDS %d:\n",comment_start,insn->detail->arm.op_count);
    int i;
    for(i=0;i<insn->detail->arm.op_count;i++) {
        printf("%s  %d: %s",comment_start,i,arm_op_type_name(insn->detail->arm.operands[i].type));
        switch(insn->detail->arm.operands[i].type) {
            case ARM_OP_IMM:
                printf("=0x%x",insn->detail->arm.operands[i].imm);
                break;
            case ARM_OP_MEM: {
                const char *reg=cs_reg_name(handle,insn->detail->arm.operands[i].mem.base);
                if(reg) {
                    printf(" base=%s",reg);
                }
                reg=cs_reg_name(handle,insn->detail->arm.operands[i].mem.index);
                if(reg) {
                    printf(" index=%s",reg);
                }
                if(insn->detail->arm.operands[i].mem.disp) {
                    printf(" scale=%d disp=0x%08x",
                        insn->detail->arm.operands[i].mem.scale,
                        insn->detail->arm.operands[i].mem.disp);
                }
                break;
            }
            case ARM_OP_REG:
                printf(" %s",cs_reg_name(handle,insn->detail->arm.operands[i].reg));
                break;
            default:
                break;
        } 
        printf("\n");
    }
}
static void describe_insn_groups(csh handle, cs_insn *insn) {
    int i;
    printf("%s GROUPS %d:",comment_start,insn->detail->groups_count);
    for(i=0;i<insn->detail->groups_count;i++) {
        if(i>0) {
            printf(",");
        }
        printf("%s",cs_group_name(handle,insn->detail->groups[i]));
    }
    printf("\n");
}

#define DIS_OPT_LABELS          0x00000001
#define DIS_OPT_SUBS            0x00000002
#define DIS_OPT_CONSTS          0x00000004
#define DIS_OPT_FMT_CHDK        0x00000008
#define DIS_OPT_FMT_OBJDUMP     0x00000010
#define DIS_OPT_ADR_LDR         0x00000020

#define DIS_OPT_DETAIL_GROUP    0x00010000
#define DIS_OPT_DETAIL_OP       0x00020000
#define DIS_OPT_DETAIL_ADDR     0x00040000
#define DIS_OPT_DETAIL_BIN      0x00080000
#define DIS_OPT_DETAIL_CONST    0x00100000
#define DIS_OPT_DETAIL_ALL      (DIS_OPT_DETAIL_GROUP\
                                    |DIS_OPT_DETAIL_OP\
                                    |DIS_OPT_DETAIL_ADDR\
                                    |DIS_OPT_DETAIL_BIN\
                                    |DIS_OPT_DETAIL_CONST)

static void do_dis_insn(
                    firmware *fw,
                    iter_state_t *is,
                    unsigned dis_opts,
                    char *mnem,
                    char *ops,
                    char *comment) {

    cs_insn *insn=is->insn;

    strcpy(mnem,insn->mnemonic);
    ops[0]=0;
    comment[0]=0;
    if((dis_opts & DIS_OPT_LABELS) && cs_insn_group(is->cs_handle,insn,CS_GRP_JUMP)
       && insn->detail->arm.op_count == 1
       && insn->detail->arm.operands[0].type == ARM_OP_IMM) {
        sprintf(ops,"loc_%08x",insn->detail->arm.operands[0].imm);
    // capstone doesn't put bl in CS_GRP_CALL, so look for BL and BLX
    } else if((dis_opts & DIS_OPT_SUBS) && (insn->id == ARM_INS_BL || insn->id == ARM_INS_BLX) 
            && insn->detail->arm.operands[0].type == ARM_OP_IMM) {
        sprintf(ops,"sub_%08x",insn->detail->arm.operands[0].imm);
    // convert LDR Rd, [PC,#foo] to LDR Rd,=... or detail calculated address/value
    } else if((dis_opts & (DIS_OPT_CONSTS|DIS_OPT_DETAIL_CONST)) && isLDR_PC(insn))  {
        // get address for display, rather than LDR_PC2valptr directly
        uint32_t ad=LDR_PC2adr(fw,insn);
        uint32_t *pv=(uint32_t *)adr2ptr(fw,ad);
        // don't handle neg scale for now, make sure not out of bounds
        if(pv) {
            if(dis_opts & DIS_OPT_CONSTS) {
                sprintf(ops,"%s, =0x%08x",
                        cs_reg_name(is->cs_handle,insn->detail->arm.operands[0].reg),
                        *pv);
                if(dis_opts & DIS_OPT_DETAIL_CONST) {
                    // show pc + and calculated addr
                    sprintf(comment,"[pc, #%d] (0x%08x)",insn->detail->arm.operands[1].mem.disp,ad);
                }
            } else if(dis_opts & DIS_OPT_DETAIL_CONST) {
                strcpy(ops,insn->op_str);
                // thumb2dis.pl style
                sprintf(comment,"0x%08x: (%08x)",ad,*pv);
            }
        } else {
            sprintf(comment,"WARNING didn't convert PC rel to constant!");
            strcpy(ops,insn->op_str);
        }
    } else if((dis_opts & (DIS_OPT_CONSTS|DIS_OPT_DETAIL_CONST)) && (insn->id == ARM_INS_ADR))  {
        uint32_t ad=ADR2adr(fw,insn);
        uint32_t *pv=(uint32_t *)adr2ptr(fw,ad);
        // capstone doesn't appear to generate ADR for thumb, so no special case needed
        if(pv) {
            if(dis_opts & DIS_OPT_ADR_LDR) {
                strcpy(mnem,"ldr");
                sprintf(ops,"%s, =0x%08x",
                        cs_reg_name(is->cs_handle,insn->detail->arm.operands[0].reg),
                        ad);
                if(dis_opts & DIS_OPT_DETAIL_CONST) {
                    // show original ADR
                    sprintf(comment,"adr %s, #%x (0x%08x)",
                            cs_reg_name(is->cs_handle,insn->detail->arm.operands[0].reg), 
                            insn->detail->arm.operands[1].imm,
                            *pv);
                }
            } else {
                if(dis_opts & DIS_OPT_FMT_OBJDUMP) {
                    strcpy(mnem,"add");
                    sprintf(ops,"%s, pc, #%d",
                            cs_reg_name(is->cs_handle,insn->detail->arm.operands[0].reg), 
                            insn->detail->arm.operands[1].imm);
                } else {
                    strcpy(ops,insn->op_str);
                }
                if(dis_opts & DIS_OPT_DETAIL_CONST) {
                    // thumb2dis.pl style
                    sprintf(comment,"0x%08x: (%08x)",ad,*pv);
                }
            }
        } else {
            sprintf(comment,"WARNING didn't convert ADR to constant!");
            strcpy(ops,insn->op_str);
        }
    } else if((dis_opts & (DIS_OPT_CONSTS|DIS_OPT_DETAIL_CONST)) && isSUBW_PC(insn))  {
        // it looks like subw is thubm only, so shouldn't need special case for arm?
        unsigned ad=ADRx2adr(fw,insn);
        uint32_t *pv=(uint32_t *)adr2ptr(fw,ad);
        if(pv) {
            if(dis_opts & DIS_OPT_ADR_LDR) {
                strcpy(mnem,"ldr");
                sprintf(ops,"%s, =0x%08x",
                        cs_reg_name(is->cs_handle,insn->detail->arm.operands[0].reg),
                        ad);
                if(dis_opts & DIS_OPT_DETAIL_CONST) {
                    // show original subw
                    sprintf(comment,"subw %s, pc, #%x (0x%08x)",
                            cs_reg_name(is->cs_handle,insn->detail->arm.operands[0].reg), 
                            insn->detail->arm.operands[2].imm,
                            *pv);
                }
            } else {
                strcpy(ops,insn->op_str);
                if(dis_opts & DIS_OPT_DETAIL_CONST) {
                    // thumb2dis.pl style
                    sprintf(comment,"0x%08x: (%08x)",ad,*pv);
                }
            }
        } else {
            sprintf(comment,"WARNING didn't convert SUBW Rd, PC, #x to constant!");
            strcpy(ops,insn->op_str);
        }
    } else if((dis_opts & (DIS_OPT_CONSTS|DIS_OPT_DETAIL_CONST)) && isADDW_PC(insn))  {
        // it looks like addw is thubm only, so shouldn't need special case for arm?
        unsigned ad=ADRx2adr(fw,insn);
        uint32_t *pv=(uint32_t *)adr2ptr(fw,ad);
        if(pv) {
            if(dis_opts & DIS_OPT_ADR_LDR) {
                strcpy(mnem,"ldr");
                sprintf(ops,"%s, =0x%08x",
                        cs_reg_name(is->cs_handle,insn->detail->arm.operands[0].reg),
                        ad);
                if(dis_opts & DIS_OPT_DETAIL_CONST) {
                    // show original subw
                    sprintf(comment,"addw %s, pc, #%x (0x%08x)",
                            cs_reg_name(is->cs_handle,insn->detail->arm.operands[0].reg), 
                            insn->detail->arm.operands[2].imm,
                            *pv);
                }
            } else {
                strcpy(ops,insn->op_str);
                if(dis_opts & DIS_OPT_DETAIL_CONST) {
                    // thumb2dis.pl style
                    sprintf(comment,"0x%08x: (%08x)",ad,*pv);
                }
            }
        } else {
            sprintf(comment,"WARNING didn't convert ADDW Rd, PC, #x to constant!");
            strcpy(ops,insn->op_str);
        }
    } else {
        strcpy(ops,insn->op_str);
    }
}

static void do_dis_range(firmware *fw,
                    unsigned dis_start,
                    unsigned dis_count,
                    unsigned dis_end,
                    unsigned dis_opts)
{
    iter_state_t *is=disasm_iter_new(fw,dis_start);
    size_t count=0;
    struct llist *branch_list = new_list();

    // pre-scan for branches
    if(dis_opts & DIS_OPT_LABELS) {
        // TODO should use fw_search_insn, but doesn't easily support count
        while(count < dis_count &&  is->adr < dis_end) {
            if(disasm_iter(fw,is)) {
                uint32_t b_tgt=B_BL_BLXimm_target(fw,is->insn);
                if(b_tgt) {
                    l_insert(branch_list,b_tgt,0);
                }
            } else {
                if(!disasm_iter_init(fw,is,(is->adr+is->insn_min_size) | is->thumb)) {
                    fprintf(stderr,"do_dis_range: disasm_iter_init failed\n");
                    break;
                }
            }
            count++;
        }
    }
    count=0;
    disasm_iter_init(fw,is,dis_start);
    while(count < dis_count && is->adr < dis_end) {
        if(disasm_iter(fw,is)) {
            if(dis_opts & DIS_OPT_LABELS) {
                struct lnode *label = l_search(branch_list,is->insn->address);
                if(label) {
                    if(dis_opts & DIS_OPT_FMT_CHDK) {
                        printf("\"");
                    }
                    printf("loc_%08x:", label->address);
                    if(dis_opts & DIS_OPT_FMT_CHDK) {
                        printf("\\n\"");
                    }
                    printf("\n");
                }
            }
            if(!(dis_opts & DIS_OPT_FMT_OBJDUMP) // objdump format puts these on same line as instruction
                && (dis_opts & (DIS_OPT_DETAIL_ADDR | DIS_OPT_DETAIL_BIN))) {
                printf(comment_start);
                if(dis_opts & DIS_OPT_DETAIL_ADDR) {
                    printf(" 0x%"PRIx64"",is->insn->address);
                }
                if(dis_opts & DIS_OPT_DETAIL_BIN) {
                    int k;
                    for(k=0;k<is->insn->size;k++) {
                        printf(" %02x",is->insn->bytes[k]);
                    }
                }
                printf("\n");
            }
            if(dis_opts & DIS_OPT_DETAIL_OP) {
                describe_insn_ops(is->cs_handle,is->insn);
            }
            if(dis_opts & DIS_OPT_DETAIL_GROUP) {
                describe_insn_groups(is->cs_handle,is->insn);
            }
            // mnemonic and op size from capstone.h
            char insn_mnemonic[32], insn_ops[160], comment[256];
            do_dis_insn(fw,is,dis_opts,insn_mnemonic,insn_ops,comment);
            if(dis_opts & DIS_OPT_FMT_CHDK) {
                printf("\"");
            }
/*
objdump / thumb2dis format examples
fc000016: \tf8df d004 \tldr.w\tsp, [pc, #4]\t; 0xfc00001c: (1ffffffc) 
fc000020: \tbf00      \tnop

TODO most constants are decimal, while capstone defaults to hex
*/

            if(dis_opts & DIS_OPT_FMT_OBJDUMP) {// objdump format puts these on same line as instruction
                if(dis_opts & DIS_OPT_DETAIL_ADDR) {
                    printf("%"PRIx64": \t",is->insn->address);
                }
                if(dis_opts & DIS_OPT_DETAIL_BIN) {
                    if(is->insn->size == 2) {
                        printf("%04x ",*(unsigned short *)is->insn->bytes);
                    } else if(is->insn->size == 4) {
                        printf("%04x %04x",*(unsigned short *)is->insn->bytes,*(unsigned short *)(is->insn->bytes+2));
                    }
                }
                printf(" \t%s", insn_mnemonic);
                if(strlen(insn_ops)) {
                    printf("\t%s",insn_ops);
                }
                if(strlen(comment)) {
                    printf("\t%s %s",comment_start,comment);
                }
            } else {
                printf("    %-7s", insn_mnemonic);
                if(strlen(insn_ops)) {
                    printf(" %s",insn_ops);
                }
                if(dis_opts & DIS_OPT_FMT_CHDK) {
                    printf("\\n\"");
                }
                if(strlen(comment)) {
                    printf(" %s %s",comment_start,comment);
                }
            }
            printf("\n");
        } else {
            uint16_t *pv=(uint16_t *)adr2ptr(fw,is->adr);
            // TODO optional data directives
            if(pv) {
                if(is->thumb) {
                    printf("%s %04x\n",comment_start,*pv);
                } else {
                    printf("%s %04x %04x\n",comment_start,*pv,*(pv+1));
                }
            } else {
                printf("%s invalid address %"PRIx64"\n",comment_start,is->adr);
            }
            if(!disasm_iter_init(fw,is,(is->adr+is->insn_min_size)|is->thumb)) {
                fprintf(stderr,"do_dis_range: disasm_iter_init failed\n");
                break;
            }
        }
        count++;
    }
    free_list(branch_list);

}

int main(int argc, char** argv)
{
    char *dumpname=NULL;
    unsigned load_addr=0xFFFFFFFF;
    unsigned offset=0;
    unsigned dis_start=0;
    unsigned dis_end=0;
    unsigned dis_count=0;
    int verbose=0;
    unsigned dis_opts=(DIS_OPT_LABELS|DIS_OPT_SUBS|DIS_OPT_CONSTS);
    int dis_arch=FW_ARCH_ARMv7;
    if(argc < 2) {
        usage();
    }
    int i;
    for(i=1; i < argc; i++) {
        if ( strncmp(argv[i],"-c=",3) == 0 ) {
            dis_count=strtoul(argv[i]+3,NULL,0);
        }
        else if ( strncmp(argv[i],"-o=",3) == 0 ) {
            offset=strtoul(argv[i]+3,NULL,0);
        }
        else if ( strncmp(argv[i],"-s=",3) == 0 ) {
            dis_start=strtoul(argv[i]+3,NULL,0);
        }
        else if ( strncmp(argv[i],"-e=",3) == 0 ) {
            dis_end=strtoul(argv[i]+3,NULL,0);
        }
        else if ( strncmp(argv[i],"-f=",3) == 0 ) {
            if ( strcmp(argv[i]+3,"chdk") == 0 ) {
                dis_opts |= DIS_OPT_FMT_CHDK;
                dis_opts |= DIS_OPT_ADR_LDR;
            } else if ( strcmp(argv[i]+3,"objdump") == 0 ) {
                dis_opts |= DIS_OPT_FMT_OBJDUMP;
            } else {
                fprintf(stderr,"unknown output format %s\n",argv[i]+3);
                usage();
            }
        }
        else if ( strcmp(argv[i],"-armv5") == 0 ) {
            dis_arch=FW_ARCH_ARMv5;
        }
        else if ( strcmp(argv[i],"-v") == 0 ) {
            verbose++;
        }
        else if ( strcmp(argv[i],"-d") == 0 ) {
            dis_opts |= DIS_OPT_DETAIL_ALL;
        }
        else if ( strcmp(argv[i],"-noloc") == 0 ) {
            dis_opts &= ~DIS_OPT_LABELS;
        }
        else if ( strcmp(argv[i],"-nosub") == 0 ) {
            dis_opts &= ~DIS_OPT_SUBS;
        }
        else if ( strcmp(argv[i],"-noconst") == 0 ) {
            dis_opts &= ~DIS_OPT_CONSTS;
        }
        else if ( strcmp(argv[i],"-noadrldr") == 0 ) {
            dis_opts &= ~DIS_OPT_ADR_LDR;
        }
        else if ( strcmp(argv[i],"-adrldr") == 0 ) {
            dis_opts |= DIS_OPT_ADR_LDR;
        }
        else if ( strcmp(argv[i],"-d-const") == 0 ) {
            dis_opts |= DIS_OPT_DETAIL_CONST;
        }
        else if ( strcmp(argv[i],"-d-group") == 0 ) {
            dis_opts |= DIS_OPT_DETAIL_GROUP;
        }
        else if ( strcmp(argv[i],"-d-op") == 0 ) {
            dis_opts |= DIS_OPT_DETAIL_OP;
        }
        else if ( strcmp(argv[i],"-d-addr") == 0 ) {
            dis_opts |= DIS_OPT_DETAIL_ADDR;
        }
        else if ( strcmp(argv[i],"-d-bin") == 0 ) {
            dis_opts |= DIS_OPT_DETAIL_BIN;
        }
        else if ( argv[i][0]=='-' ) {
            fprintf(stderr,"unknown option %s\n",argv[i]);
            usage();
        } else {
            if(!dumpname) {
                dumpname=argv[i];
            } else if(load_addr == 0xFFFFFFFF) {
                load_addr=strtoul(argv[i],NULL,0);
            } else {
                fprintf(stderr,"unexpected %s\n",argv[i]);
                usage();
            }
        }
    }
    if(!dumpname) {
        fprintf(stderr,"missing input file\n");
        usage();
    }
    struct stat st;
    if(stat(dumpname,&st) != 0) {
        fprintf(stderr,"bad input file %s\n",dumpname);
        return 1;
    }
    int dumpsize = st.st_size;

    if(load_addr==0xFFFFFFFF) {
        fprintf(stderr,"missing load address\n");
        usage();
    }
    if(offset) {
        if(dis_start) {
            fprintf(stderr,"both start and and offset given\n");
            usage();
        }
        dis_start = offset+load_addr;
    }
    if(dis_end) {
        if(dis_count) {
            fprintf(stderr,"both end and count given\n");
            usage();
        }
        if(dis_end > load_addr+dumpsize) {
            fprintf(stderr,"end > load address + size\n");
            usage();
        }
        if(dis_end < dis_start) {
            fprintf(stderr,"end < start\n");
            usage();
        }
        dis_count=(dis_end-dis_start)/2; // need a count for do_dis_range, assume all 16 bit ins
    }
    if(dis_count==0) {
        fprintf(stderr,"missing instruction count\n");
        usage();
    }
    // count, no end
    if(dis_end==0) {
        dis_end=dis_start + dis_count * 4; // assume all 32 bit ins
    }
    if((dis_opts & (DIS_OPT_FMT_CHDK | DIS_OPT_FMT_OBJDUMP)) == (DIS_OPT_FMT_CHDK | DIS_OPT_FMT_OBJDUMP)) {
        fprintf(stderr,"multiple -f options not allowed\n");
        usage();
    }
    if(dis_opts & (DIS_OPT_FMT_CHDK)) {
        comment_start = "//";
    }
    
    firmware fw;
    firmware_load(&fw,dumpname,load_addr,dis_arch); 
    firmware_init_capstone(&fw);
    firmware_init_data_ranges(&fw);

    // check for RAM code address
    if(dis_start < fw.base) {
        adr_range_t *ar=adr_get_range(&fw,dis_start);
        if(!ar || ar->type != ADR_RANGE_RAM_CODE) {
            fprintf(stderr,"invalid start address 0x%08x\n",dis_start);
            return 1;
        }
    }

    if(verbose) {
        printf("%s %s size:0x%x start:0x%x instructions:%d opts:0x%x\n",comment_start,dumpname,dumpsize,dis_start,dis_count,dis_opts);
    }


    do_dis_range(&fw, dis_start, dis_count, dis_end, dis_opts);

    firmware_unload(&fw);


    return 0;
}