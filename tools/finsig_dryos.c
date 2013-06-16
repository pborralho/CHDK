#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>

#include "dancingbits.h"

//------------------------------------------------------------------------------------------------------------

// Buffer output into header and body sections

FILE    *out_fp;
char	out_buf[32*1024] = "";
int		out_len = 0;
char	hdr_buf[32*1024] = "";
int		hdr_len = 0;
int		out_hdr = 1;

void bprintf(char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);

	if (out_hdr)
		hdr_len += vsprintf(hdr_buf+hdr_len,fmt,argp);
	else
		out_len += vsprintf(out_buf+out_len,fmt,argp);

	va_end(argp);
}

void add_blankline()
{
	if (strcmp(hdr_buf+hdr_len-2,"\n\n") != 0)
	{
		hdr_buf[hdr_len++] = '\n';
		hdr_buf[hdr_len] = 0;
	}
}

void write_output()
{
	add_blankline();
    if (out_fp)
    {
	    fprintf(out_fp,"%s",hdr_buf);
    	fprintf(out_fp,"%s",out_buf);
    }
}

//------------------------------------------------------------------------------------------------------------

void usage(char *err)
{
    bprintf("finsig <primary> <base> <outputfilename> [alt base] - Error = %s\n",err);
    write_output();
    fprintf(stderr,"finsig <primary> <base> <outputfilename> [alt base] - Error = %s\n",err);
    exit(1);
}

//------------------------------------------------------------------------------------------------------------

// Load original stubs_entry.S, stubs_entry_2.s and stubs_min.S to compare results to

typedef struct _osig
{
    char        nm[100];
    uint32_t    val;
	char		sval[100];
    int         pct;
    struct _osig *nxt;
} osig;

osig *stubs2 = 0;
osig *stubs_min = 0;
osig *modemap = 0;

void print_stubs(osig *p)
{
    while (p)
    {
        bprintf("//%s 0x%08x (%s) %d\n",p->nm,p->val,p->sval,p->pct);
        p = p->nxt;
    }
}

int read_line(FILE *f, char *buf)
{
    int eof = 0;
    int len = 0;
    while (1)
    {
        if (fread(buf,1,1,f) != 1) { eof = 1; break; }
        if ((*buf == 0x0A) || (*buf == 0x0D)) break;
        len++; 
        buf++;
    }
    *buf = 0;
    return (eof == 0) || (len > 0);
}

char* get_str(char *s, char *d)
{
    while ((*s == ' ') || (*s == '\t') || (*s == ',')) s++;
    while (*s && (*s != ' ') && (*s != '\t') && (*s != ',') && (*s != ')'))
    {
        *d++ = *s++;
    }
	while (*s && (*s != ',') && (*s != ')'))
	{
		if (*s == '+')
		{
			*d++ = *s++;
			while ((*s == ' ') || (*s == '\t') || (*s == ',')) s++;
			while (*s && (*s != ' ') && (*s != '\t') && (*s != ',') && (*s != ')'))
			{
				*d++ = *s++;
			}
		}
		else s++;
	}
    *d = 0;
    return s;
}

void add_sig(char *nm, char *val, int pct, osig **hdr)
{
    osig *p = malloc(sizeof(osig));
    strcpy(p->nm, nm);
	strcpy(p->sval, val);
    p->pct = pct;
    p->nxt = *hdr;
    *hdr = p;

	uint32_t v = 0, n = 0;
	if ((strncmp(val,"0x",2) == 0) || (strncmp(val,"0X",2) == 0))
	{
		while (val)
		{
			sscanf(val,"%x",&n);
			v += n;
			val = strchr(val,'+');
			if (val) val++;
		}
	}
	else
	{
		sscanf(val,"%d",&v);
	}

	p->val = v;
}

osig* find_sig(osig* p, const char *nm)
{
    while (p)
    {
        if (strcmp(p->nm, nm) == 0) return p;
        p = p->nxt;
    }
    return 0;
}

osig* find_sig_val(osig* p, uint32_t val)
{
    while (p)
    {
        if (p->val == val) return p;
        p = p->nxt;
    }
    return 0;
}

void load_stubs2()
{
    FILE *f = fopen("stubs_entry_2.S", "rb");

    if (f == NULL) return;

    char line[500];
    char nm[100];
    char val[12];
    int pct = 100;
    char *s;

    while (read_line(f,line))
    {
		int off = 7;
        s = strstr(line, "NHSTUB(");
		if (s == 0) { off = 6; s = strstr(line, "NSTUB("); }
		if (s == 0) { off = 4; s = strstr(line, "DEF("); }
        if (s != 0)
        {
            char *c = strstr(line, "//");
            if ((c == 0) || (c > s))
            {
                s = get_str(s+off,nm);
                get_str(s,val);
                add_sig(nm, val, pct, &stubs2);
                pct = 100;
                continue;
            }
        }
    }
}

void load_stubs_min()
{
    FILE *f = fopen("stubs_min.S", "rb");

    if (f == NULL) return;

    char line[500];
    char nm[100];
    char val[12];
    int pct = 100;
    char *s;

    while (read_line(f,line))
    {
		int off = 7;
        s = strstr(line, "NHSTUB(");
		if (s == 0) { off = 4; s = strstr(line, "DEF("); }
        if (s != 0)
        {
            char *c = strstr(line, "//");
            if ((c == 0) || (c > s))
            {
                s = get_str(s+off,nm);
                get_str(s,val);
                add_sig(nm, val, pct, &stubs_min);
                pct = 100;
                continue;
            }
        }
    }
}

void load_modemap()
{
    FILE *f = fopen("../../shooting.c", "rb");

    if (f == NULL) return;

    char line[500];
    char nm[100];
    char val[12];
	int found_modemap = 0;
    char *s;

    while (read_line(f,line))
    {
		if (found_modemap)
		{
			s = strstr(line, "};");
			if (s != 0) return;
			s = strstr(line, "MODE_");
			if (s != 0)
			{
				char *c = strstr(line, "//");
				if ((c == 0) || (c > s))
				{
					s = get_str(s,nm);
					get_str(s,val);
					add_sig(nm, val, 0, &modemap);
				}
			}
		}
		else
		{
			s = strstr(line, "modemap[");
			if (s != 0) found_modemap = 1;
		}
    }
}

int min_focus_len = 0;
int max_focus_len = 0;

void load_platform()
{
    FILE *f = fopen("../../platform_camera.h", "rb");

    if (f == NULL) return;

    char line[500];
    char val[12];
    char div[12];
    int v, d;
    char *s;

    while (read_line(f,line))
    {
		s = strstr(line, "CAM_DNG_LENS_INFO");
		if (s != 0)
		{
			char *c = strstr(line, "//");
			if ((c == 0) || (c > s))
			{
                s = strstr(line,"{")+1;
				s = get_str(s,val);
				s = get_str(s,div);
                v = atoi(val);
                d = atoi(div);
                min_focus_len = (v * 1000) / d;
				s = get_str(s,val);
				s = get_str(s,div);
                v = atoi(val);
                d = atoi(div);
                max_focus_len = (v * 1000) / d;
			}
		}
    }
}

//------------------------------------------------------------------------------------------------------------

// Signature match handling

typedef struct {
    uint32_t ptr;
    uint32_t fail;
    uint32_t success;
	int sig;
} Match;

int match_compare(const Match *p1, const Match *p2)
{
    /* NOTE: If a function has *more* matches, it will be prefered, even if it has a lower percent matches */
    if (p1->success > p2->success)
	{
		if ((p2->fail == 0) && (p1->fail > 0))
		{
			return 1;
		}
		else
		{
			return -1;
		}
    }
	else if (p1->success < p2->success)
	{
		if ((p1->fail == 0) && (p2->fail > 0))
		{
			return -1;
		}
		else
		{
			return 1;
		}
    }
	else
	{
        if (p1->fail < p2->fail)
		{
            return -1;
        }
		else if (p1->fail > p2->fail)
		{
            return 1;
        }
    }

	if (p1->sig < p2->sig)
	{
		return -1;
	}
	else if (p1->sig > p2->sig)
	{
		return 1;
	}
	
    /* scores are equal. prefer lower address */

    if (p1->ptr < p2->ptr)
	{
        return -1;
    }
	else if (p1->ptr > p2->ptr)
	{
        return 1;
    }

    return 0;
}

#define MAX_MATCHES (8192)

Match matches[MAX_MATCHES];
int count;

void addMatch(uint32_t fadr, int s, int f, int sig)
{
    matches[count].ptr = fadr;
    matches[count].success = s;
    matches[count].fail = f;
	matches[count].sig = sig;
    count++;
}

//------------------------------------------------------------------------------------------------------------

// Signature structures generated by gensig2.exe

typedef struct {
    uint32_t offs;
    uint32_t value;
    uint32_t mask;
} FuncSig;

typedef struct {
    const char *name;
    FuncSig *sig;
	int ver;
} FuncsList;

//------------------------------------------------------------------------------------------------------------

// Firmware handling

typedef struct bufrange {
    uint32_t *p;
    int off;
    int len;
    struct bufrange* next;
} BufRange;

typedef struct {
    uint32_t        *buf;
    uint32_t        base;
    int             size;
    BufRange        *br, *last;
	int			    dryos_ver;
    int             pid;
    int             maxram;
	char		    cam[100];

    // Alt copy of ROM in RAM (DryOS R50, R51)
    uint32_t        *buf2;          // pointer to loaded FW data that is copied
    uint32_t        base2;          // RAM address copied to
    uint32_t        base_copied;    // ROM address copied from
    int             size2;          // Block size copied (in words)

    // Alt copy of ROM (DryOS R51 - only seen on S110 so far)
    uint32_t        alt_base;       // Alternative base address
} firmware;

int idx_valid(firmware *fw, int i)
{
    if ((i >= 0) && (i < fw->size))
        return 1;
    if ((fw->dryos_ver >= 51) && (fw->alt_base) && (i >= fw->size))
    {
        i = ((i * 4) - (fw->alt_base - fw->base)) / 4;
        if ((i >= 0) && (i < fw->size))
            return 1;
    }
    if ((fw->dryos_ver >= 50) && (i < 0))
    {
        i = ((i * 4) + (fw->base - fw->base2)) / 4;
        if ((i >= 0) && (i < fw->size2))
            return 1;
    }
    return 0;
}

uint32_t* fwadr(firmware *fw, int i)
{
    if ((i >= 0) && (i < fw->size))
        return &fw->buf[i];
    if ((fw->dryos_ver >= 51) && (fw->alt_base) && (i >= fw->size))
    {
        i = ((i * 4) - (fw->alt_base - fw->base)) / 4;
        if ((i >= 0) && (i < fw->size))
            return &fw->buf[i];
    }
    if ((fw->dryos_ver >= 50) && (i < 0))
    {
        i = ((i * 4) + (fw->base - fw->base2)) / 4;
        if ((i >= 0) && (i < fw->size2))
            return &fw->buf2[i];
    }
    fprintf(stderr,"Invalid firmware offset %d.\n",i);
    bprintf("\nInvalid firmware offset %d. Possible corrupt firmware or incorrect start address.\n",i);
    write_output();
    exit(1);
}

uint32_t fwval(firmware *fw, int i)
{
    return *fwadr(fw,i);
}

int fwRd(firmware *fw, int i)
{
    // Destination register - Rd
    return (*fwadr(fw,i) & 0x0000F000) >> 12;
}

int fwRn(firmware *fw, int i)
{
    // Source register - Rn
    return (*fwadr(fw,i) & 0x000F0000) >> 16;
}

int fwOp2(firmware *fw, int i)
{
    // Operand2
    return (*fwadr(fw,i) & 0x00000FFF);
}

void addBufRange(firmware *fw, int o, int l)
{
    BufRange *n = malloc(sizeof(BufRange));
    n->p = fw->buf + o;
    n->off = o;
    n->len = l;
    n->next = 0;
    if (fw->br == 0)
    {
        fw->br = n;
    }
    else
    {
        fw->last->next = n;
    }
    fw->last = n;
}

int find_str(firmware *fw, char *str)
{
    int nlen = strlen(str);
    uint32_t nm0 = *((uint32_t*)str);
	uint32_t *p;
	int j;

    BufRange *br = fw->br;
    while (br)
    {
        for (p = br->p, j = 0; j < br->len - nlen/4; j++, p++)
        {
            if ((nm0 == *p) && (memcmp(p+1,str+4,nlen-4) == 0))
            {
			    return j+br->off;
		    }
	    }
        br = br->next;
    }
	
	return -1;
}

void findRanges(firmware *fw)
{
    int i, j, k;

    // Find all the valid ranges for checking (skips over large blocks of 0xFFFFFFFF)
    fw->br = 0; fw->last = 0;
    k = -1; j = 0;
    for (i = 0; i < fw->size; i++)
    {
        if (fw->buf[i] == 0xFFFFFFFF)   // Possible start of block to skip
        {
            if (k == -1)            // Mark start of possible skip block
            {
                k = i;
            }
        }
        else                        // Found end of block ?
        {
            if (k != -1)
            {
                if (i - k > 32)     // If block more than 32 words then we want to skip it
                {
                    if (k - j > 8)
                    {
                        // Add a range record for the previous valid range (ignore short ranges)
                        addBufRange(fw,j,k - j);
                    }
                    j = i;          // Reset valid range start to current position
                }
                k = -1;             // Reset marker for skip block
            }
        }
    }
    // Add range for last valid block
    if (k != -1)    
    {
        if (k - j > 8)
        {
            addBufRange(fw,j,k - j);
        }
    }
    else
    {
        if (i - j > 8)
        {
            addBufRange(fw,j,i - j);
        }
    }
}

uint32_t idx2adr(firmware *fw, int idx)
{
    return fw->base + (idx << 2);
}

int adr2idx(firmware *fw, uint32_t adr)
{
    if (adr < fw->base)
        return -((fw->base - adr) >> 2);
    else
        return (adr - fw->base) >> 2;
}

int ptr2idx(firmware *fw, int idx)
{
    return (fwval(fw,idx) - fw->base) >> 2;
}

uint32_t LDR2adr(firmware *fw, int offset)  // decode LDR instruction at offset and return firmware address pointed to
{
    uint32_t inst = fwval(fw,offset);
    int offst = (inst & 0xFFF);
    uint32_t fadr = (inst & 0x00800000)?idx2adr(fw,offset+2)+offst:idx2adr(fw,offset+2)-offst;
    return fadr;
}

uint32_t LDR2val(firmware *fw, int offset)  // decode LDR instruction at offset and return firmware value stored at the address
{
	return fwval(fw,adr2idx(fw,LDR2adr(fw,offset)));
}

uint32_t LDR2idx(firmware *fw, int offset)  // decode LDR instruction at offset and return firmware buffer index of the new address
{
	return adr2idx(fw,LDR2adr(fw,offset));
}

int idxFollowBranch(firmware *fw, int fidx, int offset)
{
    if (offset)
    {
		uint32_t msk = ~(offset & 0xFF000000);
        fidx += ((offset & 0x00FFFFFF) - 1);
        uint32_t inst = fwval(fw,fidx);
        if ((inst & (0xFF000000&msk)) == (0xEA000000&msk))  // Branch (B or BL depending on msk)
        {
            int o = inst & 0x00FFFFFF;
            if (o & 0x00800000) o |= 0xFF000000;
            fidx = fidx + o + 2;
        }
        else if ((inst & (0xFFFFF000)) == (0xE51FF000))     // LDR PC,=...
        {
            fidx = adr2idx(fw,LDR2val(fw,fidx));
        }
    }
    return fidx;
}

uint32_t followBranch(firmware *fw, uint32_t fadr, int offset)
{
    if (offset)
    {
		uint32_t msk = ~(offset & 0xFF000000);
        uint32_t fidx = adr2idx(fw,fadr);  // function index
        fidx += ((offset & 0x00FFFFFF) - 1);
        uint32_t inst = fwval(fw,fidx);
        if ((inst & (0xFF000000&msk)) == (0xEA000000&msk))  // Branch (B or BL depending on msk)
        {
            int o = inst & 0x00FFFFFF;
            if (o & 0x00800000) o |= 0xFF000000;
            fadr = idx2adr(fw,fidx+o+2);
        }
        else if ((inst & (0xFFFFF000)) == (0xE51FF000))     // LDR PC,=...
        {
            fadr = LDR2val(fw,fidx);
        }
    }
    return fadr;
}

uint32_t followBranch2(firmware *fw, uint32_t fadr, int offset)
{
	fadr = followBranch(fw, fadr, offset);
	if ((offset & 0x00FFFFFF) == 1)
		fadr = followBranch(fw, fadr, offset);
	return fadr;
}

uint32_t ADR2adr(firmware *fw, int offset)  // decode ADR instruction at offset and return firmware address pointed to
{
    uint32_t inst = fwval(fw,offset);
    int rot = 32 - ((inst & 0xF00) >> 7);
    int offst = (inst & 0xFF) <<rot;
    uint32_t fadr = 0;
    switch (inst & 0x01E00000)
    {
    case 0x00400000:    // SUB
        fadr = idx2adr(fw,offset+2)-offst;
        break;
    case 0x00800000:    // ADD
        fadr = idx2adr(fw,offset+2)+offst;
        break;
    case 0x01A00000:    // MOV
        //fprintf(stderr,"***** ADR2adr MOV\n");
        break;
    case 0x01E00000:    // MVN
        //fprintf(stderr,"***** ADR2adr MVN\n");
        break;
    }
    return fadr;
}

uint32_t ALUop2(firmware *fw, int offset)  // decode operand2 from ALU inst (not complete!)
{
    uint32_t inst = fwval(fw,offset);
    int rot = 32 - ((inst & 0xF00) >> 7);
    int offst = (inst & 0xFF) <<rot;
    uint32_t fadr = 0;
    switch (inst & 0x03E00000)
    {
    case 0x02400000:    // SUB Immed
    case 0x02800000:    // ADD Immed
    case 0x03A00000:    // MOV Immed
    case 0x03C00000:    // BIC Immed
		fadr = offst;
        break;
    }
    return fadr;
}

int isLDR_PC(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0xFE1F0000) == 0xE41F0000);
}

int isLDR_SP(firmware *fw, int offset)
{
    return ((fwval(fw,offset) & 0xFFFF0000) == 0xE59D0000); // LDR Rx,[SP,x]
}

int isLDR_PC_cond(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0x0E1F0000) == 0x041F0000);
}

int isADR_PC(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0xFE0F0000) == 0xE20F0000);
}

int isADR_PC_cond(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0x0E0F0000) == 0x020F0000);
}

int isSTR_PC(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0xFE1F0000) == 0xE40F0000);
}

int isSTR_PC_cond(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0x0E1F0000) == 0x040F0000);
}

int isLDMFD(firmware *fw, int offset)
{
    return ((fwval(fw,offset) & 0xFFFF0000) == 0xE8BD0000);
}

int isLDMFD_PC(firmware *fw, int offset)
{
    return ((fwval(fw,offset) & 0xFFFF8000) == 0xE8BD8000);
}

int isLDR(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0xFE100000) == 0xE4100000);
}

int isLDR_cond(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0x0E100000) == 0x04100000);
}

int isADR(firmware *fw, int offset)
{
    return ((fwval(fw,offset) & 0xFE000000) == 0xE2000000); // ADR or MOV
}

int isSTMFD(firmware *fw, int offset)
{
    return ((fwval(fw,offset) & 0xFFFF0000) == 0xE92D0000); // STMFD SP!, 
}

int isSTMFD_LR(firmware *fw, int offset)
{
    return ((fwval(fw,offset) & 0xFFFF4000) == 0xE92D4000); // STMFD SP!, {..,LR}
}

int isSTR(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0xFE100000) == 0xE4000000);
}

int isSTR_cond(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0x0E100000) == 0x04000000);
}

int isBX(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0xFFFFFFF0) == 0xE12FFF10);	// BX
}

int isBX_LR(firmware *fw, int offset)
{
	return (fwval(fw,offset) == 0xE12FFF1E);	            // BX LR
}

int isBL(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0xFF000000) == 0xEB000000);	// BL
}

int isB(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0xFF000000) == 0xEA000000);	// B
}

int isBorBL(firmware *fw, int offset)
{
	return ((fwval(fw,offset) & 0xFE000000) == 0xEA000000);	// B or BL
}

int isCMP(firmware *fw, int offset)
{
    return ((fwval(fw,offset) & 0xFFF00000) == 0xE3500000); // CMP
}

int isMOV(firmware *fw, int offset)
{
    return ((fwval(fw,offset) & 0xFFF00000) == 0xE1A00000); // MOV
}

int isMOV_immed(firmware *fw, int offset)
{
    return ((fwval(fw,offset) & 0xFFF00000) == 0xE3A00000); // MOV Rd, #
}

int find_inst(firmware *fw, int (*inst)(firmware*,int), int idx, int len)
{
    int k;
    for (k = idx; (k < fw->size) && (k < idx + len); k++)
    {
        if (inst(fw, k))
            return k;
    }
    return -1;
}

int find_inst_rev(firmware *fw, int (*inst)(firmware*,int), int idx, int len)
{
    int k;
    for (k = idx; (k > 0) && (k > idx - len); k--)
    {
        if (inst(fw, k))
            return k;
    }
    return -1;
}

void load_firmware(firmware *fw, char *filename, char *base_addr, char *alt_base_addr)
{
    FILE *f = fopen(filename, "rb");
    int k;

    if (f == NULL)
	{
		fprintf(stderr,"Error opening %s\n",filename);
        usage("firmware open");
	}

    fw->base = strtoul(base_addr, NULL, 0);
    if (alt_base_addr)
        fw->alt_base = strtoul(alt_base_addr, NULL, 0);
    else
        fw->alt_base = 0;

    fseek(f,0,SEEK_END);
    fw->size = (ftell(f)+3)/4;
    fseek(f,0,SEEK_SET);

    // Max sig size if 32, add extra space at end of buffer and fill with 0xFFFFFFFF
    // Allows sig matching past end of firmware without checking each time in the inner loop
    fw->buf = malloc((fw->size+32)*4);
    k = fread(fw->buf, 4, fw->size, f);
    fclose(f);
	
    memset(&fw->buf[fw->size],0xff,32*4);
	
    // Find all the valid ranges for checking (skips over large blocks of 0xFFFFFFFF)
    findRanges(fw);

	bprintf("// Camera info:\n");
	
	// Get DRYOS version
	fw->dryos_ver = 0;
	k = find_str(fw, "DRYOS version 2.3, release #");
	if (k == -1)
	{
		bprintf("//   Can't find DRYOS version !!!\n\n");
	}
	else
	{
		fw->dryos_ver = atoi(((char*)&fw->buf[k])+28);
        if (fw->dryos_ver > 52)
    		bprintf("//   DRYOS R%d (%s) *** New DRYOS Version - please update finsig_dryos.c ***\n",fw->dryos_ver,(char*)&fw->buf[k]);
        else
    		bprintf("//   DRYOS R%d (%s)\n",fw->dryos_ver,(char*)&fw->buf[k]);
	}
    
	// Get firmware version info
	k = find_str(fw, "Firmware Ver ");
	if (k == -1)
	{
		bprintf("//   Can't find firmware version !!!\n\n");
	}
	else
	{
		bprintf("//   %s",(char*)&fw->buf[k]);
		bprintf("\n");
	}

	// Get camera name & platformid     ***** UPDATE for new DryOS version *****
	int fsize = -((int)fw->base)/4;
    if (fw->alt_base) fsize = -((int)fw->alt_base)/4;
	int cam_idx = 0;
    int pid_idx = 0;
	switch (fw->dryos_ver)
	{
	case 20:
	case 23:
	case 31:
	case 39: 
        cam_idx = adr2idx(fw,0xFFFE0110); 
        pid_idx = adr2idx(fw,0xFFFE0130); 
        break;
	case 43:
	case 45: 
        cam_idx = adr2idx(fw,0xFFFE00D0);
        pid_idx = adr2idx(fw,0xFFFE0130);
        break;
	case 47: 
        cam_idx = adr2idx(fw,(fw->base==0xFF000000)?0xFFF40170:0xFFFE0170);
        pid_idx = adr2idx(fw,(fw->base==0xFF000000)?0xFFF40040:0xFFFE0040);
        break;
	case 49: 
	case 50: 
	case 51:
	case 52:
        if (fw->alt_base)
        {
            cam_idx = adr2idx(fw,(fw->alt_base==0xFF000000)?0xFFF40190:0xFFFE0170);
            pid_idx = adr2idx(fw,(fw->alt_base==0xFF000000)?0xFFF40040:0xFFFE0040);
            if (idx_valid(fw,cam_idx) && (strncmp((char*)fwadr(fw,cam_idx),"Canon ",6) != 0))
                cam_idx = adr2idx(fw,(fw->alt_base==0xFF000000)?0xFFF40170:0xFFFE0170);
        }
        else
        {
            cam_idx = adr2idx(fw,(fw->base==0xFF000000)?0xFFF40190:0xFFFE0170);
            pid_idx = adr2idx(fw,(fw->base==0xFF000000)?0xFFF40040:0xFFFE0040);
            if (idx_valid(fw,cam_idx) && (strncmp((char*)fwadr(fw,cam_idx),"Canon ",6) != 0))
                cam_idx = adr2idx(fw,(fw->base==0xFF000000)?0xFFF40170:0xFFFE0170);
        }
        break;
	}

    if (fsize > (fw->size + 256))
	{
		bprintf("//   Possible corrupt firmware dump - file size to small for start address 0x%08x\n",fw->base);
		bprintf("//     file size = %.2fMB, should be %.2fMB\n", ((double)fw->size*4.0)/(1024.0*1024.0),((double)fsize*4.0)/(1024.0*1024.0));
	}
	
	strcpy(fw->cam,"Unknown");
	if (idx_valid(fw,cam_idx) && (strncmp((char*)fwadr(fw,cam_idx),"Canon ",6) == 0))
	{
		strcpy(fw->cam,(char*)fwadr(fw,cam_idx));
		bprintf("//   %s\n",fw->cam);
	}
	else
	{
		bprintf("//   Could not find Camera name - possible corrupt firmware dump\n");
	}

	bprintf("\n// Values for makefile.inc\n");
	bprintf("//   PLATFORMOSVER = %d\n",fw->dryos_ver);

    if (idx_valid(fw,pid_idx))
    {
        fw->pid = fwval(fw,pid_idx) & 0xFFFF;
		bprintf("//   PLATFORMID = %d (0x%04x) // Found @ 0x%08x\n",fw->pid,fw->pid,idx2adr(fw,pid_idx));
    }
    else fw->pid = 0;

    // Calc MAXRAMADDR
    if (((fw->buf[0x10] & 0xFFFFFF00) == 0xE3A00000) && (fw->buf[0x11] == 0xEE060F12))
    {
        fw->maxram = (1 << (((fw->buf[0x10] & 0x3E) >> 1) + 1)) - 1;
    }
    else if (((fw->buf[0x14] & 0xFFFFFF00) == 0xE3A00000) && (fw->buf[0x15] == 0xEE060F12))
    {
        fw->maxram = (1 << (((fw->buf[0x14] & 0x3E) >> 1) + 1)) - 1;
    }
    else fw->maxram = 0;

    if (fw->maxram != 0)
		bprintf("//   MAXRAMADDR = 0x%08x\n",fw->maxram);

    // Find MEMISOSTART
    for (k=0; k<100; k++)
    {
        if (isLDR_PC(fw,k) && (LDR2val(fw,k) == 0x1900) && isLDR_PC(fw,k+6))
        {
            uint32_t memisostart = LDR2val(fw,k+6);
		    bprintf("//   MEMISOSTART = 0x%08x\n",memisostart);
        }
    }

    uint32_t ofst = adr2idx(fw,0xFFFF0000);    // Offset of area to find dancing bits
    if (idx_valid(fw,ofst) && isB(fw,ofst) && isLDR_PC(fw,ofst+1))
    {
        // Get KEYSYS value
        ofst = adr2idx(fw,LDR2val(fw,ofst+1));     // Address of firmware encryption key
        if (idx_valid(fw,ofst))
        {
            char *ksys = "? Not found, possible new firmware encryption key.";
            switch (fwval(fw,ofst))
            {
            case 0x70726964:    ksys = "d3   "; break;
            case 0x646C726F:    ksys = "d3enc"; break;
            case 0x774D450B:    ksys = "d4   "; break;
            case 0x80751A95:    ksys = "d4a  "; break;
            case 0x76894368:    ksys = "d4b  "; break;
            case 0x50838EF7:    ksys = "d4c  "; break;
            case 0xCCE4D2E6:    ksys = "d4d  "; break;
            }
            bprintf("//   KEYSYS = %s              // Found @ 0x%08x\n",ksys,idx2adr(fw,ofst));
        }

        // Get NEED_ENCODED_DISKBOOT value
        ofst = ofst + 4; // Address of dancing bits data (try after firmware key)
        if (idx_valid(fw,ofst))
        {
            int fnd = 0, i, j;
            for (i=0; i<VITALY && !fnd; i++)
            {
                fnd = i+1;
                for (j=0; j<8 && fnd; j++)
                {
                    if ((fwval(fw,ofst+j) & 0xFF) != _chr_[i][j])
                    {
                        fnd = 0;
                    }
                }
            }
            if (!fnd)
            {
                // Try before firmware key
                ofst = ofst - 12;
                for (i=0; i<VITALY && !fnd; i++)
                {
                    fnd = i+1;
                    for (j=0; j<8 && fnd; j++)
                    {
                        if ((fwval(fw,ofst+j) & 0xFF) != _chr_[i][j])
                        {
                            fnd = 0;
                        }
                    }
                }
            }
            if (fnd)
            {
        		bprintf("//   NEED_ENCODED_DISKBOOT = %d   // Found @ 0x%08x\n",fnd,idx2adr(fw,ofst));
            }
            else
            {
        		bprintf("//   NEED_ENCODED_DISKBOOT = ? Not found, possible new 'dancing bits' entry needed. // Found @ 0x%08x\n",idx2adr(fw,ofst));
            }
        }
    }

    // DryOS R50/R51/R52 copies a block of ROM to RAM and then uses that copy
    // Need to allow for this in finding addresses
    // Seen on SX260HS
    if (fw->dryos_ver >= 50)
    {
        fw->buf2 = 0;
        fw->base2 = 0;
        fw->size2 = 0;
        
        int i;
        // Try and find ROM address copied, and location copied to
        for (i=3; i<100; i++)
        {
            if (isLDR_PC(fw,i) && isLDR_PC(fw,i+1) && (isLDR_PC(fw,i+2)))
            {
                uint32_t fadr = LDR2val(fw,i);
                uint32_t dadr = LDR2val(fw,i+1);
                uint32_t eadr = LDR2val(fw,i+2);
                if ((fadr > fw->base) && (dadr < fw->base))
                {
                    fw->buf2 = &fw->buf[adr2idx(fw,fadr)];
                    fw->base2 = dadr;
                    fw->base_copied = fadr;
                    fw->size2 = (eadr - dadr) / 4;
                    bprintf("\n// Note, ROM copied to RAM :- from 0x%08x, to 0x%08x, len %d words.\n",fadr,dadr,(eadr-dadr)/4);
                    break;
                }
            }
        }
    }

	bprintf("\n");
}

void fwAddMatch(firmware *fw, uint32_t fadr, int s, int f, int sig)
{
    if ((fadr >= fw->base_copied) && (fadr < (fw->base_copied + fw->size2*4)))
    {
        addMatch(fadr - fw->base_copied + fw->base2,s,f,sig);
    }
    else
    {
        addMatch(fadr,s,f,sig);
    }
}

//------------------------------------------------------------------------------------------------------------

// Master list of functions / addresses to find

#define DONT_EXPORT     1
#define OPTIONAL        2
#define UNUSED          4

typedef struct {
	char		*name;
    int         flags;
	uint32_t	val;
} func_entry;

func_entry  func_names[] =
{
    // Do these first as they are needed to find others
    { "CreateJumptable", UNUSED },
    { "_uartr_req", UNUSED },

    { "AllocateMemory", UNUSED },
    { "AllocateUncacheableMemory" },
    { "Close" },
    { "CreateTask" },
    { "DebugAssert", OPTIONAL },
    { "DeleteDirectory_Fut" },
    { "DeleteFile_Fut" },
    { "DoAFLock" },
    { "EnterToCompensationEVF" },
    { "ExecuteEventProcedure" },
    { "ExitFromCompensationEVF" },
    { "ExitTask" },
    { "ExpCtrlTool_StartContiAE" },
    { "ExpCtrlTool_StopContiAE" },
    { "Fclose_Fut" },
    { "Feof_Fut" },
    { "Fflush_Fut" },
    { "Fgets_Fut" },
    { "Fopen_Fut" },
    { "Fread_Fut" },
    { "FreeMemory", UNUSED },
    { "FreeUncacheableMemory" },
    { "Fseek_Fut" },
    { "Fwrite_Fut" },
    { "GetBatteryTemperature" },
    { "GetCCDTemperature" },
    { "GetCurrentAvValue" },
    { "GetDrive_ClusterSize" },
    { "GetDrive_FreeClusters" },
    { "GetDrive_TotalClusters" },
    { "GetFocusLensSubjectDistance" },
    { "GetFocusLensSubjectDistanceFromLens" },
    { "GetImageFolder", OPTIONAL },
    { "GetKbdState" },
    { "GetMemInfo" },
    { "GetOpticalTemperature" },
    { "GetParameterData" },
    { "GetPropertyCase" },
    { "GetSystemTime" },
    { "GetVRAMHPixelsSize" },
    { "GetVRAMVPixelsSize" },
    { "GetZoomLensCurrentPoint" },
    { "GetZoomLensCurrentPosition" },
    { "GiveSemaphore", OPTIONAL },
    { "IsStrobeChargeCompleted" },
    { "LEDDrive", OPTIONAL },
    { "LocalTime" },
    { "LockMainPower" },
    { "Lseek", UNUSED },
    { "MakeDirectory_Fut" },
    { "MakeSDCardBootable", OPTIONAL },
    { "MoveFocusLensToDistance" },
    { "MoveIrisWithAv", OPTIONAL },
    { "MoveZoomLensWithPoint" },
    { "NewTaskShell", UNUSED },
    { "Open" },
    { "PB2Rec" },
    { "PT_MoveDigitalZoomToWide", OPTIONAL },
    { "PT_MoveOpticalZoomAt", OPTIONAL },
    { "PT_PlaySound" },
    { "PostLogicalEventForNotPowerType" },
    { "PostLogicalEventToUI" },
    { "PutInNdFilter", OPTIONAL },
    { "PutOutNdFilter", OPTIONAL },
    { "Read" },
    { "ReadFastDir" },
    { "Rec2PB" },
    { "RefreshPhysicalScreen" },
    { "Remove", UNUSED },
    { "RenameFile_Fut" },
    { "Restart" },
    { "ScreenLock" },
    { "ScreenUnlock" },
    { "SetAE_ShutterSpeed" },
    { "SetAutoShutdownTime" },
    { "SetCurrentCaptureModeType" },
    { "SetFileAttributes" },
    { "SetFileTimeStamp" },
    { "SetLogicalEventActive" },
    { "SetParameterData" },
    { "SetPropertyCase" },
    { "SetScriptMode" },
    { "SleepTask" },
    { "TakeSemaphore" },
    { "TurnOffBackLight" },
    { "TurnOnBackLight" },
    { "UIFS_WriteFirmInfoToFile" },
    { "UnlockAF" },
    { "UnlockMainPower" },
    { "UnsetZoomForMovie", OPTIONAL },
    { "UpdateMBROnFlash" },
    { "VbattGet" },
    { "Write" },
    { "WriteSDCard" },

    { "_log" },
    { "_log10" },
    { "_pow" },
    { "_sqrt" },
    { "add_ptp_handler" },
    { "apex2us" },
    { "close" },
    { "closedir" },
    { "err_init_task", OPTIONAL },
    { "exmem_alloc" },
    { "exmem_free", OPTIONAL },
    { "free" },

    { "kbd_p1_f" },
    { "kbd_p1_f_cont" },
    { "kbd_p2_f" },
    { "kbd_read_keys" },
    { "kbd_read_keys_r2" },

    { "kbd_pwr_off" },
    { "kbd_pwr_on" },
    { "lseek" },
    { "malloc" },
    { "memcmp" },
    { "memcpy" },
    { "memset" },
    { "mkdir" },
    { "mktime_ext" },
    { "open" },
    { "OpenFastDir" },
    { "qsort" },
    { "rand" },
    { "read", UNUSED },
    { "realloc", OPTIONAL },
    { "reboot_fw_update" },
    { "set_control_event" },
    { "srand" },
    { "stat" },
    { "strcat" },
    { "strchr" },
    { "strcmp" },
    { "strcpy" },
    { "strftime" },
    { "strlen" },
    { "strncmp" },
    { "strncpy" },
    { "strrchr" },
    { "strtol" },
    { "strtolx" },

    { "task_CaptSeq" },
    { "task_ExpDrv" },
    { "task_InitFileModules" },
    { "task_MovieRecord" },
    { "task_PhySw", OPTIONAL },
    { "task_RotaryEncoder", OPTIONAL },
    { "task_TouchPanel", OPTIONAL },

    { "hook_CreateTask" },
    { "hook_CreateTask2" },

    { "time" },
    { "vsprintf" },
    { "write", UNUSED },

    { "EngDrvIn", OPTIONAL|UNUSED },
    { "EngDrvOut", OPTIONAL|UNUSED },
    { "EngDrvRead", OPTIONAL|UNUSED },
    { "EngDrvBits", OPTIONAL|UNUSED },

    // Other stuff needed for finding misc variables - don't export to stubs_entry.S
	{ "GetSDProtect", UNUSED },
	{ "StartRecModeMenu", UNUSED },
	{ "DispCon_ShowBitmapColorBar", UNUSED },
	{ "ResetZoomLens", OPTIONAL|UNUSED },
	{ "ResetFocusLens", OPTIONAL|UNUSED },
	{ "NR_GetDarkSubType", OPTIONAL|UNUSED },
	{ "NR_SetDarkSubType", OPTIONAL|UNUSED },
	{ "SavePaletteData", OPTIONAL|UNUSED },
    { "GUISrv_StartGUISystem", OPTIONAL|UNUSED },

    { 0, 0, 0 }
};

//------------------------------------------------------------------------------------------------------------

// Signature min / max DryOS versions

typedef struct {
	char		*name;
	uint32_t	val;
} sig_stuff;

sig_stuff min_ver[] = {
	{ "ScreenLock", 39 },
	{ "ScreenUnlock", 39 },
	{ "MakeSDCardBootable", 47 },
    { "hook_CreateTask", 51 },
    { "hook_CreateTask2", 51 },
	
	{ 0, 0 }
};

sig_stuff max_ver[] = {
	{ "UpdateMBROnFlash", 45 },
	
	{ 0, 0 }
};

int find_min_ver(const char *name)
{
	int i;
	for (i=0; min_ver[i].name != 0; i++)
	{
		if (strcmp(name,min_ver[i].name) == 0)
		{
			return min_ver[i].val;
		}
	}
	return 0;
}

int find_max_ver(const char *name)
{
	int i;
	for (i=0; max_ver[i].name != 0; i++)
	{
		if (strcmp(name,max_ver[i].name) == 0)
		{
			return max_ver[i].val;
		}
	}
	return 99999;
}

uint32_t find_saved_sig(const char *name)
{
	int i;
	for (i=0; func_names[i].name != 0; i++)
	{
		if (strcmp(name,func_names[i].name) == 0)
		{
			return i;
		}
	}
	return -1;
}

void save_sig(const char *name, uint32_t val)
{
	int i = find_saved_sig(name);
	if (i >= 0)
	{
		func_names[i].val = val;
	}
}

uint32_t get_saved_sig(firmware *fw, const char *name)
{
	int j = find_saved_sig(name);
	if (j >= 0)
	{
		if (func_names[j].val == 0)
		{
			int find_func(const char* name);
			int k1 = find_func(name);
			if (k1 >= 0)
			{
				void find_matches(firmware*,const char*);
				find_matches(fw, name);
				count = 0;
			}
			else
			{
				void find_str_sig_matches(firmware*,const char*);
				find_str_sig_matches(fw, name);
				count = 0;
			}
		}
		if (func_names[j].val == 0)
		{
			j = -1;
		}
	}
	return j;
}

//------------------------------------------------------------------------------------------------------------

// New string / signature matching structure

typedef struct {
    int     type;           // 1 = func*, string, 2 = string, ... string*, func*, 3 = ADR Rx, func, ADR Ry, string, BL, ... string
    char    *name;
    char    *ev_name;
    int     offset;
	int		dryos20_offset;     // ***** UPDATE for new DryOS version *****
	int		dryos23_offset;
	int		dryos31_offset;
	int		dryos39_offset;
	int		dryos43_offset;
	int		dryos45_offset;
	int		dryos47_offset;
	int		dryos49_offset;
	int		dryos50_offset;
	int		dryos51_offset;
	int		dryos52_offset;
} string_sig;

#include "signatures_dryos.h"

uint32_t log_test[] = {
	0x1526E50E, 0x3FDBCB7B, 0
};

uint32_t DeleteDirectory_Fut_test[] = { 0x09400017 };
uint32_t MakeDirectory_Fut_test[]   = { 0x09400015 };
uint32_t RenameFile_Fut_test[]      = { 0x09400013 };

string_sig string_sigs[] = {
	{ 1, "AllocateMemory", "AllocateMemory", 1 },
    { 1, "Close", "Close", 1 },
	{ 1, "CreateTask", "CreateTask", 1 },
	{ 1, "DoAFLock", "PT_DoAFLock", 0x01000002 },
    { 1, "ExitTask", "ExitTask", 1 },
	{ 1, "exmem_alloc", "ExMem.AllocCacheable", 4 },
	{ 1, "exmem_free", "ExMem.FreeCacheable", 0x01000003 },
    { 1, "Fclose_Fut", "Fclose_Fut", 1 },
    { 1, "Feof_Fut", "Feof_Fut", 1 },
    { 1, "Fflush_Fut", "Fflush_Fut", 1 },
    { 1, "Fgets_Fut", "Fgets_Fut", 1 },
    { 1, "Fopen_Fut", "Fopen_Fut", 1 },
    { 1, "Fread_Fut", "Fread_Fut", 1 },
	{ 1, "FreeMemory", "FreeMemory", 1 },
    { 1, "Fseek_Fut", "Fseek_Fut", 1 },
    { 1, "Fwrite_Fut", "Fwrite_Fut", 1 },
	{ 1, "GetParameterData", "PTM_RestoreUIProperty", 0xF0000004 },
	{ 1, "GetPropertyCase", "PT_GetPropertyCaseString", 1 },
	{ 1, "GetPropertyCase", "PT_GetPropertyCaseInt", 0x0100000F },
	{ 1, "GetPropertyCase", "GetPropertyCase", 0x0100000F },
	{ 1, "GetSDProtect", "GetSDProtect", 1 },
	{ 1, "GetSystemTime", "GetSystemTime", 1 },
	{ 1, "LEDDrive", "LEDDrive", 1 },
    { 1, "LockMainPower", "LockMainPower", 1 },
    { 1, "Lseek", "Lseek", 1 },
    { 1, "lseek", "Lseek", 1 },
    { 1, "memcpy", "memcpy", 1 },
    { 1, "memcmp", "memcmp", 1 },
    { 1, "memset", "memset", 1 },
    { 1, "NewTaskShell", "NewTaskShell", 1 },
    { 1, "Open", "Open", 1 },
    { 1, "PostLogicalEventToUI", "PostLogicalEventToUI", 1 },
    { 1, "PostLogicalEventForNotPowerType", "PostLogicalEventForNotPowerType", 1 },
    { 1, "Read", "Read", 1 },
    { 1, "read", "Read", 1 },
	{ 1, "RefreshPhysicalScreen", "RefreshPhysicalScreen", 1 },
    { 1, "SetAutoShutdownTime", "SetAutoShutdownTime", 1 },
    { 1, "SetCurrentCaptureModeType", "SetCurrentCaptureModeType", 1 },
    { 1, "SetLogicalEventActive", "UiEvnt_SetLogicalEventActive", 1 },
    { 1, "SetParameterData", "PTM_BackupUIProperty", 1 },
	{ 1, "SetPropertyCase", "PT_SetPropertyCaseInt", 0x01000003 },
	{ 1, "SetPropertyCase", "SetPropertyCase", 0x01000004 },
	{ 1, "SetScriptMode", "SetScriptMode", 1 },
    { 1, "SleepTask", "SleepTask", 1 },
    { 1, "strcmp", "strcmp", 0 },
    { 1, "strcpy", "strcpy", 1 },
    { 1, "strlen", "strlen", 1 },
    { 1, "strtol", "atol", 3 },
    { 1, "TakeSemaphore", "TakeSemaphore", 1 },
    { 1, "UIFS_WriteFirmInfoToFile", "UIFS_WriteFirmInfoToFile", 1 },
	{ 1, "UnlockAF", "PT_UnlockAF", 0x01000002 },
    { 1, "UnlockMainPower", "UnlockMainPower", 1 },
    { 1, "VbattGet", "VbattGet", 1 },
    { 1, "Write", "Write", 1 },
    { 1, "write", "Write", 1 },
    { 1, "GUISrv_StartGUISystem", "GUISrv_StartGUISystem", 1 },

    { 2, "GetBatteryTemperature", "GetBatteryTemperature", 1 },
    { 2, "GetCCDTemperature", "GetCCDTemperature", 1 },
    { 2, "GetOpticalTemperature", "GetOpticalTemperature", 1 },
    { 2, "GetFocusLensSubjectDistance", "GetCurrentTargetDistance", 1 },
    { 2, "GetZoomLensCurrentPoint", "GetZoomLensCurrentPoint", 1 },
    { 2, "GetZoomLensCurrentPosition", "GetZoomLensCurrentPosition", 1 },
    { 2, "MoveFocusLensToDistance", "MoveFocusLensToDistance", 1 },
    { 2, "MoveZoomLensWithPoint", "MoveZoomLensWithPoint", 1 },
    { 2, "GetCurrentAvValue", "GetCurrentAvValue", 1 },
	{ 2, "PT_MoveOpticalZoomAt", "PT_MoveOpticalZoomAt", 1 },
	{ 2, "PT_MoveOpticalZoomAt", "SS.MoveOpticalZoomAt", 1 },
	{ 2, "PT_MoveDigitalZoomToWide", "PT_MoveDigitalZoomToWide", 1 },
	{ 2, "PT_MoveDigitalZoomToWide", "SS.MoveDigitalZoomToWide", 1 },
	{ 2, "MoveIrisWithAv", "MoveIrisWithAv", 1},
    { 2, "PutInNdFilter", "TurnOnNdFilter", 1 },
    { 2, "PutOutNdFilter", "TurnOffNdFilter", 1 },
    { 2, "PutInNdFilter", "PutInNdFilter", 1 },
    { 2, "PutOutNdFilter", "PutOutNdFilter", 1 },
	{ 2, "IsStrobeChargeCompleted", "EF.IsChargeFull", 1 },
	{ 2, "GetPropertyCase", "PT_GetPropertyCaseInt", 0x01000012 },
	{ 2, "SetPropertyCase", "PT_SetPropertyCaseInt", 0x01000008 },
	{ 2, "SetPropertyCase", "PT_SetPropertyCaseInt", 0x01000009 },
	{ 2, "UnlockAF", "PT_UnlockAF", 0x01000002 },
	{ 2, "UnlockAF", "SS.UnlockAF", 0x01000002 },
	{ 2, "DoAFLock", "PT_DoAFLock", 0x01000002 },
	{ 2, "DoAFLock", "SS.DoAFLock", 0x01000002 },
	{ 2, "GetSystemTime", "PT_GetSystemTime", 0x01000003 },
	{ 2, "PT_PlaySound", "PT_PlaySound", 0x01000005 },
	{ 2, "StartRecModeMenu", "StartRecModeMenu", 1 },
	{ 2, "GetSDProtect", "GetSDProtect", 1 },
	{ 2, "DispCon_ShowBitmapColorBar", "DispCon_ShowBitmapColorBar", 1 },
	{ 2, "SetAE_ShutterSpeed", "SetAE_ShutterSpeed", 1 },
    { 2, "ResetZoomLens", "ResetZoomLens", 1 },
    { 2, "ResetFocusLens", "ResetFocusLens", 1 },
    { 2, "NR_GetDarkSubType", "NR_GetDarkSubType", 1 },
    { 2, "NR_GetDarkSubType", "NRTBL.GetDarkSubType", 1 },
    { 2, "NR_SetDarkSubType", "NR_SetDarkSubType", 1 },
    { 2, "NR_SetDarkSubType", "NRTBL.SetDarkSubType", 1 },
    { 2, "SavePaletteData", "SavePaletteData", 1 },
    { 2, "GetVRAMHPixelsSize", "GetVRAMHPixelsSize", 1 },
    { 2, "GetVRAMVPixelsSize", "GetVRAMVPixelsSize", 1 },
    { 2, "EngDrvIn", "EngDrvIn", 2 },
    { 2, "EngDrvOut", "EngDrvOut", 0x01000005 },
    { 2, "EngDrvRead", "EngDrvRead", 2 },
    { 2, "EngDrvBits", "EngDrvBits", 0x01000007 },
	{ 2, "exmem_alloc", "ExMem.AllocCacheable", 4 },
	{ 2, "exmem_free", "ExMem.FreeCacheable", 0x01000003 },

	{ 3, "AllocateMemory", "AllocateMemory", 1 },
	{ 3, "FreeMemory", "FreeMemory", 1 },
    { 3, "PostLogicalEventToUI", "PostLogicalEventToUI", 1 },
    { 3, "PostLogicalEventForNotPowerType", "PostLogicalEventForNotPowerType", 1 },
    { 3, "LockMainPower", "LockMainPower", 1 },
    { 3, "UnlockMainPower", "UnlockMainPower", 1 },
    { 3, "SetAutoShutdownTime", "SetAutoShutdownTime", 1 },
    { 3, "NewTaskShell", "NewTaskShell", 1 },
    { 3, "VbattGet", "VbattGet", 1 },
	{ 3, "LEDDrive", "LEDDrive", 1 },
	{ 3, "SetPropertyCase", "PT_SetPropertyCaseInt", 0x01000003 },
	{ 3, "UnlockAF", "PT_UnlockAF", 0x01000002 },
	{ 3, "DoAFLock", "PT_DoAFLock", 0x01000002 },
    { 3, "UIFS_WriteFirmInfoToFile", "UIFS_WriteFirmInfoToFile", 1 },
	{ 3, "PT_MoveOpticalZoomAt", "PT_MoveOpticalZoomAt", 1 },
	{ 3, "PT_MoveDigitalZoomToWide", "PT_MoveDigitalZoomToWide", 1 },
	{ 3, "PT_PlaySound", "PT_PlaySound", 1 },
	{ 3, "exmem_alloc", "ExMem.AllocCacheable", 4 },
	{ 3, "exmem_free", "ExMem.FreeCacheable", 0x01000003 },
	{ 3, "GetSDProtect", "GetSDProtect", 1 },

    { 4, "TurnOnBackLight", "TurnOnBackLight", 1 },
    { 4, "TurnOffBackLight", "TurnOffBackLight", 1 },
    { 4, "EnterToCompensationEVF", "SSAPI::EnterToCompensationEVF", 1 },
    { 4, "EnterToCompensationEVF", "ExpComp On", 1 },
    { 4, "EnterToCompensationEVF", "ExpOn", 1 },
    { 4, "ExitFromCompensationEVF", "SSAPI::ExitFromCompensationEVF", 1 },
    { 4, "ExitFromCompensationEVF", "ExpComp Off", 1 },
    { 4, "ExitFromCompensationEVF", "ExpOff", 1 },
    { 4, "PB2Rec", "AC:PB2Rec", 1 },
    { 4, "PB2Rec", "AC:PB2Rec", 6 },
    { 4, "PB2Rec", "AC:PB2Rec", 11 },
    { 4, "Rec2PB", "AC:Rec2PB", 1 },
	{ 4, "RefreshPhysicalScreen", "ScreenUnLock", 1 },
	{ 4, "RefreshPhysicalScreen", "ScreenUnLock", 7 },
	{ 4, "RefreshPhysicalScreen", "ScreenUnLock", 15 },
	{ 4, "RefreshPhysicalScreen", "Reduce ScreenUnLock", 5 },
	{ 4, "RefreshPhysicalScreen", "Window:IneffectiveLockPhysicalScreen", 8 },
	{ 4, "UnsetZoomForMovie", "ZoomCon_UnsetZoomForMovie", 1 },
	{ 4, "ExpCtrlTool_StopContiAE", "StopContiAE", 9 },
	{ 4, "ExpCtrlTool_StopContiAE", "StopContiAE", 10 },
	{ 4, "ExpCtrlTool_StartContiAE", "StartContiAE", 9 },
	{ 4, "ExpCtrlTool_StartContiAE", "StartContiAE", 10 },
    { 4, "ExecuteEventProcedure", "Can not Execute ", 14 },

    { 5, "UIFS_WriteFirmInfoToFile", "UIFS_WriteFirmInfoToFile", 1 },
    { 5, "CreateTask", "CreateTask", 1 },
    { 5, "hook_CreateTask", "CreateTask", 1 },
    { 5, "ExitTask", "ExitTask", 1 },
    { 5, "SleepTask", "SleepTask", 1 },
	//																	 R20   R23   R31   R39   R43   R45   R47   R49   R50   R51   R52
	{ 5, "UpdateMBROnFlash", "MakeBootDisk", 0x01000003,				  11,   11,   11,   11,   11,   11,    1,    1,    1,    1,    1 },
	{ 5, "MakeSDCardBootable", "MakeBootDisk", 0x01000003,				   1,    1,    1,    1,    1,    1,    8,    8,    8,    8,    8 },

    { 6, "Restart", "Bye", 0 },
	{ 6, "GetImageFolder", "GetCameraObjectTmpPath ERROR[ID:%lx] [TRY:%lx]\n", 0 },
    { 6, "reboot_fw_update", "FirmUpgrade.c", 0 },
	
	{ 7, "hook_CreateTask2", "PhySw", 0x01000001 },

	{ 8, "WriteSDCard", "Mounter.c", 0 }, 

    // Ensure ordering in func_names is correct for dependencies here
	//																	 R20   R23   R31   R39   R43   R45   R47   R49   R50   R51   R52
	{ 9, "kbd_p1_f", "task_PhySw", 0,							           5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5 },
	{ 9, "kbd_p2_f", "task_PhySw", 0,							           7,    7,    7,    7,    7,    7,    7,    7,    7,    7,    7 },
	{ 9, "kbd_read_keys", "kbd_p1_f", 0,						           2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2 },
	{ 9, "kbd_p1_f_cont", "kbd_p1_f", -1,								   3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3 },
    { 9, "kbd_read_keys_r2", "kbd_read_keys", 0,                          11,   11,   11,   11,   11,   11,   11,   11,   11,   11,   11 },
    { 9, "GetKbdState", "kbd_read_keys", 0,                                8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8 },
	{ 9, "GetKbdState", "kbd_read_keys", 0,                                9,    9,    9,    9,    9,    9,    9,    9,    9,    9,    9 },
	{ 9, "strtolx", "strtol", 0,                                           1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1 },
    { 9, "mkdir", "MakeDirectory_Fut", 0x01000001,                        17,   17,   17,   17,   17,   17,   17,   17,   17,   17,   17 },
    { 9, "mkdir", "MakeDirectory_Fut", 0x01000002,                        17,   17,   17,   17,   17,   17,   17,   17,   17,   17,   17 },
    { 9, "time", "MakeDirectory_Fut", 0,                                  12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12 },
    { 9, "stat", "_uartr_req", 0,                                          0,    0,    0,    4,    4,    4,    4,    4,    4,    4,    4 },
	
	{ 10, "task_CaptSeq", "CaptSeqTask", 1 },
	{ 10, "task_ExpDrv", "ExpDrvTask", 1 },
	{ 10, "task_InitFileModules", "InitFileModules", 1 },
	{ 10, "task_MovieRecord", "MovieRecord", 1 },
	{ 10, "task_PhySw", "PhySw", 1 },
	{ 10, "task_RotaryEncoder", "RotaryEncoder", 1 },
	{ 10, "task_RotaryEncoder", "RotarySw", 1 },
	{ 10, "task_TouchPanel", "TouchPanel", 1 },

	//																	 R20   R23   R31   R39   R43   R45   R47   R49   R50   R51   R52
	{ 11, "DebugAssert", "\nAssert: File %s Line %d\n", 0,				   5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5 },
	{ 11, "err_init_task", "\n-- %s() error in init_task() --", 0,		   2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2 },
	{ 11, "set_control_event", "Button:0x%08X:%s", 0x01000001,			  14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14 },
	{ 11, "set_control_event", "Button:0x%08X:%s", 0x01000001,			  15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 },
	{ 11, "set_control_event", "Button:0x%08X:%s", 0x01000001,			  20,   20,   20,   20,   20,   20,   19,   20,   20,   20,   20 },
	{ 11, "_log", (char*)log_test, 0x01000001,							   1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1 },
	{ 11, "_uartr_req", "A/uartr.req", 0,							       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3 },
	
	//																	 R20   R23   R31   R39   R43   R45   R47   R49   R50   R51   R52
	{ 12, "DeleteFile_Fut", "DeleteFile_Fut", 1,						0x38, 0x38, 0x4C, 0x4C, 0x4C, 0x54, 0x54, 0x54, 0x00, 0x00, 0x00 },
	{ 12, "AllocateUncacheableMemory", "AllocateUncacheableMemory", 1, 	0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x34, 0x34, 0x34, 0x4C, 0x4C, 0x4C },
	{ 12, "FreeUncacheableMemory", "FreeUncacheableMemory", 1, 			0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x38, 0x38, 0x50, 0x50, 0x50 },
	{ 12, "free", "free", 1,											0x28, 0x28, 0x28, 0x28, 0x28, 0x30, 0x30, 0x30, 0x48, 0x48, 0x48 },
	{ 12, "malloc", "malloc", 0x01000003,								0x24, 0x24, 0x24, 0x24, 0x24, 0x2C, 0x2C, 0x2C, 0x44, 0x44, 0x44 },
	{ 12, "TakeSemaphore", "TakeSemaphore", 1,							0x14, 0x14, 0x14, 0x14, 0x14, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C },
	{ 12, "GiveSemaphore", "GiveSemaphore", 1,							0x18, 0x18, 0x18, 0x18, 0x18, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 },
	{ 12, "_log10", "_log10", 0x01000006,							   0x278,0x280,0x280,0x284,0x294,0x2FC,0x2FC,0x31C,0x354,0x35C,0x35C },
	{ 12, "_log10", "_log10", 0x01000006,							   0x000,0x278,0x27C,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000 },
	{ 12, "_log10", "_log10", 0x01000006,							   0x000,0x000,0x2C4,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000 },
	
	{ 13, "strftime", "Sunday", 1 },
	
	{ 14, "_pow", "_pow", 0 },

    { 15, "LocalTime", "%04d:%02d:%02d %02d:%02d:%02d", 0x01000001 },
    { 15, "GetMemInfo", "Malloc Information\n", 0x01000001 },
    { 15, "GetMemInfo", "Malloc Information (%s type)\n", 0x01000001 },
    { 15, "vsprintf", "\nCPrintf Size Over!!", 0x01000001 },
    { 15, "ReadFastDir", "ReadFast_ERROR\n", 0x01000001 },
    { 15, "OpenFastDir", "OpenFastDir_ERROR\n", 0x01000001 },
    { 15, "realloc", "fatal error - scanner input buffer overflow", 0x01000001 },

    { 16, "DeleteDirectory_Fut", (char*)DeleteDirectory_Fut_test, 0x01000001 },
    { 16, "MakeDirectory_Fut", (char*)MakeDirectory_Fut_test, 0x01000001 },
    { 16, "RenameFile_Fut", (char*)RenameFile_Fut_test, 0x01000001 },
	
    { 0, 0, 0, 0 }
};

int find_func(const char* name)
{
	int i;
	for (i=0; func_list[i].name != 0; i++)
	{
		if (strcmp(name, func_list[i].name) == 0)
		{
			return i;
		}
	}
	return -1;
}

int dryos_offset(firmware *fw, string_sig *sig)
{
	switch (fw->dryos_ver)
	{
	case 20:	return sig->dryos20_offset;
	case 23:	return sig->dryos23_offset;
	case 31:	return sig->dryos31_offset;
	case 39:	return sig->dryos39_offset;
	case 43:	return sig->dryos43_offset;
	case 45:	return sig->dryos45_offset;
	case 47:	return sig->dryos47_offset;
	case 49:	return sig->dryos49_offset;
	case 50:	return sig->dryos50_offset;
	case 51:	return sig->dryos51_offset;
	case 52:	return sig->dryos52_offset;
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------

int find_str_ref(firmware *fw, char *str)
{
	int k = find_str(fw, str);
	if (k >= 0)
	{
		uint32_t sadr = idx2adr(fw,k);		// string address
		for (k=0; k<fw->size; k++)
		{
			if (isADR_PC_cond(fw,k) && (ADR2adr(fw,k) == sadr))
			{
				return k;
			}
            else if (isLDR_PC_cond(fw,k) && (LDR2val(fw,k) == sadr))
            {
                return k;
            }
		}
	}
	return -1;
}

int find_nxt_str_ref(firmware *fw, int str_adr, int ofst)
{
	if (str_adr >= 0)
	{
        int k;
		uint32_t sadr = idx2adr(fw,str_adr);		// string address
		for (k=ofst+1; k<fw->size; k++)
		{
			if (isADR_PC_cond(fw,k) && (ADR2adr(fw,k) == sadr))
			{
				return k;
			}
            else if (isLDR_PC_cond(fw,k) && (LDR2val(fw,k) == sadr))
            {
                return k;
            }
		}
	}
	return -1;
}

//------------------------------------------------------------------------------------------------------------

int find_BL(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if (isBL(fw,k))
    {
        int n = idxFollowBranch(fw, k, 0x01000001);
        if (n == v1)
            return k;
    }
    return 0;
}

int find_B(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if (isB(fw,k))
    {
        int n = idxFollowBranch(fw, k, 0x00000001);
        if (n == v1)
            return k;
    }
    return 0;
}

//------------------------------------------------------------------------------------------------------------

int search_fw(firmware *fw, int (*func)(firmware*, int, uint32_t, uint32_t), uint32_t v1, uint32_t v2, int len)
{
    BufRange *p = fw->br;
    while (p)
    {
        int k;
        for (k = p->off; k <= p->off + p->len - len; k++)
        {
            int rv = func(fw,k,v1,v2);
            if (rv != 0)
                return rv;
        }
        p = p->next;
    }
    return 0;
}

int search_fw_bytes(firmware *fw, int (*func)(firmware*, int))
{
    BufRange *p = fw->br;
    while (p)
    {
        int k;
        for (k = p->off*4; k < (p->off + p->len)*4; k++)
        {
            if (func(fw,k))
                return 1;
        }
        p = p->next;
    }
    return 0;
}

int search_saved_sig(firmware *fw, char *sig, int (*func)(firmware*, int, int), int v, int ofst, int len)
{
    int k = get_saved_sig(fw, sig);
    if (k >= 0)
	{
        int idx = adr2idx(fw, func_names[k].val);
        for (k=idx+ofst; k<idx+ofst+len; k++)
        {
            int rv = func(fw, k, v);
            if (rv)
                return rv;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------------------------------------

// Loop through firmware looking for instances of a sig string
// For each one found call the check_match function to see if it matches the sig
// Return 1 0 match found, else return 0
int fw_string_process(firmware *fw, string_sig *sig, int (*check_match)(firmware *fw, string_sig *sig, int j), int inc_eos)
{
    int nlen = strlen(sig->ev_name);
	uint32_t nm0 = *((uint32_t*)sig->ev_name);
    uint32_t *p;
	int j;
    BufRange *br;

	for (br = fw->br; br != 0; br = br->next)
    {
    	for (p = br->p, j = br->off; j < br->off+br->len-nlen/4; p++, j++)
	    {
		    if ((nm0 == *p) && (memcmp(p+1,sig->ev_name+4,nlen-4+inc_eos) == 0))
            {
                if (check_match(fw,sig,j))
                    return 1;
            }
        }
    }

    return 0;
}

int fw_string_process_unaligned(firmware *fw, string_sig *sig, int (*check_match)(firmware *fw, string_sig *sig, int j))
{
    int nlen = strlen(sig->ev_name);
    char *p;
	int j;
    BufRange *br;

	for (br = fw->br; br != 0; br = br->next)
    {
    	for (p = (char*)br->p, j = 0; j < br->len*4-nlen; p++, j++)
	    {
		    if (strcmp(p,sig->ev_name) == 0)
            {
                if (check_match(fw,sig,j+br->off*4))
                    return 1;
            }
        }
    }

    return 0;
}

int fw_process(firmware *fw, string_sig *sig, int (*check_match)(firmware *fw, string_sig *sig, int j))
{
    uint32_t *p;
	int j;
    BufRange *br;

	for (br = fw->br; br != 0; br = br->next)
    {
    	for (p = br->p, j = br->off; j < br->off+br->len; p++, j++)
	    {
            if (check_match(fw,sig,j))
                return 1;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------------------------------------

// New string / signature matching functions

// Sig pattern:
//		Function pointer	-	DCD	func
//		String				-	DCB	"func"
int match_strsig1(firmware *fw, string_sig *sig, int j)
{
    uint32_t fadr = fwval(fw,j-1);  // function address (*padr)
    if (idx_valid(fw,adr2idx(fw,fadr)))
    {
       	if (sig->offset > 1) fadr = followBranch(fw, fadr, 1);
        fadr = followBranch2(fw, fadr, sig->offset);
        //fprintf(stderr,"%s %08x\n",curr_name,fadr);
        fwAddMatch(fw,fadr,32,0,101);
        return 1;
    }
	return 0;
}

// Sig pattern:
//		String pointer		-		DCD	str
//		Function pointer	-		DCD	func
//				...
//		String				-	str	DCB	"func"
int match_strsig2a(firmware *fw, int k, uint32_t sadr, uint32_t offset)
{
    if (fwval(fw,k) == sadr)                // pointer to string?
    {
        uint32_t fadr = fwval(fw,k+1);      // function address (*padr)
        if (idx_valid(fw,adr2idx(fw,fadr)))
        {
            uint32_t bfadr = followBranch2(fw, fadr, offset);
			if ((offset <= 1) || (bfadr != fadr))
			{
				//fprintf(stderr,"%s %08x\n",curr_name,fadr);
				fwAddMatch(fw,bfadr,32,0,102);
				return 1;
			}
        }
    }
    return 0;
}
int match_strsig2(firmware *fw, string_sig *sig, int j)
{
    // Note - 'j' is offset in firmware not instruction index (called from fw_string_process_unaligned)
    return search_fw(fw, match_strsig2a, fw->base + j, sig->offset, 2);
}

// Sig pattern:
//		Load Func Address	-	ADR	Rx, func
//		Load String Address	-	ADR	Rx, "func"
//		Branch				-	BL
//				...
//		String				-	DCB	"func"
int match_strsig3a(firmware *fw, int k, uint32_t sadr, uint32_t offset)
{
	if (isADR_PC(fw,k+1) &&    // ADR ?
		isBorBL(fw,k+2))       // B or BL ?
	{
		uint32_t padr = ADR2adr(fw,k+1);
		if (padr == sadr)
		{
			int j2 = k;
			int found = 0;
			if (isADR_PC(fw,k)) // ADR ?
				found = 1;
			else
			{
				for (j2 = k-2; j2 >= 0 && j2 >= k-4096; j2--)
				{
					if (isADR_PC(fw,j2) &&  // ADR ?
						isB(fw,j2+1))       // B
					{
						uint32_t fa = idx2adr(fw,j2+1);
						fa = followBranch(fw,fa,1);
						if (adr2idx(fw,fa) == k+1)
						{
							found = 1;
							break;
						}
					}
				}
			}
			if (found)
			{
				uint32_t fadr = ADR2adr(fw,j2);
				if (offset > 1) fadr = followBranch(fw, fadr, 1);
				fadr = followBranch2(fw, fadr, offset);
				//fprintf(stderr,"%s %08x\n",curr_name,fadr);
				fwAddMatch(fw,fadr,32,0,103);
				return 1;
			}
		}
	}
    return 0;
}
int match_strsig3(firmware *fw, string_sig *sig, int j)
{
    return search_fw(fw, match_strsig3a, idx2adr(fw,j), sig->offset, 3);
}

// Sig pattern:
//		Save Regs			-	STMFD
//				... (ofst)
//		Load String Address	-	ADR	Rx, "func"
//				...
//		String				-	DCB	"func"
int match_strsig4a(firmware *fw, int k, uint32_t sadr, uint32_t offset)
{
	if (isSTMFD(fw,k) &&        // STMFD
		isADR_PC(fw,k+offset))  // ADR ?
	{
		uint32_t padr = ADR2adr(fw,k+offset);
		if (padr == sadr)
		{
			uint32_t fadr = idx2adr(fw,k);
			//fprintf(stderr,"%s %08x\n",curr_name,fadr);
			fwAddMatch(fw,fadr,32,0,104);
			return 1;
		}
	}
    return 0;
}
int match_strsig4(firmware *fw, string_sig *sig, int j)
{
    return search_fw(fw, match_strsig4a, idx2adr(fw,j), sig->offset, sig->offset+1);
}

// Sig pattern:
//		Load Func Address	-	LDR	Rx, =func
//		Load String Address	-	xDR	Rx, "func"  (LDR or ADR)
//		Branch				-	BL
//				...
//		String				-	DCB	"func"
static int dryos_ofst;
int match_strsig5a(firmware *fw, int k, uint32_t sadr, uint32_t offset)
{
	if ((isADR_PC(fw,k+1) || isLDR_PC(fw,k+1)) &&   // LDR or ADR ?
		isBorBL(fw,k+2))                            // B or BL ?
	{
		uint32_t padr;
		if (isLDR_PC(fw,k+1))                       // LDR ?
            padr = LDR2val(fw,k+1);
		else
            padr = ADR2adr(fw,k+1);
		if (padr == sadr)
		{
			int j2 = k;
			int found = 0;
			if (isLDR_PC(fw,k))                     // LDR ?
				found = 1;
			else
			{
				for (j2 = k-2; j2 >= 0 && j2 >= k-4096; j2--)
				{
					if (isLDR_PC(fw,j2) &&  // LDR ?
						isB(fw,j2+1))       // B
					{
						if (idxFollowBranch(fw,j2+1,1) == k+1)
						{
							found = 1;
							break;
						}
					}
				}
			}
			if (found)
			{
				uint32_t fadr = LDR2val(fw,j2);
				if (offset > 1) fadr = followBranch(fw, fadr, 1);
				fadr = followBranch2(fw, fadr, offset);
                if (dryos_ofst != 0)
                {
                    uint32_t fadr2 = followBranch(fw, fadr, dryos_ofst);
                    if (fadr == fadr2) return 0;
                    fadr = fadr2;
                }
				//fprintf(stderr,"%s %08x\n",curr_name,fadr);
				fwAddMatch(fw,fadr,32,0,105);
				return 1;
			}
		}
	}
    return 0;
}
int match_strsig5(firmware *fw, string_sig *sig, int j)
{
    dryos_ofst = dryos_offset(fw,sig);
    return search_fw(fw, match_strsig5a, idx2adr(fw,j), sig->offset, 3);
}

// Sig pattern:
//	Function immediately preceeding string
int match_strsig6(firmware *fw, string_sig *sig, int j)
{
	int j1 = find_inst_rev(fw, isSTMFD_LR, j-1, j-1);
    if (j1 > 0)
	{
		uint32_t fadr = idx2adr(fw,j1);
		//fprintf(stderr,"%s %08x\n",curr_name,fadr);
		fwAddMatch(fw,fadr,32,0,106);
		return 1;
	}

	return 0;
}

// Sig pattern:
//		Str ref -	xDR Rx, =str_ptr
//			...
//				    BL	func
//			...
//		String		DCB "str"
int match_strsig7a(firmware *fw, int k, uint32_t sadr, uint32_t offset)
{
    if (isADR_PC_cond(fw,k) || isLDR_PC_cond(fw,k)) // LDR or ADR ?
    {
	    uint32_t padr;
	    if (isLDR_PC_cond(fw,k)) // LDR ?
            padr = LDR2val(fw,k);
	    else
            padr = ADR2adr(fw,k);
	    if (padr == sadr)
	    {
		    int j2 = find_inst(fw, isBL, k+1, 10);
            if (j2 > 0)
		    {
			    uint32_t fa = idx2adr(fw,j2);
			    fa = followBranch2(fw,fa,offset);
			    //fprintf(stderr,"%s %08x\n",curr_name,fa);
			    fwAddMatch(fw,fa,32,0,107);
			    return 1;
		    }
	    }
    }
    return 0;
}
int match_strsig7(firmware *fw, string_sig *sig, int j)
{
    return search_fw(fw, match_strsig7a, idx2adr(fw,j), sig->offset, 2);
}

// Sig pattern:
//		Special case for WriteSDCard
int ofst;
int match_strsig8(firmware *fw, string_sig *sig, int j)
{
	int j1;
	for (j1=j-2; j1<j+8; j1++)
	{
		uint32_t fadr = idx2adr(fw,j1);
		if (fwval(fw,j1) >= fw->base)   // pointer ??
		{
			int j2;
			for (j2=j1-1; j2>=j1-1000 && j2>=0; j2--)
			{
				if (isLDR_PC(fw,j2) && (LDR2adr(fw,j2) == fadr))	// LDR ?
				{
					if ((isSTR(fw,j2+1) && ((fwval(fw,j2+1) & 0xfff) == ofst)) ||    // STR ?
						(isSTR(fw,j2+2) && ((fwval(fw,j2+2) & 0xfff) == ofst)))      // STR ?
					{
						fadr = fwval(fw,j1);
						if (idx_valid(fw,adr2idx(fw,fadr)))
						{
							//fprintf(stderr,"%s %08x\n",curr_name,fadr);
							fwAddMatch(fw,fadr,32,0,108);
							return 1;
						}
					}
				}
			}
		}
	}

	return 0;
}
int find_strsig8(firmware *fw, string_sig *sig)
{
	uint32_t fadr = 0;
	int srch = 20;

	// Find "UpdateMBROnFlash" code
	int j = get_saved_sig(fw,"UpdateMBROnFlash");
	if (j >= 0)
	{
		fadr = func_names[j].val;
	}
	else
	{
		j = get_saved_sig(fw,"MakeSDCardBootable");
		if (j >= 0)
		{
			fadr = func_names[j].val;
			srch = 32;
		}
	}
	
	if (fadr == 0) return 0;
	
	int idx = adr2idx(fw, fadr);
	ofst = -1;
	
	for (j=idx+srch; j<idx+srch+12; j++)
	{
		if (isLDR(fw,j) && isLDR(fw,j+1) && isLDR(fw,j+2))
		{
			ofst = (fwval(fw,j) & 0xfff) + (fwval(fw,j+1) & 0xfff) + (fwval(fw,j+2) & 0xfff);
			break;
		}
	}
	
	if (ofst == -1) return 0;

    return fw_string_process(fw, sig, match_strsig8, 1);
}

// Sig pattern:
//		Func is B/BL @ offset from previous found func
//			prev_func
//				... (offset)
//				BL	func
int find_strsig9(firmware *fw, string_sig *sig)
{
	int j = get_saved_sig(fw,sig->ev_name);
	if (j >= 0)
	{
		if (func_names[j].val != 0)
		{			
			int ofst = dryos_offset(fw, sig);
            uint32_t fadr = followBranch(fw, func_names[j].val+ofst*4, 0xF1000001);
			if ((sig->offset == -1) || (fadr != func_names[j].val+ofst*4))
			{
                uint32_t fadr2 = fadr;
                if (sig->offset != -1) fadr2 = followBranch2(fw, fadr2, sig->offset);
                if ((sig->offset <= 0) || (fadr2 != fadr))
                {
    				//fprintf(stderr,"%s %08x\n",curr_name,fadr);
	    			fwAddMatch(fw,fadr2,32,0,109);
		    		return 1;
                }
			}
		}
	}

	return 0;
}

// Sig pattern:
//		Load Func Address	-	LDR	R3, =func	- these four may occur in any order ?
//		Load String Address	-	ADR	R0, "func"	
//		Load value			-	MOV R2, x		
//		Load value			-	MOV R1, y		
//		Branch				-	BL
//				...
//		String				-	DCB	"func"
int match_strsig10(firmware *fw, string_sig *sig, int j)
{
	uint32_t sadr = idx2adr(fw,j);        // string address
	int j1;
	for (j1 = j-5; j1 >= 0; j1--)
	{
		if ((isADR(fw,j1)   || isLDR(fw,j1))   &&   // LDR, ADR or MOV
			(isADR(fw,j1+1) || isLDR(fw,j1+1)) &&   // LDR, ADR or MOV
			(isADR(fw,j1+2) || isLDR(fw,j1+2)) &&   // LDR, ADR or MOV
			(isADR(fw,j1+3) || isLDR(fw,j1+3)) &&   // LDR, ADR or MOV
			isBorBL(fw,j1+4))                       // B or BL ?
		{
			uint32_t padr;
			padr = ADR2adr(fw,j1+0);
			if (padr != sadr) padr = ADR2adr(fw,j1+1);
			if (padr != sadr) padr = ADR2adr(fw,j1+2);
			if (padr != sadr) padr = ADR2adr(fw,j1+3);
			if (padr == sadr)
			{
				uint32_t fadr = 0;
				if      (isLDR_PC(fw,j1)   && (fwRd(fw,j1)   == 3)) fadr = LDR2val(fw,j1);      // R3 ?
				else if (isADR_PC(fw,j1)   && (fwRd(fw,j1)   == 3)) fadr = ADR2adr(fw,j1);      // R3 ?
				else if (isLDR_PC(fw,j1+1) && (fwRd(fw,j1+1) == 3)) fadr = LDR2val(fw,j1+1);    // R3 ?
				else if (isADR_PC(fw,j1+1) && (fwRd(fw,j1+1) == 3)) fadr = ADR2adr(fw,j1+1);    // R3 ?
				else if (isLDR_PC(fw,j1+2) && (fwRd(fw,j1+2) == 3)) fadr = LDR2val(fw,j1+2);    // R3 ?
				else if (isADR_PC(fw,j1+2) && (fwRd(fw,j1+2) == 3)) fadr = ADR2adr(fw,j1+2);    // R3 ?
				else if (isLDR_PC(fw,j1+3) && (fwRd(fw,j1+3) == 3)) fadr = LDR2val(fw,j1+3);    // R3 ?
				else if (isADR_PC(fw,j1+3) && (fwRd(fw,j1+3) == 3)) fadr = ADR2adr(fw,j1+3);    // R3 ?
				if (fadr != 0)
				{
					fadr = followBranch2(fw, fadr, sig->offset);
					//fprintf(stderr,"%s %08x\n",curr_name,fadr);
					fwAddMatch(fw,fadr,32,0,110);
					return 1;
				}
			}
		}
	}

	return 0;
}

// Sig pattern:
//		Func -			func
//			.... (offset)
//		Ref to string - 	ADR Rx, str
//			....
//		String - 			DCB "str"
int match_strsig11(firmware *fw, string_sig *sig, int j)
{
	int ofst = dryos_offset(fw, sig);
	
	uint32_t sadr = idx2adr(fw,j);        // string address
	int j1;
	for (j1 = j-1; j1 >= 0; j1--)
	{
		if (isADR_PC_cond(fw,j1))   // ADR ?
		{
			uint32_t padr = ADR2adr(fw,j1);
			if (padr == sadr)
			{
				uint32_t fadr = idx2adr(fw,j1-ofst);
				uint32_t bfadr = followBranch(fw,fadr,sig->offset);
				// special case for 'set_control_event'
				int found = 0;
				if (strcmp(sig->name,"set_control_event") == 0)
				{
                    int j2 = j1 - ofst;
					if (isBL(fw,j2) &&                                                  // BL
						isLDR_SP(fw,j2+1) && (fwRd(fw,j2+1) == 0) &&                    // LDR R0,[SP,x]
						isBL(fw,j2+2) &&                                                // BL
						isMOV(fw,j2+3) && (fwRd(fw,j2+3) == 4) && (fwRn(fw,j2+3) == 0)) // LDR R4, R0
					{
						found = 1;
					}
				}
				else
					found = 1;
				if (found && ((sig->offset == 0) || (bfadr != fadr)))
				{
					//fprintf(stderr,"%s %08x\n",curr_name,fadr);
					fwAddMatch(fw,bfadr,32,0,111);
					return 1;
				}
			}
		}
	}

	return 0;
}

// Sig pattern:
//		Func is referenced in 'CreateJumptable'
//			LDR R1, =func
//			STR R1, [R0,nnn]
//		nnn - dryos version dependant offset
int find_strsig12(firmware *fw, string_sig *sig)
{
	int j = get_saved_sig(fw,"CreateJumptable");	

	int ofst = dryos_offset(fw, sig);
	
	if (ofst == 0) return 0;
	
	if (j >= 0)
	{
		if (func_names[j].val != 0)
		{
			int idx = adr2idx(fw, func_names[j].val);
			for(; !isBX_LR(fw,idx); idx++)  // BX LR
			{
				if (((fwval(fw,idx+1) & 0xFFFFF000) == 0xE5801000) && // STR R1,[R0,nnn]
				    (fwOp2(fw,idx+1) == ofst))
				{
					uint32_t fadr = LDR2val(fw,idx);
					uint32_t bfadr = followBranch2(fw,fadr,sig->offset);
					if ((sig->offset <= 1) || ((bfadr != fadr) && ((fw->buf[adr2idx(fw,fadr)] & 0xFFFF0000) == 0xE92D0000)))
					{
						//fprintf(stderr,"%s %08x\n",sig->name,bfadr);
						fwAddMatch(fw,bfadr,32,0,112);
						return 1;
					}
				}
				else if (isB(fw,idx))	// B
				{
					idx = adr2idx(fw,followBranch(fw,idx2adr(fw,idx),1)) - 1;
				}
			}
		}
	}

	return 0;
}

// Sig pattern:
//		Func -			func
//			... (offset)
//		Str ref -			LDR Rx, =str_ptr
//			...
//		Str ptr -			DCD str_ptr
//			...
//		Str ptr -		str_ptr
//							DCD str
//			...
//		String				DCB "str"
int match_strsig13a(firmware *fw, int k, uint32_t sadr, uint32_t offset)
{
	if (fwval(fw,k) == sadr)    // string ptr
	{
		uint32_t padr = idx2adr(fw,k);      // string ptr address
		int j2;
		for (j2 = k-1; j2 >= 0; j2--)
		{
			if (fwval(fw,j2) == padr)       // string ptr address
			{
				uint32_t ppadr = idx2adr(fw,j2);		// string ptr ptr address
				int j3;
				for (j3 = j2-1; j3 >= 0; j3--)
				{
					if (isLDR_PC(fw,j3) && (LDR2adr(fw,j3) == ppadr))
					{
						uint32_t fadr = idx2adr(fw,j3-offset);
						//fprintf(stderr,"%s %08x\n",curr_name,fadr);
						fwAddMatch(fw,fadr,32,0,113);
						return 1;
					}
				}
			}
		}
	}
    return 0;
}
int match_strsig13(firmware *fw, string_sig *sig, int j)
{
    // Note - 'j' is offset in firmware not instruction index (called from fw_string_process_unaligned)
    return search_fw(fw, match_strsig13a, fw->base + j, sig->offset, 1);
}

// Sig pattern:
//		Special case for _pow
int match_strsig14(firmware *fw, string_sig *sig, int j)
{
	// Find values passed to _pow
	if ((fwval(fw,j) == 0x00000000) && (fwval(fw,j+1) == 0x40000000) && (fwval(fw,j+2) == 0x00000000) && (fwval(fw,j+3) == 0x408F4000))
	{
		uint32_t adr1 = idx2adr(fw,j);		// address of 1st value
		uint32_t adr2 = idx2adr(fw,j+2);	// address of 2nd value
		int j1;
		
		for (j1 = j-5; j1>0; j1--)
		{
			if (isADR_PC_cond(fw,j1) &&                 // ADR ?
				(fwval(fw,j1+1) == 0xE8900003) &&       // LDMIA R0,{R0,R1}
				isBL(fw,j1+2) &&                        // BL
				isADR_PC_cond(fw,j1+4))                 // ADR ?
			{
				if ((ADR2adr(fw,j1) == adr1) && (ADR2adr(fw,j1+4) == adr2))
				{
					uint32_t fadr = followBranch(fw,idx2adr(fw,j1+2),0x01000001);
					//fprintf(stderr,"%s %08x\n",curr_name,fadr);
					fwAddMatch(fw,fadr,32,0,114);
					return 1;
				}
			}
			else
			if (isADR_PC_cond(fw,j1) &&                 // ADR ?
				(fwval(fw,j1+2) == 0xE8900003) &&       // LDMIA R0,{R0,R1}
				isBL(fw,j1+3) &&                        // BL
				isADR_PC_cond(fw,j1+4))                 // ADR ?
			{
				if ((ADR2adr(fw,j1) == adr1) && (ADR2adr(fw,j1+4) == adr2))
				{
					uint32_t fadr = followBranch(fw,idx2adr(fw,j1+3),0x01000001);
					//fprintf(stderr,"%s %08x\n",curr_name,fadr);
					fwAddMatch(fw,fadr,32,0,114);
					return 1;
				}
			}
		}
	}
	
	return 0;
}

// Sig pattern:
//				    BL	func
//			...
//		Str ref -	xDR Rx, =str_ptr
//			...
//		String		DCB "str"
int match_strsig15a(firmware *fw, int k, uint32_t sadr, uint32_t offset)
{
    if (isADR_PC_cond(fw,k) || isLDR_PC_cond(fw,k))   // LDR or ADR ?
    {
	    uint32_t padr;
	    if (isLDR_PC_cond(fw,k)) // LDR ?
            padr = LDR2val(fw,k);
	    else
            padr = ADR2adr(fw,k);
	    if (padr == sadr)
	    {
		    int j2 = find_inst_rev(fw, isBL, k-1, 50);
            if (j2 > 0)
		    {
			    uint32_t fa = idx2adr(fw,j2);
			    fa = followBranch2(fw,fa,offset);
			    //fprintf(stderr,"%s %08x\n",curr_name,fa);
			    fwAddMatch(fw,fa,32,0,115);
			    return 1;
		    }
	    }
    }
    return 0;
}
int match_strsig15(firmware *fw, string_sig *sig, int j)
{
    return search_fw(fw, match_strsig15a, idx2adr(fw,j), sig->offset, 1);
}

// Sig pattern:
//	Function immediately preceeding usage of hex value
int match_strsig16(firmware *fw, string_sig *sig, int j)
{
	uint32_t nm0 = *((uint32_t*)sig->ev_name);
	
	if (isADR_PC_cond(fw,j) || isLDR_PC_cond(fw,j))   // LDR or ADR ?
	{
		uint32_t padr;
		if (isLDR_PC_cond(fw,j)) // LDR ?
            padr = LDR2val(fw,j);
		else
            padr = ADR2adr(fw,j);
		if (padr == nm0)
		{
			int j2 = find_inst_rev(fw, isSTMFD_LR, j-1, 50);
            if (j2 > 0)
			{
				uint32_t fa = idx2adr(fw,j2);
				//fprintf(stderr,"%s %08x\n",curr_name,fa);
				fwAddMatch(fw,fa,32,0,116);
				return 1;
			}
		}
	}

	return 0;
}

int find_strsig(firmware *fw, string_sig *sig)
{
	switch (sig->type)
	{
    case 1:		return fw_string_process(fw, sig, match_strsig1, 1);
    case 2:     return fw_string_process_unaligned(fw, sig, match_strsig2);
    case 3:		return fw_string_process(fw, sig, match_strsig3, 1);
    case 4:		return fw_string_process(fw, sig, match_strsig4, 1);
    case 5:		return fw_string_process(fw, sig, match_strsig5, 1);
    case 6:		return fw_string_process(fw, sig, match_strsig6, 1);
    case 7:		return fw_string_process(fw, sig, match_strsig7, 1);
    case 8:		return find_strsig8(fw, sig);
    case 9:		return find_strsig9(fw, sig);
    case 10:	return fw_string_process(fw, sig, match_strsig10, 1);
    case 11:	return fw_string_process(fw, sig, match_strsig11, 0);
    case 12:	return find_strsig12(fw, sig);
    case 13:	return fw_string_process_unaligned(fw, sig, match_strsig13);
    case 14:	return fw_process(fw, sig, match_strsig14);
    case 15:	return fw_string_process(fw, sig, match_strsig15, 1);
    case 16:	return fw_process(fw, sig, match_strsig16);
	}
	
	return 0;
}

//------------------------------------------------------------------------------------------------------------

// Matching functions

void find_str_sig_matches(firmware *fw, const char *curr_name)
{
	int i;
	
	int found_ev = 0;

	count = 0;

	for (i = 0; string_sigs[i].ev_name != 0 && !found_ev; i++)
	{
		if (strcmp(curr_name, string_sigs[i].name) == 0)
		{
			if (find_strsig(fw, &string_sigs[i]))
			{
				found_ev = 1;
				break;
			}
		}
	}

	if (count > 1)
	{
		qsort(matches, count, sizeof(Match), (void*)match_compare);
    }

	if (count > 0)
	{
		save_sig(curr_name, matches->ptr);	
	}
}

void find_matches(firmware *fw, const char *curr_name)
{
    FuncSig *sig, *s;
    BufRange *n;
    uint32_t *p;
	int i, j;
    int fail, success;
	
	int found_ev = 0;

	count = 0;

    // Try and match using 'string' based signature matching first
	for (i = 0; string_sigs[i].ev_name != 0 && !found_ev; i++)
	{
		if (strcmp(curr_name, string_sigs[i].name) == 0)
		{
			if (find_strsig(fw, &string_sigs[i]))
			{
				found_ev = 1;
				break;
			}
		}
	}

    // If not found see if the name is in the old style instruction compare match table
    // Set start value for j in next section if found
    if (!found_ev)
    {
        found_ev = 1;
        for (j=0; func_list[j].name; j++)
        {
            if (strcmp(curr_name,func_list[j].name) == 0)
            {
                found_ev = 0;
                break;
            }
        }
    }

    // Not found so far, try instruction comparison matching
	while (!found_ev)
    {
   		sig = func_list[j].sig;

		for (n = fw->br; n != 0; n = n->next)
        {
    		for (p = n->p, i = 0; i < n->len; p++, i++)
            {
				fail = 0;
				success = 0;
				for (s = sig; s->offs != -1; s++)
                {
					if ((p[s->offs] & s->mask) != s->value)
						fail++;
					else
						success++;
				}
                // If sig starts with STMFD and first instruction does not match ignore it
                if (((p[sig->offs] & sig->mask) != sig->value) && (sig->offs == 0) && (sig->value == 0xe92d0000)) success = 0;
				if (success > fail)
                {
					if (s->mask == -2)
					{
						int end_branch = 0;
						uint32_t idx = 0;
						uint32_t *p1 = 0;
						if ((fw->buf[n->off+i+s->value] & 0x0F000000) == 0x0A000000)   // B
						{
							idx = adr2idx(fw, followBranch2(fw, idx2adr(fw,n->off+i+s->value), 0xF0000001));
							if ((idx >= 0) && (idx < fw->size))
							{
								end_branch = 1;
								p1 = &fw->buf[idx];
							}
						}
						int fail2 = 0;
						int success2 = 0;
						//fprintf(stderr,"\t%s %d %08x %08x %d %d\n",curr_name,idx,idx2adr(fw,idx),idx2adr(fw,i+n->off),success,fail);
						s++;
						for (; s->offs != -1; s++)
                        {
							if (!end_branch || (p1[s->offs] & s->mask) != s->value){
								fail2++;
							} else {
								success2++;
							}
						}
						if (fail2 == 0)
						{
							success = success + fail + success2;
							fail = 0;
						}
						else
						{
							success = success + success2;
							fail = fail + fail2;
						}
						//fprintf(stderr,"\t%s %d %08x %08x %d %d\n",curr_name,idx,idx2adr(fw,idx),idx2adr(fw,i+n->off),success,fail);
					}
				}
				if (success > fail)
                {
                    // Special case for drive space functions, see if there is a refernce to "Mounter.c" in the function
                    // Increase match % if so, increase fail count if not
                    if ((strcmp(curr_name, "GetDrive_ClusterSize") == 0) ||
                        (strcmp(curr_name, "GetDrive_FreeClusters") == 0) ||
                        (strcmp(curr_name, "GetDrive_TotalClusters") == 0))
                    {
                        int fnd = 0;
				        for (s = sig; s->offs != -1; s++)
                        {
                            if (isLDR_PC_cond(fw,n->off+i+s->offs))
                            {
                                int m = adr2idx(fw,LDR2val(fw,n->off+i+s->offs));
                                if ((m >= 0) && (m < fw->size) && (strcmp((char*)(&fw->buf[m]),"Mounter.c") == 0))
                                {
                                    fnd = 1;
                                }
                            }
                            else if (isADR_PC_cond(fw,n->off+i+s->offs))
                            {
                                int m = adr2idx(fw,ADR2adr(fw,n->off+i+s->offs));
                                if ((m >= 0) && (m < fw->size) && (strcmp((char*)(&fw->buf[m]),"Mounter.c") == 0))
                                {
                                    fnd = 1;
                                }
                            }
				        }
                        if (fnd)
                            success++;
                        else
                            fail++;
                    }
					fwAddMatch(fw,idx2adr(fw,i+n->off),success,fail,func_list[j].ver);
					if (count >= MAX_MATCHES)
                    {
						bprintf("// WARNING: too many matches for %s!\n", func_list[j].name);
						break;
					}
				}
			}
		}

		// same name, so we have another version of the same function
		if ((func_list[j+1].name == NULL) || (strcmp(curr_name, func_list[j+1].name) != 0))
        {
            found_ev = 1;
			break;
		}
		j++;
	}

	if (count > 1)
	{
		qsort(matches, count, sizeof(Match), (void*)match_compare);
    }

	if (count > 0)
	{
		save_sig(curr_name, matches->ptr);	
	}
}

void print_results(const char *curr_name, int k)
{
	int i;
	int err = 0;
    char line[500] = "";
	
    if (func_names[k].flags & DONT_EXPORT) return;

	// find best match and report results
	osig* ostub2 = find_sig(stubs2,curr_name);

	if ((count == 0)
		|| (matches->fail > 0)
		|| (ostub2 && (matches->ptr != ostub2->val))
       )
		err = 1;

	// write to header (if error) or body buffer (no error)
	out_hdr = err;

	char *macro = "NHSTUB";
	if (strncmp(curr_name,"task_",5) == 0 ||
        strncmp(curr_name,"hook_",5) == 0) macro = "   DEF";

	if (count == 0)
	{
		if (func_names[k].flags & OPTIONAL) return;
		char fmt[50] = "";
		sprintf(fmt, "// ERROR: %%s is not found. %%%ds//--- --- ", (int)(34-strlen(curr_name)));
		sprintf(line+strlen(line), fmt, curr_name, "");
	}
	else
	{
		if (ostub2 || (func_names[k].flags & UNUSED))
		    sprintf(line+strlen(line),"//%s(%-37s,0x%08x) //%3d ", macro, curr_name, matches->ptr, matches->sig);
		else
			sprintf(line+strlen(line),"%s(%-39s,0x%08x) //%3d ", macro, curr_name, matches->ptr, matches->sig);

		if (matches->fail > 0)
			sprintf(line+strlen(line),"%2d%% ", matches->success*100/(matches->success+matches->fail));
		else
			sprintf(line+strlen(line),"    ");
	}

	if (ostub2)
	{
		if ((count > 0) && (matches->ptr == ostub2->val))
			sprintf(line+strlen(line),"       == 0x%08x    ",ostub2->val);
		else
			sprintf(line+strlen(line),"   *** != 0x%08x    ",ostub2->val);
	}
	else
		sprintf(line+strlen(line),"                        ");

    for (i=strlen(line)-1; i>=0 && line[i]==' '; i--) line[i] = 0;
    bprintf("%s\n",line);

	for (i=1;i<count && matches[i].fail==matches[0].fail;i++)
	{
		if (matches[i].ptr != matches->ptr)
		{
			bprintf("// ALT: %s(%s, 0x%x) // %d %d/%d\n", macro, curr_name, matches[i].ptr, matches[i].sig, matches[i].success, matches[i].fail);
		}
	}
}

//------------------------------------------------------------------------------------------------------------

typedef struct
{
	uint16_t mode;
	char *nm;
} ModeMapName;

ModeMapName mmnames[] = {
	{ 32768,"MODE_AUTO" },
	{ 32769,"MODE_M" },
	{ 32770,"MODE_AV" },
	{ 32771,"MODE_TV" },
	{ 32772,"MODE_P" },
	
	{ 65535,"" }
};

char* mode_name(uint16_t v)
{
	int i;
	for (i=0; mmnames[i].mode != 65535; i++)
	{
		if (mmnames[i].mode == v)
			return mmnames[i].nm;
	}
	
	return "";
}

void output_modemap(firmware *fw, int k)
{
    int cnt = 0;

	if (isLDR_PC(fw,k))
	{
		k = adr2idx(fw,LDR2val(fw,k));
		bprintf("%08x\n",idx2adr(fw,k));
		uint16_t *p = (uint16_t*)(&fw->buf[k]);
		k = 0;
		while ((*p != 0xFFFF) && (k < 50))
		{
			if (((fw->dryos_ver < 47) && ((*p < 8000) || (*p > 8999))) || ((fw->dryos_ver >= 47) && ((*p < 4000) || (*p > 4999))))
			{
				osig *m = find_sig_val(modemap, *p);
				if (!m)
				{
					char *s = mode_name(*p);
					bprintf("// Mode %5d in firmware but not in current modemap",*p);
					if (strcmp(s,"") != 0)
						bprintf(" (%s)",s);
					bprintf("\n");
					cnt++;
				}
				else
				{
					m->pct = 100;
				}
			}
			p++;
			k++;
		}
	}
	osig *m = modemap;
	while (m)
	{
		if (m->pct != 100)	// not matched above?
		{
			bprintf("// Current modemap entry not found in firmware - %-24s %5d\n",m->nm,m->val);
			cnt++;
		}
		m = m->nxt;
	}
	if (cnt == 0)
	{
		bprintf("// No problems found with modemap table.\n");
	}
}

void find_modemap(firmware *fw)
{
	out_hdr = 1;
	add_blankline();

	bprintf("// Check of modemap from 'platform/CAMERA/shooting.c':\n");

	int k = find_str_ref(fw, "AC:PTM_Init");
	if (k >= 0)
	{
		bprintf("// Firmware modemap table found @%08x -> ",idx2adr(fw,k));
		if (isBL(fw,k+2))
		{
			k = idxFollowBranch(fw,k+2,0x01000001);
			bprintf("%08x -> ",idx2adr(fw,k));
            k = find_inst(fw, isBL, k+22, 5);
            if (k >= 0)
			{
				k = idxFollowBranch(fw,k,0x01000001);
				bprintf("%08x -> ",idx2adr(fw,k));
                k = find_inst(fw, isBL, k+37, 5);
                if (k >= 0)
				{
					k = idxFollowBranch(fw,k,0x01000001);
					bprintf("%08x -> ",idx2adr(fw,k));
                    output_modemap(fw,k);
					return;
				}
			}
		}
		bprintf("\n");
	}
}

//------------------------------------------------------------------------------------------------------------

int match_CAM_UNCACHED_BIT(firmware *fw, int k, int v)
{
	if ((fw->buf[k] & 0x0FFFF000) == 0x03C00000)	// BIC
	{
		uint32_t fadr = ALUop2(fw,k);
		bprintf("//#undef  CAM_UNCACHED_BIT\n");
		bprintf("//#define CAM_UNCACHED_BIT  0x%08x // Found @0x%08x\n",fadr,idx2adr(fw,k));
	}

    return 0;
}

// Search for things that go in 'platform_camera.h'
void find_platform_vals(firmware *fw)
{
	int k,k1;
	
	out_hdr = 1;
	add_blankline();

	bprintf("// Values below go in 'platform_camera.h':\n");
	bprintf("//#define CAM_DRYOS         1\n");
	if (fw->dryos_ver >= 39)
		bprintf("//#define CAM_DRYOS_2_3_R39 1 // Defined for cameras with DryOS version R39 or higher\n");
	if (fw->dryos_ver >= 47)
		bprintf("//#define CAM_DRYOS_2_3_R47 1 // Defined for cameras with DryOS version R47 or higher\n");

	// Find 'RAW' image size
	uint32_t raw_width = 0;
	uint32_t raw_height = 0;
	uint32_t kw=0, kh=0;
	
	k = find_str_ref(fw, "\r[%ld] AdjDrvType[%02ld] -> DrvType[%02");
	if (k >= 0)
	{
        // Width
		for (k1 = k-1; k1 >= k-20; k1--)
		{
			if ((fw->buf[k1] & 0x0FFF0FFF) == 0x058D0034)			// STRxx Rn, [SP,#0x34]
			{
				if ((fw->buf[k1-1] & 0x0FFF0000) == 0x03A00000)		// MOVxx Rn, #YYY
				{
					raw_width = ALUop2(fw, k1-1);
					kw = k1-1;
				}
				else if ((fw->buf[k1-2] & 0x0FFF0000) == 0x03A00000)// MOVxx Rn, #YYY
				{
					raw_width = ALUop2(fw, k1-2);
					kw = k1-2;
				}
				else if (isLDR_PC_cond(fw,k1-1))
				{
					raw_width = LDR2val(fw,k1-1);
					kw = k1-1;
				}
				else if (isLDR_PC_cond(fw,k1-2))
				{
					raw_width = LDR2val(fw,k1-2);
					kw = k1-2;
				}
			}
        }
        // Height
		for (k1 = k-1; k1 >= k-20; k1--)
		{
			if ((fw->buf[k1] & 0x0FFF0FFF) == 0x058D0030)			// STRxx Rn, [SP,#0x30]
			{
				if ((fw->buf[k1-1] & 0x0FFF0000) == 0x03A00000)		// MOVxx Rn, #YYY
				{
					raw_height = ALUop2(fw, k1-1);
					kh = k1-1;
				}
				else if ((fw->buf[k1-2] & 0x0FFF0000) == 0x03A00000)// MOVxx Rn, #YYY
				{
					raw_height = ALUop2(fw, k1-2);
					kh = k1-2;
				}
				else if (isLDR_PC_cond(fw,k1-1))
				{
					raw_height = LDR2val(fw,k1-1);
					kh = k1-1;
				}
				else if (isLDR_PC_cond(fw,k1-2))
				{
					raw_height = LDR2val(fw,k1-2);
					kh = k1-2;
				}
				if ((fw->buf[k1-1] & 0x0FFF0000) == 0x02400000)		// SUBxx Rn, #YYY
				{
					raw_height = raw_width - ALUop2(fw, k1-1);
					kh = k1-1;
				}
			}
		}
	}

	if ((raw_width == 0) && (raw_height == 0))
	{
		k = find_str_ref(fw, " CrwAddress %lx, CrwSize H %ld V %ld\r");
		if (k >= 0)
		{
			// Width
			for (k1=k-1; k1>=k-5; k1--)
			{
				if ((fw->buf[k1] & 0xFFFFF000) == 0xE3A02000)	// MOV R2, #nnn
				{
					raw_width = ALUop2(fw,k1);
					kw = k1;
				}
				else
				if (isLDR_PC(fw,k1) && ((fw->buf[k1]& 0x0000F000) == 0x00002000))	// LDR R2, =nnn
				{
					raw_width = LDR2val(fw,k1);
					kw = k1;
				}
			}
			// Height
			for (k1=k-1; k1>=k-5; k1--)
			{
				if ((fw->buf[k1] & 0xFFFFF000) == 0xE3A03000)	// MOV R3, #nnn
				{
					raw_height = ALUop2(fw,k1);
					kh = k1;
				}
				else
				if (isLDR_PC(fw,k1) && ((fw->buf[k1]& 0x0000F000) == 0x00003000))	// LDR R3, =nnn
				{
					raw_height = LDR2val(fw,k1);
					kh = k1;
				}
				else
				if ((fw->buf[k1] & 0xFFFFF000) == 0xE2423000)	// SUB R3, R2, #nnn
				{
					raw_height = raw_width - ALUop2(fw,k1);
					kh = k1;
				}
			}
		}
	}
	
	if (raw_width != 0)
	{
		bprintf("//#define CAM_RAW_ROWPIX    %d // Found @0x%08x\n",raw_width,idx2adr(fw,kw));
	}
    else
    {
		bprintf("//#define CAM_RAW_ROWPIX    *** Not Found ***\n");
    }
	if (raw_height != 0)
	{
		bprintf("//#define CAM_RAW_ROWS      %d // Found @0x%08x\n",raw_height,idx2adr(fw,kh));
	}
    else
    {
		bprintf("//#define CAM_RAW_ROWS      *** Not Found ***\n");
    }

	// Find 'CAM_UNCACHED_BIT'
	search_saved_sig(fw, "FreeUncacheableMemory", match_CAM_UNCACHED_BIT, 0, 0, 8);
}

//------------------------------------------------------------------------------------------------------------

uint32_t find_viewport_address(firmware *fw, int *kout)
{
    int k, k1;

	// find viewwport address for 'vid_get_viewport_fb'
	k = find_str_ref(fw, "VRAM Address  : %p\r");
	if (k >= 0)
	{
		for (k1=k-1; k1>k-8; k1--)
		{
			if (isLDR(fw,k1) && isLDR(fw,k1+1))
			{
				uint32_t v1 = LDR2val(fw,k1);
				uint32_t v2 = LDR2val(fw,k1+1);
				if (v2 > v1) v1 = v2;
                *kout = k1;
                return v1;
			}
		}
	}

    *kout = -1;
    return 0;
}

int match_vid_get_bitmap_fb(firmware *fw, int k, int v)
{
	if (isBL(fw,k-1) && // BL
	    isLDR_PC(fw,k))
	{
		uint32_t v1 = LDR2val(fw,k);
		bprintf("//void *vid_get_bitmap_fb()        { return (void*)0x%08x; }             // Found @0x%08x\n",v1,idx2adr(fw,k));
		return 1;
	}
	else
	if (isBL(fw,k-1) && // BL
	    (isLDR_PC(fw,k+1)))
	{
		uint32_t v1 = LDR2val(fw,k+1);
		bprintf("//void *vid_get_bitmap_fb()        { return (void*)0x%08x; }             // Found @0x%08x\n",v1,idx2adr(fw,k));
		return 1;
	}

    return 0;
}

int match_get_flash_params_count(firmware *fw, int k, int v)
{
	if ((fw->buf[k] & 0xFFF00FFF) == 0xE3C00901)	// BIC Rn, Rn, #0x4000
	{
		uint32_t r = fw->buf[k] & 0x000F0000;	// Register
		if (((fw->buf[k+1] & 0xFFF00000) == 0xE3500000) && ((fw->buf[k+1] & 0x000F0000) == r))	// CMP, Rn #val
		{
			bprintf("//int get_flash_params_count(void) { return 0x%02x; }                          // Found @0x%08x\n",fw->buf[k+1]&0xFFF,idx2adr(fw,k+1));
			return 1;
		}
	}

    return 0;
}

// Search for things that go in 'lib.c'
void find_lib_vals(firmware *fw)
{
	int k,k1;
	
	out_hdr = 1;
	add_blankline();

	bprintf("// Values below go in 'lib.c':\n");

	// Find 'vid_get_bitmap_fb'
	search_saved_sig(fw, "DispCon_ShowBitmapColorBar", match_vid_get_bitmap_fb, 0, 1, 30);
	
	// find 'vid_get_viewport_fb'
    uint32_t v = find_viewport_address(fw,&k);
	if (k >= 0)
	{
        bprintf("//void *vid_get_viewport_fb()      { return (void*)0x%08x; }             // Found @0x%08x\n",v,idx2adr(fw,k));
	}
	
	// find 'vid_get_viewport_fb_d'
    static int fbd[3][3] =
    {
        { -2, -3,  1 },
        {  1,  3,  4 },
        { -1, -2,  1 },
    };
    int sadr = find_str(fw, "ImagePlayer.c");
	k = find_nxt_str_ref(fw, sadr, -1);
    int found = 0;
	while ((k >= 0) && !found)
	{
        int f;
        for (f=0; f<3 && !found; f++)
        {
		    if (isLDR(fw,k+fbd[f][0]) && isLDR(fw,k+fbd[f][1]) && isLDR(fw,k+fbd[f][2]))
		    {
                int reg = fw->buf[k+fbd[f][2]] & 0x000F0000;    // Index register used
                int ka = 0;
                if (((fw->buf[k+fbd[f][0]] & 0x0000F000) << 4) == reg)      { ka = k+fbd[f][0]; }
                else if (((fw->buf[k+fbd[f][1]] & 0x0000F000) << 4) == reg) { ka = k+fbd[f][1]; }
                if (ka > 0)
                {
                    uint32_t adr = LDR2val(fw,ka);
                    for (k1=k+2; k1<k+20; k1++)
                    {
                        if (isSTR(fw,k1) && ((fw->buf[k1] & 0x000F0000) == reg))
                        {
                            uint32_t ofst = fw->buf[k1] & 0x00000FFF;
                        	bprintf("DEF(%-40s,0x%08x) // Found 0x%04x (@0x%08x) + 0x%02x (@0x%08x)\n","viewport_fb_d",adr+ofst,adr,idx2adr(fw,ka),ofst,idx2adr(fw,k1));
            			    bprintf("//void *vid_get_viewport_fb_d()    { return (void*)(*(int*)(0x%04x+0x%02x)); } // Found @0x%08x & 0x%08x\n",adr,ofst,idx2adr(fw,ka),idx2adr(fw,k1));
                            found = 1;
                            break;
                        }
                    }
                }
		    }
        }
        k = find_nxt_str_ref(fw, sadr, k);
	}
	
	// find 'camera_jpeg_count_str'
	k = find_str_ref(fw, "9999");
	if (k >= 0)
	{
		if (isLDR(fw,k-1) && isBL(fw,k+1))
		{
			uint32_t v1 = LDR2val(fw,k-1);
        	bprintf("DEF(%-40s,0x%08x) // Found @0x%08x\n","jpeg_count_str",v1,idx2adr(fw,k-1));
			bprintf("//char *camera_jpeg_count_str()    { return (char*)0x%08x; }             // Found @0x%08x\n",v1,idx2adr(fw,k-1));
		}
	}
	
	// find 'hook_raw_size'
	k = find_str_ref(fw, "CRAW BUFF SIZE  %p");
	if (k >= 0)
	{
		if (isLDR(fw,k-1))
		{
			uint32_t craw_bufsize = LDR2val(fw,k-1);
			bprintf("//long hook_raw_size()             { return 0x%08x; }                    // Found @0x%08x\n",craw_bufsize,idx2adr(fw,k-1));
		}
	}
	
	// Find value for 'get_flash_params_count'
	search_saved_sig(fw, "GetParameterData", match_get_flash_params_count, 0, 0, 30);
}

//------------------------------------------------------------------------------------------------------------

void print_stubs_min(firmware *fw, const char *name, uint32_t fadr, uint32_t atadr)
{
	osig *o = find_sig(stubs_min,name);
	if (o)
	{
    	bprintf("//DEF(%-40s,0x%08x) // Found @0x%08x",name,fadr,atadr);
		if (fadr != o->val)
		{
			bprintf(", ** != ** stubs_min = 0x%08x (%s)",o->val,o->sval);
		}
		else
		{
			bprintf(",          stubs_min = 0x%08x (%s)",o->val,o->sval);
		}
	}
    else
    {
    	bprintf("DEF(%-40s,0x%08x) // Found @0x%08x",name,fadr,atadr);
    }
	bprintf("\n");
}

int match_levent_table(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if ((fw->buf[k] > fw->base) && (fw->buf[k+1] == 0x00000800) && (fw->buf[k+2] == 0x00000002))
	{
	    print_stubs_min(fw,"levent_table",idx2adr(fw,k),idx2adr(fw,k));
	}
    return 0;
}

int match_FlashParamsTable2(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if (fw->buf[k] == v1)
    {
        print_stubs_min(fw,"FlashParamsTable",idx2adr(fw,k),idx2adr(fw,k));
        return 1;
    }
    return 0;
}

int match_FlashParamsTable(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if ((fw->buf[k] > fw->base) && (fw->buf[k+1] == 0x00010000) && (fw->buf[k+2] == 0xFFFF0002))
    {
        if (search_fw(fw, match_FlashParamsTable2, idx2adr(fw,k), 0, 1))
            return 1;
    }
    return 0;
}

int match_movie_status(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if (isLDR_PC(fw, k) &&								// LDR R0, =base
        ((fw->buf[k+1] & 0xFE0F0000) == 0xE20F0000) &&	// ADR R1, =sub
        isSTR(fw, k+2) &&								// STR R1, [R0,N]
        (fw->buf[k+3] == 0xE3A01003) &&					// MOV R1, 3
        isSTR(fw, k+4) &&								// STR R1, [R0,ofst]
        (LDR2val(fw,k) < fw->base))
    {
        uint32_t base = LDR2val(fw,k);
        uint32_t ofst = fw->buf[k+4] & 0x00000FFF;
        print_stubs_min(fw,"movie_status",base+ofst,idx2adr(fw,k));
        return 1;
    }
    else
    if (isLDR_PC(fw, k) &&								// LDR R1, =sub
        isLDR_PC(fw, k+1) &&							// LDR R0, =base
        isSTR(fw, k+2) &&								// STR R1, [R0,N]
        (fw->buf[k+3] == 0xE3A01003) &&					// MOV R1, 3
        isSTR(fw, k+4) &&								// STR R1, [R0,ofst]
        (LDR2val(fw,k+1) < fw->base))
    {
        uint32_t base = LDR2val(fw,k+1);
        uint32_t ofst = fw->buf[k+4] & 0x00000FFF;
        print_stubs_min(fw,"movie_status",base+ofst,idx2adr(fw,k));
        return 1;
    }
    else
    if (isLDR_PC(fw, k) &&								        // LDR Rx, =base
        isLDR(fw, k+1) && (fwRd(fw,k) == fwRn(fw,k+1)) &&       // LDR R0, [Rx, ...]
        isCMP(fw, k+2) && (fwRd(fw,k+2) == fwRd(fw,k+1)) &&     // CMP R0, #...
        (fwval(fw,k+3) == 0x03A00005) &&
        isSTR_cond(fw, k+4) && (fwRn(fw,k+4) == fwRd(fw,k)) &&  // STRxx R0, [Rx,ofst]
        (LDR2val(fw,k) < fw->base))
    {
        uint32_t base = LDR2val(fw,k);
        uint32_t ofst = fwOp2(fw,k+4);
        print_stubs_min(fw,"movie_status",base+ofst,idx2adr(fw,k));
        return 1;
    }
    return 0;
}

int match_full_screen_refresh(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
	if (((fw->buf[k] & 0xFF1FF000) == 0xE51F0000) &&	// LDR R0, =base
		(fw->buf[k+1] == 0xE5D01000) &&					// LDRB R1, [R0]
		(fw->buf[k+2] == 0xE3811002) &&					// ORR R1, R1, #2
		(fw->buf[k+3] == 0xE5C01000) &&					// STRB R1, [R0]
		isBX_LR(fw,k+4))                                // BX LR
	{
		uint32_t base = LDR2val(fw,k);
		print_stubs_min(fw,"full_screen_refresh",base,idx2adr(fw,k));
	}
    return 0;
}

int match_canon_shoot_menu_active(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
	if (((fw->buf[k]   & 0xFF1FF000) == 0xE51F1000) &&	// LDR R1, =base
		((fw->buf[k+1] & 0xFFFFF000) == 0xE5D10000) &&	// LDRB R0, [R1, #n]
		(fw->buf[k+2] == 0xE2800001) &&					// ADD R0, R0, #1
		((fw->buf[k+3] & 0xFFFFF000) == 0xE5C10000) &&	// STRB R0, [R1, #n]
		(isB(fw,k+4)))	                                // B
	{
		uint32_t base = LDR2val(fw,k);
		uint32_t ofst = fw->buf[k+1] & 0x00000FFF;
		print_stubs_min(fw,"canon_shoot_menu_active",base+ofst,idx2adr(fw,k));
	}
	else
	if (((fw->buf[k]   & 0xFF1FF000) == 0xE51F0000) &&	// LDR R0, =base
		((fw->buf[k+1] & 0xFFFFF000) == 0xE5D01000) &&	// LDRB R1, [R0, #n]
		(fw->buf[k+2] == 0xE2811001) &&					// ADD R1, R1, #1
		((fw->buf[k+3] & 0xFFFFF000) == 0xE5C01000) &&	// STRB R1, [R0, #n]
		(isB(fw,k+4)))	                                // B
	{
		uint32_t base = LDR2val(fw,k);
		uint32_t ofst = fw->buf[k+1] & 0x00000FFF;
		print_stubs_min(fw,"canon_shoot_menu_active",base+ofst,idx2adr(fw,k));
	}
    return 0;
}

int match_playrec_mode(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
	if (((fw->buf[k]    & 0xFF1FF000) == 0xE51F1000) &&	// LDR R1, =base
		((fw->buf[k+1]  & 0xFFFFF000) == 0xE5810000) &&	// STR R0, [R1, #n]
		((fw->buf[k+3]  & 0xFF1FF000) == 0xE51F0000) &&	// LDR R0, =base
		((fw->buf[k+4]  & 0xFFFFF000) == 0xE5900000) &&	// LDR R0, [R0, #n]
		((fw->buf[k+6]  & 0xFF1FF000) == 0xE51F1000) &&	// LDR R1, =base
		((fw->buf[k+9]  & 0xFF1FF000) == 0xE51F0000) &&	// LDR R0, =base
		((fw->buf[k+12] & 0xFF1FF000) == 0xE51F1000) &&	// LDR R1, =base
		((fw->buf[k+15] & 0xFF1FF000) == 0xE51F0000) &&	// LDR R0, =base
		((fw->buf[k+18] & 0xFF1FF000) == 0xE51F1000) &&	// LDR R1, =base
		(LDR2val(fw,k) == LDR2val(fw,k+3)) &&
		(LDR2val(fw,k) == LDR2val(fw,k+6)) &&
		(LDR2val(fw,k) == LDR2val(fw,k+9)) &&
		(LDR2val(fw,k) == LDR2val(fw,k+12)) &&
		(LDR2val(fw,k) == LDR2val(fw,k+15)) &&
		(LDR2val(fw,k) == LDR2val(fw,k+18)))
	{
		uint32_t base = LDR2val(fw,k);
		uint32_t ofst = fw->buf[k+1] & 0x00000FFF;
		print_stubs_min(fw,"playrec_mode",base+ofst,idx2adr(fw,k));
	}
    return 0;
}

int match_some_flag_for_af_scan(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
	if (isB(fw,k)   &&  // B loc
		isB(fw,k+1) &&  // B loc
		isB(fw,k+2) &&  // B loc
		isB(fw,k+3) &&  // B loc
		isB(fw,k+4) &&  // B loc
		isB(fw,k+5) &&  // B loc
		isB(fw,k+6) &&  // B loc
		isB(fw,k+7) &&  // B loc
		(followBranch(fw,idx2adr(fw,k),1) != followBranch(fw,idx2adr(fw,k+1),1)) &&
		(followBranch(fw,idx2adr(fw,k),1) == followBranch(fw,idx2adr(fw,k+2),1)) &&
		(followBranch(fw,idx2adr(fw,k),1) == followBranch(fw,idx2adr(fw,k+3),1)) &&
		(followBranch(fw,idx2adr(fw,k),1) != followBranch(fw,idx2adr(fw,k+4),1)) &&
		(followBranch(fw,idx2adr(fw,k),1) != followBranch(fw,idx2adr(fw,k+5),1)) &&
		(followBranch(fw,idx2adr(fw,k),1) == followBranch(fw,idx2adr(fw,k+6),1)) &&
		(followBranch(fw,idx2adr(fw,k),1) == followBranch(fw,idx2adr(fw,k+7),1)) &&
		(isLDR_PC(fw,adr2idx(fw,followBranch(fw,idx2adr(fw,k),1)))))	// LDR R0, =base
	{
		uint32_t base = LDR2val(fw,adr2idx(fw,followBranch(fw,idx2adr(fw,k),1)));
        if (base < fw->base)
		    print_stubs_min(fw,"some_flag_for_af_scan",base,followBranch(fw,idx2adr(fw,k),1));
	}
    return 0;
}

int match_palette_data(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if ((fw->buf[k] == 0) && (fw->buf[k+1] == 0x00FF0000) &&
        (fw->buf[k+577] == 1) && (fw->buf[k+578] == 0x00FF0000) && 
        (fw->buf[k+1154] == 2) && (fw->buf[k+1155] == 0x00FF0000))
    {
        return idx2adr(fw,k);
    }
    else if ((fw->buf[k] == 0) && (fw->buf[k+1] == 0x00FF0000) &&
             (fw->buf[k+513] == 1) && (fw->buf[k+514] == 0x00FF0000) && 
             (fw->buf[k+1026] == 2) && (fw->buf[k+1027] == 0x00FF0000))
    {
        return idx2adr(fw,k);
    }
    return 0;
}

int match_palette_buffer_offset(firmware *fw, int k)
{
    int idx2 = idxFollowBranch(fw, k, 0x01000001);
    if (isLDR(fw, idx2+2) && isBL(fw, idx2+3))
    {
        uint32_t palette_size = LDR2val(fw,idx2+2);
        if (palette_size >= 0x400)
        {
            bprintf("// Offset from start of palette_buffer to color data = %d (Found @0x%08x)\n",palette_size-0x400,idx2adr(fw,idx2+2));
            return 1;
        }
    }
    else if (isADR(fw, idx2+2) && isBL(fw, idx2+3))
    {
        uint32_t palette_size = ALUop2(fw,idx2+2);
        if (palette_size >= 0x400)
        {
            bprintf("// Offset from start of palette_buffer to color data = %d (Found @0x%08x)\n",palette_size-0x400,idx2adr(fw,idx2+2));
            return 1;
        }
    }
    return 0;
}

int match_palette_data3(firmware *fw, int k, uint32_t palette_data, uint32_t v2)
{
    if (isLDR_PC(fw, k) && (LDR2val(fw,k) == palette_data) && isLDR_PC(fw,k-1) && isLDR_PC(fw,k-6) && isLDR(fw,k-5))
    {
        int palette_control = LDR2val(fw,k-6);
        int ptr_offset = fwOp2(fw,k-5);
        uint32_t fadr = find_inst_rev(fw, isSTMFD_LR, k-7, 30);
        if (fadr > 0)
        {
            int k1 = search_fw(fw, find_B, fadr, 0, 1);
            if ((k1 > 0) && isLDR_PC(fw,k1-2) && isLDR(fw,k1-1) && (LDR2val(fw,k1-2) == palette_control))
            {
                int active_offset = fwOp2(fw,k1-1);
                print_stubs_min(fw,"active_palette_buffer",palette_control+active_offset,idx2adr(fw,k1-1));
                print_stubs_min(fw,"palette_buffer_ptr",palette_control+ptr_offset,idx2adr(fw,k-5));
                if (isBL(fw,k+8))
                {
                    fadr = followBranch(fw, idx2adr(fw,k+8), 0x01000001);
                    int idx = adr2idx(fw, fadr);
                    if (isLDR(fw, idx+2) && isBL(fw, idx+3))
                    {
                        uint32_t palette_size = LDR2val(fw,idx+2);
                        if (palette_size >= 0x400)
                        {
                            bprintf("// Offset from start of palette_buffer to color data = %d (Found @0x%08x)\n",palette_size-0x400,idx2adr(fw,idx+2));
                        }
                    }
                }
                return 1;
            }
        }
    }
    return 0;
}

int match_palette_data2(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if (isLDR(fw,k) && (LDR2val(fw,k) == v1))
    {
        int k1;
        for (k1=k+1; k1<k+20; k1++)
        {
            if (isBL(fw,k1) && isLDMFD(fw,k1+2))
            {
                uint32_t fadr = followBranch(fw, idx2adr(fw,k1), 0x01000001);
                int idx = adr2idx(fw, fadr);
                int k2;
                for (k2=idx; k2<idx+40; k2++)
                {
                    if (isSTR(fw,k2) && isLDMFD(fw,k2+1))
                    {
                        int ptr_offset = fwval(fw,k2) & 0xFFF;
                        print_stubs_min(fw,"palette_buffer_ptr",v2+ptr_offset,idx2adr(fw,k2));
                        int found = 0;
                        for (k=idx; (k<idx+16) && !found; k++)
                        {
                            if (isBL(fw,k) && isCMP(fw,k+1) && isADR_PC(fw,k+2))
                            {
                                fadr = ADR2adr(fw,k+2);
                                idx = adr2idx(fw, fadr);
                                int k3;
                                for (k3=idx; (k3<idx+16) && !found; k3++)
                                {
                                    if (isBL(fw,k3))
                                    {
                                        if (match_palette_buffer_offset(fw,k3))
                                            return 1;
                                    }
                                }
                            }
                        }
                        return 1;
                    }
                }
            }
            else if (isLDR_cond(fw,k1) && isLDMFD(fw,k1+2) && isBL(fw,k1-2))
            {
                int ptr_offset = fwval(fw,k1) & 0xFFF;
                print_stubs_min(fw,"palette_buffer_ptr",v2+ptr_offset,idx2adr(fw,k1));
                match_palette_buffer_offset(fw, k1-2);
                return 1;
            }
        }
    }
    return 0;
}

int match_SavePaletteData(firmware *fw, int idx, int palette_data)
{
    if (isBL(fw,idx+13))
    {
        uint32_t fadr = followBranch(fw, idx2adr(fw,idx+13), 0x01000001);
        idx = adr2idx(fw, fadr);
        if (isLDR(fw,idx) && isLDR(fw,idx+1) && isB(fw,idx+2))
        {
            uint32_t palette_control = LDR2val(fw,idx);
            print_stubs_min(fw,"palette_control",palette_control,idx2adr(fw,idx));
            int active_offset = fwval(fw,idx+1) & 0xFFF;
            print_stubs_min(fw,"active_palette_buffer",palette_control+active_offset,idx2adr(fw,idx+1));
            fadr = followBranch(fw,idx2adr(fw,idx+2),1);
            idx = adr2idx(fw, fadr);
            if (isLDR(fw,idx+17) && isLDR(fw,idx+18) && isLDR(fw,idx+12) && (LDR2val(fw,idx+12) == palette_control))
            {
                if (isLDR(fw,idx+13))
                {
                    int ptr_offset = fwval(fw,idx+13) & 0xFFF;
                    print_stubs_min(fw,"palette_buffer_ptr",palette_control+ptr_offset,idx2adr(fw,idx+13));
                }
                int palette_buffer;
                if ((fwval(fw,idx+18) & 0x0000F000) == 0)
                {
                    palette_buffer = LDR2val(fw,idx+17);
                    print_stubs_min(fw,"palette_buffer",palette_buffer,idx2adr(fw,idx+17));
                }
                else
                {
                    palette_buffer = LDR2val(fw,idx+18);
                    print_stubs_min(fw,"palette_buffer",palette_buffer,idx2adr(fw,idx+18));
                }
                if (isBL(fw,idx+26))
                {
                    fadr = followBranch(fw, idx2adr(fw,idx+26), 0x01000001);
                    idx = adr2idx(fw, fadr);
                    if (isLDR(fw, idx+2) && isBL(fw, idx+3))
                    {
                        uint32_t palette_size = LDR2val(fw,idx+2);
                        if (palette_size >= 0x400)
                        {
                            bprintf("// Offset from start of palette_buffer to color data = %d (Found @0x%08x)\n",palette_size-0x400,idx2adr(fw,idx+2));
                        }
                    }
                }
            }
        }
        else if (isLDR(fw,idx) && isLDR(fw,idx+6) && isLDR(fw,idx+7) && isBX(fw,idx+8))
        {
            int active_offset = -1;
            if ((fwval(fw,idx+6) & 0x0000F000) == 0)
                active_offset = fwval(fw,idx+6) & 0xFFF;
            else if ((fwval(fw,idx+7) & 0x0000F000) == 0)
                active_offset = fwval(fw,idx+7) & 0xFFF;
            if (active_offset >= 0)
            {
                uint32_t palette_control = LDR2val(fw,idx);
                //print_stubs_min(fw,"palette_control",palette_control,idx2adr(fw,idx));
                print_stubs_min(fw,"active_palette_buffer",palette_control+active_offset,idx2adr(fw,idx+1));
                search_fw(fw, match_palette_data2, palette_data, palette_control, 1);
            }
        }
        return 1;
    }

    return 0;
}

int match_viewport_address3(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if (isLDR_PC(fw,k) && (LDR2val(fw,k) == v1))
    {
        // Scan back to start of function
        int k1 = find_inst_rev(fw, isSTMFD_LR, k-1, 1000);
        if (k1 > 0)
        {
            // Check if function sig matches what we want
            if (isLDR_PC(fw,k1+1) && isLDR_PC(fw,k1+2) && isLDR(fw,k1+3) &&
                (fwRd(fw,k1+1) == fwRn(fw,k1+3)))
            {
                uint32_t a = LDR2val(fw,k1+1);
                print_stubs_min(fw,"viewport_buffers",v1,idx2adr(fw,k));
                print_stubs_min(fw,"active_viewport_buffer",a,idx2adr(fw,k1+1));
                return 1;
            }
            else
            if (isLDR_PC(fw,k1+1) && isLDR_PC(fw,k1+3) && isLDR(fw,k1+4) &&
                (fwRd(fw,k1+1) == fwRn(fw,k1+4)))
            {
                uint32_t a = LDR2val(fw,k1+1);
                print_stubs_min(fw,"viewport_buffers",v1,idx2adr(fw,k));
                print_stubs_min(fw,"active_viewport_buffer",a,idx2adr(fw,k1+1));
                return 1;
            }
            else
            if (isLDR_PC(fw,k1+1) && isLDR_PC(fw,k1+4) && isLDR(fw,k1+5) &&
                (fwRd(fw,k1+1) == fwRn(fw,k1+5)))
            {
                uint32_t a = LDR2val(fw,k1+1);
                print_stubs_min(fw,"viewport_buffers",v1,idx2adr(fw,k));
                print_stubs_min(fw,"active_viewport_buffer",a,idx2adr(fw,k1+1));
                return 1;
            }
        }
    }
    return 0;
}

int match_viewport_address2(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if (fw->buf[k] == v1)
    {
        if (search_fw(fw, match_viewport_address3, v1, 0, 1))
            return 1;
    }
    return 0;
}

int match_viewport_address(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if (fw->buf[k] == v1)
    {
        // Find location in firmware that points to viewport address
        if (search_fw(fw, match_viewport_address2, idx2adr(fw,k), 0, 1))
            return 1;
    }
    return 0;
}

int match_physw_status(firmware *fw, int k, int v)
{
	if (isLDR_PC(fw,k))
	{
		print_stubs_min(fw,"physw_status",LDR2val(fw,k),idx2adr(fw,k));
	}

    return 0;
}

int match_physw_run(firmware *fw, int k, int v)
{
	if (isLDR_PC(fw,k))
	{
		uint32_t base = LDR2val(fw,k);
		uint32_t fadr = followBranch(fw, idx2adr(fw,k+1), 1);
		uint32_t ofst = fw->buf[adr2idx(fw,fadr)] & 0x00000FFF;
		print_stubs_min(fw,"physw_run",base+ofst,idx2adr(fw,k));

		ofst = fw->buf[k+2] & 0x00000FFF;
		print_stubs_min(fw,"physw_sleep_delay",base+ofst,idx2adr(fw,k));
	}

    return 0;
}

int match_canon_menu_active(firmware *fw, int k, int v)
{
	if (isLDR_PC(fw,k))
	{
		uint32_t base = LDR2val(fw,k);
        int k1;
		for (k1=k+1; k1<k+5; k1++)
		{
			if (isLDR(fw,k1))
			{
				uint32_t ofst = fw->buf[k1] & 0x00000FFF;
				print_stubs_min(fw,"canon_menu_active",base+ofst,idx2adr(fw,k));
			}
		}
	}

    return 0;
}

int match_zoom_busy(firmware *fw, int k, int v)
{
    if (isBL(fw,k))
    {
        int idx1 = idxFollowBranch(fw,k,0x01000001);
        int k1;
        for (k1=idx1; k1<idx1+50; k1++)
        {
            if ((fw->buf[k1] & 0xFFFF0000) == 0xE8BD0000)   // LDMFD
            {
                uint32_t fadr = 0;
                if (isADR_PC(fw,k1+1))
                {
                    fadr = ADR2adr(fw,k1+1);
                }
                else if (isADR_PC(fw,k1+2))
                {
                    fadr = ADR2adr(fw,k1+2);
                }
                else if (isADR_PC(fw,k1-3))
                {
                    fadr = ADR2adr(fw,k1-3);
                }
                else if (isLDR_PC(fw,k1+1))
                {
                    fadr = LDR2val(fw,k1+1);
                }
                else if (isLDR_PC(fw,k1+2))
                {
                    fadr = LDR2val(fw,k1+2);
                }
                if (fadr != 0)
                {
                    int idx2 = adr2idx(fw,fadr);
                    if (isLDR_PC(fw,idx2+1) && isLDR(fw,idx2+2))
                    {
                        int base = LDR2val(fw,idx2+1);
                        int ofst = fw->buf[idx2+2] & 0xFFF;
               			print_stubs_min(fw,"zoom_busy",base+ofst-4,fadr);
                        break;
                    }
                }
            }
        }
    }

    return 0;
}

int match_focus_busy(firmware *fw, int k, int v)
{
    if ((fw->buf[k] & 0xFFFF0000) == 0xE8BD0000)   // LDMFD
    {
        int k1 = 0;
        if (isBL(fw,k-2))
        {
            k1 = idxFollowBranch(fw,k-2,0x01000001);
        }
        if (isBL(fw,k-1))
        {
            k1 = idxFollowBranch(fw,k-1,0x01000001);
        }
        if (k1 != 0)
        {
            if (isLDR_PC(fw,k1+1) && isLDR(fw,k1+3))
            {
                int base = LDR2val(fw,k1+1);
                int ofst = fw->buf[k1+3] & 0xFFF;
               	print_stubs_min(fw,"focus_busy",base+ofst-4,idx2adr(fw,k1));
                return 1;
            }
        }
    }

    return 0;
}

int match_bitmap_buffer2(firmware *fw, int k, int v)
{
    uint32_t screen_lock = idx2adr(fw,k);
    if (isBL(fw,v) && (followBranch(fw,idx2adr(fw,v),0x01000001) == screen_lock) && isBL(fw,v+2) && isBL(fw,v+3))
    {
        uint32_t fadr = followBranch2(fw,idx2adr(fw,v+3),0x01000001);
        int k1 = adr2idx(fw,fadr);
        if (isLDR_PC(fw,k1+1))
        {
            int reg = (fwval(fw,k1+1) & 0x0000F000) >> 12;
            uint32_t adr = LDR2val(fw,k1+1);
            int k2;
            for (k2=k1; k2<k1+32; k2++)
            {
                if (isLDR_PC(fw,k2) && isLDR(fw,k2+1) && (((fwval(fw,k2+1) & 0x000F0000) >> 16) == reg))
                {
                    uint32_t bitmap_buffer = LDR2val(fw,k2);
                    if (bitmap_buffer == (adr + 0x1C))
                    {
                        uint32_t active_bitmap_buffer = adr + (fwval(fw,k2+1) & 0xFFF);
                        print_stubs_min(fw,"bitmap_buffer",bitmap_buffer,idx2adr(fw,k2));
                        print_stubs_min(fw,"active_bitmap_buffer",active_bitmap_buffer,idx2adr(fw,k2+1));
                    }
                }
            }
        }
    }

    return 0;
}

int match_bitmap_buffer(firmware *fw, int k, int v)
{
    search_saved_sig(fw, "ScreenLock", match_bitmap_buffer2, k, 0, 1);
    return 0;
}

int match_raw_buffer(firmware *fw, int k, uint32_t rb1, uint32_t v2)
{
    if ((fwval(fw,k) == rb1) && (fwval(fw,k+4) == rb1) && (fwval(fw,k-2) != 1))
    {
        uint32_t rb2 = fwval(fw,k+1);
        if ((rb1 != rb2) && (rb2 > 0))
        {
            bprintf("// Camera has 2 RAW buffers @ 0x%08x & 0x%08x\n", rb1, rb2, idx2adr(fw,k));
            bprintf("//  Note: active buffer --> raw_buffers[active_raw_buffer]\n");
            bprintf("//        other buffer  --> raw_buffers[active_raw_buffer^1]\n");
            print_stubs_min(fw,"raw_buffers",idx2adr(fw,k),idx2adr(fw,k));
        }
        return rb2;
    }
    else if ((fwval(fw,k) == rb1) && (fwval(fw,k-2) == 2) && (fwval(fw,k-7) == rb1))
    {
        uint32_t rb2 = fwval(fw,k+3);
        if ((rb1 != rb2) && (rb2 > 0))
        {
            bprintf("// Camera has 2 RAW buffers @ 0x%08x & 0x%08x\n", rb1, rb2, idx2adr(fw,k));
            bprintf("//  Note: active buffer --> raw_buffers[ active_raw_buffer   *3]\n");
            bprintf("//        other buffer  --> raw_buffers[(active_raw_buffer^1)*3]\n");
            print_stubs_min(fw,"raw_buffers",idx2adr(fw,k),idx2adr(fw,k));
        }
        return rb2;
    }
    return 0;
}

// Search for things that go in 'stubs_min.S'
void find_stubs_min(firmware *fw)
{
	int k,k1;
	
	out_hdr = 1;
	add_blankline();

	bprintf("// Values below can be overridden in 'stubs_min.S':\n");

	// Find 'physw_status'
	search_saved_sig(fw, "kbd_read_keys", match_physw_status, 0, 0, 5);
	
	// Find 'physw_run' & 'physw_sleep_delay'
	search_saved_sig(fw, "task_PhySw", match_physw_run, 0, 0, 5);
	
	// Find 'levent_table'
    search_fw(fw, match_levent_table, 0, 0, 1);
	
	// Find 'FlashParamsTable'
    search_fw(fw, match_FlashParamsTable, 0, 0, 1);
	
	// Find 'movie_status'
    search_fw(fw, match_movie_status, 0, 0, 1);
	
    // Find 'video_compression_rate'
	int sadr = find_str(fw, "CompressionRateAdjuster.c");
	k = find_nxt_str_ref(fw, sadr, -1);
    int found = 0;
	while ((k >= 0) && !found)
	{
        int f = find_inst_rev(fw, isSTMFD_LR, k-1, 100);
        if (f != -1)
        {
            f = search_fw(fw, find_BL, f, 0, 1);
            if (f > 0)
            {
                f--;
                if ((fwval(fw,f) & 0xFFF00000) == 0xE2400000)     // SUB
                {
                    int src = fwRn(fw,f);
                    for (k1 = f-1; (k1 > f-10) && !found; k1--)
                    {
                        if (isLDR_PC(fw,k1) && (fwRd(fw,k1) == src))
                        {
                            uint32_t v = LDR2val(fw,k1) - ALUop2(fw,f);
                            print_stubs_min(fw,"video_compression_rate",v,idx2adr(fw,k1));
                            found = 1;
                        }
                    }
                }
            }
        }
        k = find_nxt_str_ref(fw, sadr, k);
	}

	// Find 'full_screen_refresh'
    search_fw(fw, match_full_screen_refresh, 0, 0, 1);
	
	// Find 'canon_menu_active'
	search_saved_sig(fw, "StartRecModeMenu", match_canon_menu_active, 0, 0, 5);
	
	// Find 'canon_shoot_menu_active'
    search_fw(fw, match_canon_shoot_menu_active, 0, 0, 1);

	// Find 'playrec_mode'
	int found_playrec_mode = 0;
	k = find_str_ref(fw, "AFFChg");
	if ((k >= 0) && isBL(fw,k+6))
	{
		k = idxFollowBranch(fw, k+6, 0x01000001);
		if (isLDR_PC(fw,k) && isLDR(fw,k+1))
		{
			uint32_t base = LDR2val(fw,k);
			uint32_t ofst = fw->buf[k+1] & 0x00000FFF;
			print_stubs_min(fw,"playrec_mode",base+ofst,idx2adr(fw,k));
			found_playrec_mode = 1;
		}
	}
	if (!found_playrec_mode)
	{
        search_fw(fw, match_playrec_mode, 0, 0, 1);
	}
	
	// Find 'zoom_status'
	int found_zoom_status = 0;
	
	k = find_str_ref(fw, "m_ZoomState            :%d\n");
	if (k >= 0)
	{
		if (isLDR(fw,k-1))
		{
			uint32_t ofst = fw->buf[k-1] & 0x00000FFF;
			uint32_t reg = (fw->buf[k-1] & 0x000F0000) >> 16;
			uint32_t ldr_inst = 0xE51F0000 | (reg << 12);
			for (k1=k-2; k1>k-20; k1--)
			{
				if ((fw->buf[k1] & 0xFF1FF000) == ldr_inst)
				{
					uint32_t base = LDR2val(fw,k1);
					print_stubs_min(fw,"zoom_status",base+ofst,idx2adr(fw,k));
					found_zoom_status = 1;
					break;
				}
			}
		}
	}
	
	if (!found_zoom_status)
	{
		for (k=0; k<fw->size; k++)
		{
			if (((fw->buf[k] & 0xFF1FF000) == 0xE51F0000) &&	// LDR R0, =base
				(fw->buf[k+1] == 0xE5D00000) &&					// LDRB R0, [R0]
				(fw->buf[k+2] == 0xE1B00000) &&					// MOVS R0, R0
				(fw->buf[k+3] == 0x13A00001) &&					// MOVNE R0, #1
				isBX_LR(fw,k+4))                                // BX LR
			{
				uint32_t base = LDR2val(fw,k);
				print_stubs_min(fw,"zoom_status",base,idx2adr(fw,k));
				found_zoom_status = 1;
				//break;
			}
		}
	}
	
	if (!found_zoom_status)
	{
		k = find_str_ref(fw, "TerminateDeliverToZoomController");
		if (k >= 0)
		{
			for (k1=0; k1<5; k1++)
			{
				if (isLDR_PC(fw,k+k1))
				{
					uint32_t base = LDR2val(fw,k+k1);
					print_stubs_min(fw,"zoom_status",base+0x20,idx2adr(fw,k+k1));
					found_zoom_status = 1;
					break;
				}
			}
		}
	}
	
	// Find 'some_flag_for_af_scan'
    search_fw(fw, match_some_flag_for_af_scan, 0, 0, 1);

    // focus_len_table
    if (min_focus_len != 0)
    {
        int found = 0, pos = 0, len = 0, size = 0;
    	for (k=0; k<fw->size; k++)
	    {
            if (fw->buf[k] == min_focus_len)
            {
                int mul = 1;
                if ((fw->buf[k+1] == 100) && (fw->buf[k+2] == 0)) mul = 3;
                if ((fw->buf[k+1] == 100) && (fw->buf[k+2] != 0)) mul = 2;
                if ((fw->buf[k+1] ==   0) && (fw->buf[k+2] != 0)) mul = 2;
                for (k1 = k + mul; (k1 < fw->size) && (fw->buf[k1] > fw->buf[k1-mul]) && (fw->buf[k1] > min_focus_len) && (fw->buf[k1] < max_focus_len); k1 += mul) ;
                if (fw->buf[k1] == max_focus_len)
                {
                    found++;
                    pos = k;
                    len = ((k1 - k) / mul) + 1;
                    size = mul;
                }
            }
        }
        if (found == 1)
        {
            bprintf("// focus_len_table contains zoom focus lengths for use in 'get_focal_length' (main.c).\n");
            if (size == 1)
                bprintf("// each entry contains 1 int value, which is the the zoom focus length.\n",size);
            else
                bprintf("// each entry contains %d int value(s), the first is the zoom focus length.\n",size);
            bprintf("// there are %d entries in the table - set NUM_FL to %d\n",len,len);
			print_stubs_min(fw,"focus_len_table",idx2adr(fw,pos),idx2adr(fw,pos));
        }
    }
	
	// Find 'zoom_busy'
	search_saved_sig(fw, "ResetZoomLens", match_zoom_busy, 0, 0, 5);
	
	// Find 'focus_busy'
	search_saved_sig(fw, "ResetFocusLens", match_focus_busy, 0, 0, 25);
	
	// Find 'recreview_hold'
	k = find_str_ref(fw, "ShootCon_NotifyStartReviewHold");
	if (k >= 0)
	{
        for (k1=k; k1<k+20; k1++)
        {
            if (isLDR_PC(fw,k1) && ((fw->buf[k1+1] & 0xFFFF0FFF) == 0xE3A00001) && isSTR(fw,k1+2) &&
                ((fw->buf[k1+1] & 0x0000F000) == (fw->buf[k1+2] & 0x0000F000)) &&
                ((fw->buf[k1] & 0x0000F000) == ((fw->buf[k1+2] & 0x000F0000) >> 4)))
            {
                uint32_t base = LDR2val(fw,k1);
                int ofst = fw->buf[k1+2] & 0x00000FFF;
                print_stubs_min(fw,"recreview_hold",base+ofst,idx2adr(fw,k1));
                break;
            }
        }
	}

    // Find palette colour data
    uint32_t palette_data = search_fw(fw, match_palette_data, 0, 0, 1);

	// Find 'palette buffer' info
    if (palette_data)
    {
        bprintf("// Palette colour tables  found @ 0x%08x\n", palette_data);
        if (search_saved_sig(fw, "SavePaletteData", match_SavePaletteData, palette_data, 0, 1) == 0)
        {
            search_fw(fw, match_palette_data3, palette_data, 0, 1);
        }
	}
    
	// Find 'bitmap buffer' info
	search_saved_sig(fw, "GUISrv_StartGUISystem", match_bitmap_buffer, 0, 0, 32);

    // Get viewport address
    uint32_t v = find_viewport_address(fw,&k);
	if (k >= 0)
	{
        search_fw(fw, match_viewport_address, v, 0, 1);
    }
	
	// find 1st RAW buffer address
	k = find_str_ref(fw, "CRAW BUFF       %p");
	if (k >= 0)
	{
        uint32_t rb1 =0, rb2 = 0;
		if (isLDR(fw,k-1))
		{
			rb1 = LDR2val(fw,k-1);
        }
        else if (isMOV_immed(fw,k-1))
        {
			rb1 = ALUop2(fw,k-1);
        }
        else if (isMOV(fw,k-1) && (fwRd(fw,k-1) == 1))
        {
            int reg = fwval(fw,k-1) & 0xF;
            for (k1=k-2; k1>k-50; k1--)
            {
                if (isLDR(fw,k1) && (fwRd(fw,k1) == reg))
                {
                    rb1 = LDR2val(fw,k1);
                    break;
                }
            }
        }
        if (rb1 > 0)
        {
            rb2 = search_fw(fw, match_raw_buffer, rb1, 0, 5);
            if ((rb2 > 0) && (rb1 != rb2))
            {
                // Find 'active_raw_buffer'
	            sadr = find_str(fw, "SsImgProcBuf.c");
	            k = find_nxt_str_ref(fw, sadr, -1);
                found = 0;
	            while ((k >= 0) && !found)
	            {
                    int f = find_inst_rev(fw, isSTMFD_LR, k-1, 100);
                    if (f != -1)
                    {
                        int e = find_inst(fw, isLDMFD_PC, f+1, 200);
                        for (k1 = f+1; k1 < e; k1++)
                        {
                            if (
                                (
                                    ((fwval(fw,k1)   & 0xFFF00FFF) == 0xE2400001) &&    // SUB Rx, Rn, #1
                                    isLDR(fw,k1+1) &&                                   // LDR Ry, [Rz,
                                    ((fwval(fw,k1+2) & 0xFFF00000) == 0xE1500000) &&    // CMP Rx, Ry
                                    (((fwRd(fw,k1) == fwRd(fw,k1+2)) && (fwRd(fw,k1+1) == fwRn(fw,k1+2))) ||
                                     ((fwRd(fw,k1) == fwRn(fw,k1+2)) && (fwRd(fw,k1+1) == fwRd(fw,k1+2)))) &&
                                    ((fwval(fw,k1+3) & 0xFFF00FFF) == 0x12800001) &&    // ADDNE Ry, Ry, #1
                                    ((fwRd(fw,k1+3) == fwRn(fw,k1+3)) && (fwRd(fw,k1+3) == fwRd(fw,k1+1))) &&
                                    ((fwval(fw,k1+4) & 0xFFF00FFF) == 0x03A00000) &&    // MOVEQ Ry, #0
                                    (fwRd(fw,k1+4) == fwRd(fw,k1+1)) &&
                                    isSTR(fw,k1+5) &&                                   // STR Ry, [Rz,
                                    ((fwRd(fw,k1+5) == fwRd(fw,k1+1)) && (fwRn(fw,k1+5) == fwRn(fw,k1+1)) && (fwOp2(fw,k1+5) == fwOp2(fw,k1+1)))
                                ) ||
                                (
                                    ((fwval(fw,k1)   & 0xFFF00FFF) == 0xE2400001) &&    // SUB Rx, Rn, #1
                                    isLDR(fw,k1+1) &&                                   // LDR Ry, [Rz,
                                    ((fwval(fw,k1+3) & 0xFFF00000) == 0xE1500000) &&    // CMP Rx, Ry
                                    (((fwRd(fw,k1) == fwRd(fw,k1+3)) && (fwRd(fw,k1+1) == fwRn(fw,k1+3))) ||
                                     ((fwRd(fw,k1) == fwRn(fw,k1+3)) && (fwRd(fw,k1+1) == fwRd(fw,k1+3)))) &&
                                    ((fwval(fw,k1+4) & 0xFFF00FFF) == 0x12800001) &&    // ADDNE Ry, Ry, #1
                                    ((fwRd(fw,k1+4) == fwRn(fw,k1+4)) && (fwRd(fw,k1+4) == fwRd(fw,k1+1))) &&
                                    ((fwval(fw,k1+5) & 0xFFF00FFF) == 0x03A00000) &&    // MOVEQ Ry, #0
                                    (fwRd(fw,k1+5) == fwRd(fw,k1+1)) &&
                                    isSTR(fw,k1+7) &&                                   // STR Ry, [Rz,
                                    ((fwRd(fw,k1+7) == fwRd(fw,k1+1)) && (fwRn(fw,k1+7) == fwRn(fw,k1+1)) && (fwOp2(fw,k1+7) == fwOp2(fw,k1+1)))
                                )
                               )
                            {
                                int ofst = fwOp2(fw,k1+1);
                                int reg = fwRn(fw,k1+1);
                                int k2;
                                for (k2 = f+1; k2 < e; k2++)
                                {
                                    if (isLDR_PC(fw,k2) && (fwRd(fw,k2) == reg))
                                    {
                                        uint32_t base = LDR2val(fw,k2);
                                        print_stubs_min(fw,"active_raw_buffer",base+ofst,idx2adr(fw,k1));
                                        found = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    k = find_nxt_str_ref(fw, sadr, k);
	            }
            }
		}
	}
}

//------------------------------------------------------------------------------------------------------------

int find_ctypes(firmware *fw, int k)
{
    static unsigned char ctypes[] = 
    { 
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x60, 0x60, 0x60, 0x60, 0x60, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x48, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
        0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
        0x10, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0x10, 0x10, 0x10, 0x10, 0x10,
        0x10, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0x10, 0x10, 0x10, 0x10, 0x20
    };

    if (k < (fw->size*4 - sizeof(ctypes)))
    {
        if (memcmp(((char*)fw->buf)+k,ctypes,sizeof(ctypes)) == 0)
        {
            bprintf("DEF(ctypes, 0x%08x)\n", fw->base + k);
            return 1;
        }
    }
    return 0;
}

int match_nrflag3(firmware *fw, int k, uint32_t v1, uint32_t v2)
{
    if (isBL(fw,k) && (idxFollowBranch(fw,k,0x01000001) == v1))
    {
        // Found call to function, work out R3 value passed in
        int ofst1 = 0;
        int k3, k4 = 0;
        for (k3=k; k3>k-30; k3--)
        {
            if ((fwval(fw,k3) & 0x0F0FF000) == 0x020D3000)       // Dest = R3, Src = SP = skip
                break;
            if ((fwval(fw,k3) & 0xFF0FF000) == 0xE2033000)       // ADD/SUB R3,R3,x
            {
                k4 = k3;
                if ((fwval(fw,k3) & 0x00F00000) == 0x00400000)   // SUB
                    ofst1 -= (fwval(fw,k3) & 0x00000FFF);
                else
                    ofst1 += (fwval(fw,k3) & 0x00000FFF);
            }
            if (isLDR_PC(fw,k3) && (fwRd(fw,k3) == 3))
            {
                int ofst2 = LDR2val(fw,k3);
                bprintf("\n// For capt_seq.c\n");
                if (ofst1 == 0)
                {
                    bprintf("DEF(_nrflag,0x%04x) // Found @ %08x (0x%04x)\n",ofst2,idx2adr(fw,k3),ofst2);
                    bprintf("//static long *nrflag = (long*)(0x%04x);       // Found @ %08x\n",ofst2,idx2adr(fw,k3));
                }
                else if (ofst1 < 0)
                {
                    bprintf("DEF(_nrflag,0x%04x) // Found @ %08x (0x%04x) & %08x (-0x%02x)\n",ofst2+ofst1,idx2adr(fw,k3),ofst2,idx2adr(fw,k),-ofst1);
                    bprintf("//static long *nrflag = (long*)(0x%04x-0x%02x);  // Found @ %08x & %08x\n",ofst2,-ofst1,idx2adr(fw,k3),idx2adr(fw,k4));
                }
                else
                {
                    bprintf("DEF(_nrflag,0x%04x) // Found @ %08x (0x%04x) & %08x (+0x%02x)\n",ofst2+ofst1,idx2adr(fw,k3),ofst2,idx2adr(fw,k),ofst1);
                    bprintf("//static long *nrflag = (long*)(0x%04x+0x%02x);  // Found @ %08x & %08x\n",ofst2,ofst1,idx2adr(fw,k3),idx2adr(fw,k4));
                }
                return 1;
            }
        }
    }
    return 0;
}

int match_nrflag(firmware *fw, int idx, int v)
{
    int k1, k2, k3;
    int found = 0;

    if (isLDR(fw, idx+1) && isLDR(fw, idx+2))
    {
        k3 = idx+2;
        int ofst2 = LDR2val(fw, k3);

        for (k1=k3+1; k1<k3+8; k1++)
        {
            if (isB(fw, k1))
            {
                k2 = idxFollowBranch(fw,k1,0x01000001);
                if (isSTR(fw, k2))
                {
                    found = 1;
                    break;
                }
                k2++;
                if (isSTR(fw, k2))
                {
                    found = 1;
                    break;
                }
            }
        }

        if (found)
        {
            int ofst1 = fw->buf[k2] & 0x00000FFF;
            bprintf("\n// For capt_seq.c\n");
            bprintf("DEF(_nrflag,0x%04x) // Found @ %08x (0x%04x) & %08x (+0x%02x)\n",ofst2+ofst1,idx2adr(fw,k3),ofst2,idx2adr(fw,k2),ofst1);
            bprintf("//static long *nrflag = (long*)(0x%04x+0x%02x);  // Found @ %08x & %08x\n",ofst2,ofst1,idx2adr(fw,k3),idx2adr(fw,k2));
            bprintf("//#define NR_AUTO (0)                          // have to explictly reset value back to 0 to enable auto\n");
        }
    }

    return found;
}

int match_nrflag2(firmware *fw, int k, int v)
{
    // Found NR_GetDarkSubType function, now follow first BL call.
    if (isBL(fw,k))
    {
        k = idxFollowBranch(fw,k,0x01000001);
        return search_fw(fw, match_nrflag3, k, 0, 1);
    }

    return 0;
}

// Search for things
void find_other_vals(firmware *fw)
{
	out_hdr = 1;
	add_blankline();

	bprintf("// Misc stuff\n");

    if (!search_fw_bytes(fw, find_ctypes))
    {
        bprintf("//DEF(ctypes, *** Not Found ***)\n");
    }

    // Look for nrflag (for capt_seq.c)
    int found = 0;
    if (fw->dryos_ver >= 45)
    {
        found = search_saved_sig(fw, "NR_SetDarkSubType", match_nrflag, 0, 0, 1);
    }
    if (!found)
    {
	    search_saved_sig(fw, "NR_GetDarkSubType", match_nrflag2, 0, 0, 20);
    }
}

//------------------------------------------------------------------------------------------------------------

void print_kval(firmware *fw, uint32_t tadr, int tsiz, int tlen, uint32_t ev, const char *name, char *sfx)
{
	int tidx = adr2idx(fw,tadr);
	int k, kval = 0;
	for (k=0; k<tlen; k+=tsiz)
	{
		if (fw->buf[tidx+k+1] == ev)
		{
			kval = fw->buf[tidx+k];
			tadr = idx2adr(fw,tidx+k);
			break;
		}
	}
	if (kval > 0)
	{
		char fn[100], rn[100];
		strcpy(fn,name); strcat(fn,sfx);
		strcpy(rn,name); strcat(rn,"_IDX");
		
		int r = (kval >> 5) & 7;
		uint32_t b = (1 << (kval & 0x1F));
		
		bprintf("//#define %-20s0x%08x // Found @0x%08x, levent 0x%x\n",fn,b,tadr,ev);
		bprintf("//#define %-20s%d\n",rn,r);
	}
}

typedef struct {
	int			reg;
	uint32_t	bits;
	char		nm[32];
	uint32_t	fadr;
	uint32_t	ev;
	int         inv;
} kinfo;

int		kmask[3];
kinfo	key_info[100];
int		kcount = 0;

void add_kinfo(int r, uint32_t b, const char *nm, uint32_t adr, uint32_t ev, int inv)
{
	key_info[kcount].reg = r;
	key_info[kcount].bits = b;
	strcpy(key_info[kcount].nm, nm);
	key_info[kcount].fadr = adr;
	key_info[kcount].ev = ev;
	key_info[kcount].inv = inv;
	kcount++;
	kmask[r] |= b;
}

uint32_t add_kmval(firmware *fw, uint32_t tadr, int tsiz, int tlen, uint32_t ev, const char *name, uint32_t xtra)
{
	int tidx = adr2idx(fw,tadr);
	int r, k, kval = 0;
	uint32_t b = 0;
	int inv = 0;
	for (k=0; k<tlen; k+=tsiz)
	{
		if (fw->buf[tidx+k+1] == ev)
		{
			kval = fw->buf[tidx+k];
			tadr = idx2adr(fw,tidx+k);
			break;
		}
	}
	if (kval > 0)
	{
		r = (kval >> 5) & 7;
		b = (1 << (kval & 0x1F));
		inv = ((kval&0xff0000)==0x10000)?0:1;
		
		add_kinfo(r,b|xtra,name,tadr,ev,inv);
	}
	
	return b;
}

int kinfo_compare(const kinfo *p1, const kinfo *p2)
{
    if (p1->reg > p2->reg)
	{
		return 1;
    }
	else if (p1->reg < p2->reg)
	{
		return -1;
    }
	if ((p1->ev <= 1) && (p2->ev <= 1))	// output shutter entries in reverse order
	{
		if (p1->bits > p2->bits)
		{
			return -1;
		}
		else if (p1->bits < p2->bits)
		{
			return 1;
		}
	}
    if (p1->bits > p2->bits)
    {
        return 1;
    }
    else if (p1->bits < p2->bits)
    {
        return -1;
    }

    return 0;
}

void print_kmvals()
{
	qsort(key_info, kcount, sizeof(kinfo), (void*)kinfo_compare);
	
	bprintf("//static KeyMap keymap[] = {\n");
	
	int k;
	for (k=0; k<kcount; k++)
	{
		bprintf("//    { %d, %-20s,0x%08x }, // Found @0x%08x, levent 0x%02x%s\n",key_info[k].reg,key_info[k].nm,key_info[k].bits,key_info[k].fadr,key_info[k].ev,(key_info[k].inv==0)?"":" (uses inverted logic in physw_status)");
	}
	
	bprintf("//    { 0, 0, 0 }\n//};\n");
}

int match_GetSDProtect(firmware *fw, int k, int v)
{
	if (isB(fw,k))	// B
	{
		k = idxFollowBranch(fw,k,1);
		if (isLDR_PC(fw,k))
		{
			return LDR2val(fw,k);
		}
	}

    return 0;
}

void find_key_vals(firmware *fw)
{
	int k,k1;
	
	out_hdr = 1;
	add_blankline();

	// find 'SD_READONLY_FLAG'
	uint32_t tadr = search_saved_sig(fw, "GetSDProtect", match_GetSDProtect, 0, 1, 1);
	if (tadr == 0)
	{
		k = find_str_ref(fw,"SD Not Exist\n");
		if (k >= 0)
		{
			for (k1=k-1; k1>k-5; k1--)
			{
				if (isBL(fw,k1))	// BL
				{
					uint32_t fadr = followBranch(fw,idx2adr(fw,k1),0x01000001);
					int k2 = adr2idx(fw,fadr);
					if (isLDR_PC(fw,k2))
					{
						tadr = LDR2val(fw,k2);
					}
				}
			}
		}
	}
	if (tadr != 0)
	{
		int tsiz = 2;
		if (fw->buf[adr2idx(fw,tadr)+2] == 0) tsiz = 3;
		
		uint32_t madr = fw->base + (fw->size*4-4);
		for (k=0; k<(tadr-fw->base)/4; k++)
		{
			if (isLDR_PC(fw,k))
			{
				uint32_t adr = LDR2val(fw,k);
				if ((adr > tadr) && (adr < madr))
				{
					madr = adr;
				}
			}
		}
		int tlen = (madr - tadr) / 4;
		if (tsiz == 2)
		{
			k1 = adr2idx(fw,tadr);
			for (k=0; k<tlen/3; k+=3)
			{
				if ((fw->buf[k1+k+1] == 0xFFFFFFFF) && (fw->buf[k1+k+4] == 0xFFFFFFFF))
				{
					tsiz = 3;
					break;
				}
			}
		}
		if (tlen > 50*tsiz) tlen = 50*tsiz;
		
		bprintf("// Bitmap masks and physw_status index values for SD_READONLY and USB power flags (for kbd.c).\n");
        if (fw->dryos_ver >= 49)
        {
            // Event ID's have changed in DryOS R49 **********
    		print_kval(fw,tadr,tsiz,tlen,0x20A,"SD_READONLY","_FLAG");
	    	print_kval(fw,tadr,tsiz,tlen,0x202,"USB","_MASK");
        }
        else
        {
    		print_kval(fw,tadr,tsiz,tlen,0x90A,"SD_READONLY","_FLAG");
	    	print_kval(fw,tadr,tsiz,tlen,0x902,"USB","_MASK");
        }
				
		uint32_t key_half = add_kmval(fw,tadr,tsiz,tlen,0,"KEY_SHOOT_HALF",0);
		add_kmval(fw,tadr,tsiz,tlen,1,"KEY_SHOOT_FULL",key_half);
		add_kmval(fw,tadr,tsiz,tlen,1,"KEY_SHOOT_FULL_ONLY",0);
		add_kmval(fw,tadr,tsiz,tlen,2,"KEY_ZOOM_IN",0);
		add_kmval(fw,tadr,tsiz,tlen,3,"KEY_ZOOM_OUT",0);
		add_kmval(fw,tadr,tsiz,tlen,4,"KEY_UP",0);
		add_kmval(fw,tadr,tsiz,tlen,5,"KEY_DOWN",0);
		add_kmval(fw,tadr,tsiz,tlen,6,"KEY_LEFT",0);
		add_kmval(fw,tadr,tsiz,tlen,7,"KEY_RIGHT",0);
		add_kmval(fw,tadr,tsiz,tlen,8,"KEY_SET",0);
		add_kmval(fw,tadr,tsiz,tlen,9,"KEY_MENU",0);
		add_kmval(fw,tadr,tsiz,tlen,0xA,"KEY_DISPLAY",0);
        if (fw->dryos_ver <= 47)
        {
    		add_kmval(fw,tadr,tsiz,tlen,0x601,"KEY_PLAYBACK",0);
	    	add_kmval(fw,tadr,tsiz,tlen,0x600,"KEY_POWER",0);
	    	add_kmval(fw,tadr,tsiz,tlen,0x12,"KEY_VIDEO",0);
        }
        else
        {
    		add_kmval(fw,tadr,tsiz,tlen,0x101,"KEY_PLAYBACK",0);
	    	add_kmval(fw,tadr,tsiz,tlen,0x100,"KEY_POWER",0);
            if (fw->dryos_ver == 49)
            {
                add_kmval(fw,tadr,tsiz,tlen,0x19,"KEY_VIDEO",0);
            }
            else if(fw->dryos_ver == 50)
            {
                add_kmval(fw,tadr,tsiz,tlen,0x1A,"KEY_VIDEO",0);
                add_kmval(fw,tadr,tsiz,tlen,0x14,"KEY_HELP",0);
            }	
        }

		bprintf("\n// Keymap values for kbd.c. Additional keys may be present, only common values included here.\n");
		print_kmvals();
    }
}

//------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    firmware fw;
    int k;
    int ret = 0;
    const char *curr_name;

    clock_t t1 = clock();

    if ((argc < 4) || (argc > 5))
        usage("args");

    out_fp = fopen(argv[3],"w");
    if (out_fp == NULL) usage("failed to open outputfile");

    load_stubs2();
    //print_stubs(stubs2);
	load_stubs_min();
    //print_stubs(stubs_min);
	load_modemap();
    //print_stubs(modemap);
    load_platform();
	
    bprintf("// !!! THIS FILE IS GENERATED. DO NOT EDIT. !!!\n");
    bprintf("#include \"stubs_asm.h\"\n\n");

    load_firmware(&fw,argv[1],argv[2],(argc==5)?argv[4]:0);

	out_hdr = 1;
	bprintf("// Stubs below should be checked. Stub not matched 100%%, or difference found to current 'stubs_entry_2.S'\n");
	bprintf("//    Name                                     Address      Rule  %%  Comp to stubs_entry_2.S\n");
	out_hdr = 0;
	bprintf("// Stubs below matched 100%%.\n");
	bprintf("//    Name                                     Address                Comp to stubs_entry_2.S\n");

    for (k = 0; func_names[k].name; k++)
    {
        count = 0;
        curr_name = func_names[k].name;

		find_matches(&fw, curr_name);

		if ((fw.dryos_ver >= find_min_ver(curr_name)) && (fw.dryos_ver <= find_max_ver(curr_name)))
			print_results(curr_name,k);

		if (count == 0)
		{
			ret = 1;
		}
    }

    clock_t t2 = clock();

    find_modemap(&fw);
    find_stubs_min(&fw);
    find_lib_vals(&fw);
    find_key_vals(&fw);
    find_platform_vals(&fw);
    find_other_vals(&fw);

    printf("Time to generate stubs %.2f seconds\n",(double)(t2-t1)/(double)CLOCKS_PER_SEC);
	
	write_output();
	
    fclose(out_fp);

    return ret;
}

//------------------------------------------------------------------------------------------------------------
