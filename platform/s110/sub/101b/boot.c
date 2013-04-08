#include "lolevel.h"
#include "platform.h"
#include "core.h"
#include "dryos31.h"

#define offsetof(TYPE, MEMBER) ((int) &((TYPE *)0)->MEMBER)

const char * const new_sa = &_end;

// Forward declarations
void CreateTask_PhySw();
void CreateTask_spytask();
extern volatile int jogdial_stopped;
void JogDial_task_my(void);

/*
static void __attribute__((naked,noinline)) blink(int cnt)
{
	volatile long *p=(void*)0xC022C30C;
	int i;

	for(;cnt>0;cnt--){
		*p = *p | 0x21; // Turn on LED
		for(i=0;i<0x200000;i++){
			asm ("nop\n");
			asm ("nop\n");
		}
		*p = (*p & 0xFFFFFFDF) | 0x1; // Turn off LED
		for(i=0;i<0x200000;i++){
			asm ("nop\n");
			asm ("nop\n");
		}
	}
}
*/

extern void task_CaptSeq();
extern void task_InitFileModules();
extern void task_RotaryEncoder();
extern void task_MovieRecord();
extern void task_ExpDrv();

void __attribute__((naked,noinline)) boot() {
asm volatile (
      "    LDR     R1, =0xC0410000 \n" 
      "    MOV     R0, #0 \n" 
      "    STR     R0, [R1] \n" 
      "    MOV     R1, #0x78 \n" 
      "    MCR     p15, 0, R1, c1, c0 \n" 
      "    MOV     R1, #0 \n" 
      "    MCR     p15, 0, R1, c7, c10, 4 \n" 
      "    MCR     p15, 0, R1, c7, c5 \n" 
      "    MCR     p15, 0, R1, c7, c6 \n" 
      "    MOV     R0, #0x3D \n" 
      "    MCR     p15, 0, R0, c6, c0 \n" 
      "    MOV     R0, #0xC000002F \n" 
      "    MCR     p15, 0, R0, c6, c1 \n" 
      "    MOV     R0, #0x37 \n" 
      "    MCR     p15, 0, R0, c6, c2 \n" 
      "    MOV     R0, #0x40000037 \n" 
      "    MCR     p15, 0, R0, c6, c3 \n" 
      "    MOV     R0, #0x80000017 \n" 
      "    MCR     p15, 0, R0, c6, c4 \n" 
      "    LDR     R0, =0xF0000037 \n" 
      "    MCR     p15, 0, R0, c6, c5 \n" 
      "    LDR     R0, =0xD000002B \n" 
      "    MCR     p15, 0, R0, c6, c7 \n" 
      "    MOV     R0, #0x34 \n" 
      "    MCR     p15, 0, R0, c2, c0 \n" 
      "    MOV     R0, #0x34 \n" 
      "    MCR     p15, 0, R0, c2, c0, 1 \n" 
      "    MOV     R0, #0x34 \n" 
      "    MCR     p15, 0, R0, c3, c0 \n" 
      "    LDR     R0, =0x33333330 \n" 
      "    MCR     p15, 0, R0, c5, c0, 2 \n" 
      "    LDR     R0, =0x33333330 \n" 
      "    MCR     p15, 0, R0, c5, c0, 3 \n" 
      "    MRC     p15, 0, R0, c1, c0 \n" 
      "    ORR     R0, R0, #0x1000 \n" 
      "    ORR     R0, R0, #4 \n" 
      "    ORR     R0, R0, #1 \n" 
      "    MCR     p15, 0, R0, c1, c0 \n" 
      "    MOV     R1, #0x80000006 \n" 
      "    MCR     p15, 0, R1, c9, c1 \n" 
      "    MOV     R1, #6 \n" 
      "    MCR     p15, 0, R1, c9, c1, 1 \n" 
      "    MRC     p15, 0, R1, c1, c0 \n" 
      "    ORR     R1, R1, #0x50000 \n" 
      "    MCR     p15, 0, R1, c1, c0 \n" 
      "    LDR     R2, =0xC0200000 \n" 
      "    MOV     R1, #1 \n" 
      "    STR     R1, [R2, #0x10C] \n" 
      "    MOV     R1, #0xFF \n" 
      "    STR     R1, [R2, #0xC] \n" 
      "    STR     R1, [R2, #0x1C] \n" 
      "    STR     R1, [R2, #0x2C] \n" 
      "    STR     R1, [R2, #0x3C] \n" 
      "    STR     R1, [R2, #0x4C] \n" 
      "    STR     R1, [R2, #0x5C] \n" 
      "    STR     R1, [R2, #0x6C] \n" 
      "    STR     R1, [R2, #0x7C] \n" 
      "    STR     R1, [R2, #0x8C] \n" 
      "    STR     R1, [R2, #0x9C] \n" 
      "    STR     R1, [R2, #0xAC] \n" 
      "    STR     R1, [R2, #0xBC] \n" 
      "    STR     R1, [R2, #0xCC] \n" 
      "    STR     R1, [R2, #0xDC] \n" 
      "    STR     R1, [R2, #0xEC] \n" 
      "    STR     R1, [R2, #0xFC] \n" 
      "    LDR     R1, =0xC0400008 \n" 
      "    LDR     R2, =0x430005 \n" 
      "    STR     R2, [R1] \n" 
      "    MOV     R1, #1 \n" 
      "    LDR     R2, =0xC0243100 \n" 
      "    STR     R2, [R1] \n" 
      "    LDR     R2, =0xC0242010 \n" 
      "    LDR     R1, [R2] \n" 
      "    ORR     R1, R1, #1 \n" 
      "    STR     R1, [R2] \n" 
      "    LDR     R0, =0xF896E790 \n" 
      "    LDR     R1, =0x685000 \n" 
      "    LDR     R3, =0x6B3264 \n" 
"loc_F8000144:\n"
      "    CMP     R1, R3 \n" 
      "    LDRCC   R2, [R0], #4 \n" 
      "    STRCC   R2, [R1], #4 \n" 
      "    BCC     loc_F8000144 \n" 
      "    LDR     R0, =0xF8953CD0 \n" 
      "    LDR     R1, =0x1900 \n" 
      "    LDR     R3, =0x1C3C0 \n" 
"loc_F8000160:\n"
      "    CMP     R1, R3 \n" 
      "    LDRCC   R2, [R0], #4 \n" 
      "    STRCC   R2, [R1], #4 \n" 
      "    BCC     loc_F8000160 \n" 
      "    LDR     R1, =0x28C088 \n" 
      "    MOV     R2, #0 \n" 
"loc_F8000178:\n"
      "    CMP     R3, R1 \n" 
      "    STRCC   R2, [R3], #4 \n" 
      "    BCC     loc_F8000178 \n" 
		/* Install task hooks via 0x193x is not possible with this new drysos version
		   So the below call patches the taskcreate functions in RAM to install our
		   hook TODO: FIXME: find out if this is the case or old style hooks are still possible
		*/
//      "    BL      HookIntoTaskCreateFunktion\n"
      "    B       sub_F800038C_my \n" 
//      "    B       sub_F800038C \n" 
    );
}

/* ATTENTION: This function does not save any registers,
              the calling functions saves&restores the used registers
*/
void __attribute__((naked,noinline)) code_copy() {
    asm volatile (
"copy_loop:\n" // Copy patch code to RAM location
        "LDR     R3, [R0], #4\n"
        "STR     R3, [R1], #4\n"
        "SUBS    R2, R2, #1\n"
        "BNE     copy_loop\n"
		"BX      LR\n"
		);
}
/* ERR99: PATCH the TASK CREATE functions in RAM to install our hooks.
   To catch all TaskCreate calls (need edto install our own task functions),
   it is necessary to patch to different TaskCreate functions in RAM. 
 */

void __attribute__((naked,noinline)) HookIntoTaskCreateFunktion() {

    asm volatile (
		"STMFD   SP!, {R0-R3,LR}\n" // Save all register which are used below
		/* Install HOOK #1 */
		"LDR     R0, = PatchCodeHook1\n" // R0 = Start adr. of our hook code call (SRC)
		"LDR     R1, =0x68AF04\n"	     // Get start adress of function to patch (DST)
		"MOV     R2, #(PatchCodeHook1End-PatchCodeHook1)/4\n" // WORDs to copy (lenght of patch bytes/4)
		"BL      code_copy\n"

		/* Install HOOK #2 */
		"LDR     R0, = PatchCodeHook2\n" // Get start adr. of our hook code call (SRC)
		"LDR     R1, = 0x68AB8C\n"	// Get start adress of function to patch (DST)
		"MOV     R2, #(PatchCodeHook2End-PatchCodeHook2)/4\n" // WORDs to copy (lenght of patch bytes/4)
		"BL      code_copy\n"

		"LDMFD   SP!, {R0-R3,PC}\n" // Restore all used registers

		/* Our hook code payload for HOOK #1*/
"PatchCodeHook1:\n"
		"ldr PC,[PC,#-0x4]\n"	// Do jump to absolute adress TaskHookFnc1
		".long TaskHookFnc1\n"
"PatchCodeHook1End:\n"
		/* Our hook code payload for HOOK #2*/
"PatchCodeHook2:\n"
		"ldr PC,[PC,#-0x4]\n"  // Do jump to absolute adress TaskHookFnc2
		".long TaskHookFnc2\n"
"PatchCodeHook2End:\n"
	 );
}

/* Task HOOK function #1. Called if TaskCreate is called. */
void __attribute__((naked,noinline)) TaskHookFnc1() {
    asm volatile (

	"STMFD   SP!, {R0-R1}\n"
	//R3 = Pointer to task function to create

	/*** INSTALL capt_seq_task() hook **
	"LDR     R0, =task_CaptSeq\n"	// DryOS original code function ptr.
	"LDR     R1, =capt_seq_task\n"	// our task function base ptr.
	"CMP     R0, R3\n"	       // is the given taskptr.  equal to our searched function?
	"MOVEQ   R3, R1\n"        // if so, overwrite original task pointer in R12 with our task (R1)
	"BEQ     exitHook\n"       // below compares not necessary if this check has found something.
    */
	/*** INSTALL exp_drv_task() hook **
	"LDR     R0, =task_ExpDrv\n"
	"LDR     R1, =exp_drv_task\n"
	"CMP     R0, R3\n"
	"MOVEQ   R3, R1\n"
	"BEQ     exitHook\n"
    */
	/*** INSTALL JogDial() hook **
	"LDR     R0, =task_RotaryEncoder\n"
	"LDR     R1, =JogDial_task_my\n"
	"CMP     R0, R3\n"
	"MOVEQ   R3, R1\n"
	"BEQ     exitHook\n"
    */

	/*** INSTALL movie_record_task() hook **
	"LDR     R0, =task_MovieRecord\n"
	"LDR     R1, =movie_record_task\n"
	"CMP     R0, R3\n"
	"MOVEQ   R3, R1\n"
    */
"exitHook:\n" // restore overwritten registers
	"LDMFD   SP!, {R0-R1}\n"

	/* Original Task create function: */
    "STMFD   SP!, {R3-R5,LR}\n"
    "MOV     R12, R3\n"
    "LDR     R3, [SP,#0x10]\n"
    "STR     R3, [SP]\n"
    "MOV     R3, R12\n"
    "BL      sub_68AB8C\n"
    "CMP     R0, #5\n"
    "MOV     R4, R0\n"
    "MOVEQ   R1, #0x13\n"
    "LDREQ   R0, =0x68B0F4\n" // TODO but probably correct
    "BLEQ    sub_68B104\n" // TODO but probably correct
    "MOV     R0, R4\n"
    "LDMFD   SP!, {R3-R5,PC}\n"
	);
}


void __attribute__((naked,noinline)) TaskHookFnc2() {
    asm volatile (

	"STMFD   SP!, {R0-R1}\n"
	//R3 = Pointer to task to create
	"LDR     R0, =task_InitFileModules\n"
	"LDR     R1, =init_file_modules_task\n"
	"CMP     R0, R3\n"
	"MOVEQ   R3, R1\n"

"exitHook2:\n"
	"LDMFD   SP!, {R0-R1}\n"

	/* ORIGINAL CODE: */  // TODO: This is SX50, probably good for S110 too? (with FF to F8 change)
    "                STMFD   SP!, {R1-R9,LR}\n"
    "                MOV     R4, R0\n"
    "                LDR     R0, =0xF80003FC\n"
    "                LDR     R8, [SP,#0x28]\n"
    "                LDR     R0, [R0]\n"
    "                MOV     R6, R1\n"
    "                LDR     R0, [R0]\n"
    "                MOV     R5, R2\n"
    "                CMP     R0, #0\n"
    "                MOV     R7, R3\n"
    "                MOVGT   R1, #0xA3 \n"
    "                LDRGT   R0, =0x68ADB8\n"
    "                BLGT    sub_68B104\n"
    "                LDR     R0, =0x1B08\n"
    "                CMP     R4, #0\n"
    "                LDREQ   R4, [R0]\n"
    "                SUB     R1, R6, #1\n"
    "                CMP     R1, #0x20 \n"
    "                BCS     loc_68ABE0\n"
    "                CMP     R7, #0\n"
    "                BNE     loc_68ABE8\n"
	"loc_68ABE0:\n"
    "                MOV     R0, #3\n"
    "                LDMFD   SP!, {R1-R9,PC}\n"
	"loc_68ABE8:\n"
    "                CMP     R5, #0\n"
    "                LDREQ   R5, [R0,#4]\n"
    "                MOV     R2, #0\n"
    "                STMFA   SP, {R2,R4}\n"
    "                MOV     R3, R5\n"
    "                MOV     R1, R8\n"
    "                MOV     R0, R7\n"
    "                STR     R6, [SP]\n"
    "                BL      sub_6855A4\n"
    "                CMN     R0, #1\n"
    "                MOV     R4, R0\n"
    "                BNE     loc_68AC2C\n"
    "                MOV     R1, #0xB9 \n"
    "                LDR     R0, =0x68ADB8\n"
    "                BL      sub_68B104\n"
    "                MOV     R0, #5\n"
    "                LDMFD   SP!, {R1-R9,PC}\n"
	"loc_68AC2C:                              \n"
    "                MOV     R0, R4\n"
    "                BL      sub_6858AC\n"
    "                MOV     R0, R4,LSL#1\n"
    "                LDMFD   SP!, {R1-R9,PC}\n"
	);
}


void __attribute__((naked,noinline)) sub_F800038C_my() {

 if ( (*(int*)0xC022F48C & 0x800000) )
		*(int*)(0x2ff0+0x4) = 0x200000;  // Playmode "PhySwConfig.c" S100 f807b174
	else
		*(int*)(0x2ff0+0x4) = 0x100000; // Shootingmode
   asm volatile (
      "    LDR     R0, =0xF8000404 \n" 
      "    MOV     R1, #0 \n" 
      "    LDR     R3, =0xF800043C \n" 
"loc_F8000398:\n"
      "    CMP     R0, R3 \n" 
      "    LDRCC   R2, [R0], #4 \n" 
      "    STRCC   R2, [R1], #4 \n" 
      "    BCC     loc_F8000398 \n" 
      "    LDR     R0, =0xF800043C \n" 
      "    MOV     R1, #0x4B0 \n" 
      "    LDR     R3, =0xF8000624 \n" 
"loc_F80003B4:\n"
      "    CMP     R0, R3 \n" 
      "    LDRCC   R2, [R0], #4 \n" 
      "    STRCC   R2, [R1], #4 \n" 
      "    BCC     loc_F80003B4 \n" 
      "    MOV     R0, #0xD2 \n" 
      "    MSR     CPSR_cxsf, R0 \n" 
      "    MOV     SP, #0x1000 \n" 
      "    MOV     R0, #0xD3 \n" 
      "    MSR     CPSR_cxsf, R0 \n" 
      "    MOV     SP, #0x1000 \n" 
      "    LDR     R0, =0x698 \n" 
      "    LDR     R2, =0xEEEEEEEE \n" 
      "    MOV     R3, #0x1000 \n" 
"loc_F80003E8:\n"
      "    CMP     R0, R3 \n" 
      "    STRCC   R2, [R0], #4 \n" 
      "    BCC     loc_F80003E8 \n" 
//      "    BL      sub_F800116C \n"
      "    BL      sub_F800116C_my \n"
	);
}

void __attribute__((naked,noinline)) sub_F800116C_my() {

	asm volatile (
      "    STR     LR, [SP, #-4]! \n" 
      "    SUB     SP, SP, #0x74 \n" 
      "    MOV     R1, #0x74 \n" 
      "    MOV     R0, SP \n" 
      "    BL      sub_F86488EC \n" 
      "    MOV     R0, #0x83000 \n" 
      "    STR     R0, [SP, #4] \n" 
	#if defined(OPT_CHDK_IN_EXMEM) 
      "    LDR     R0, =0x28C088 \n" 
	#else 
      "    LDR     R0, =new_sa \n"	
      "    LDR     R0, [R0] \n"	
	#endif 
      "    LDR     R2, =0x53F15C \n" 
      "    STR     R0, [SP, #8] \n" 
      "    SUB     R0, R2, R0 \n" 
      "    STR     R0, [SP, #0xC] \n" 
      "    MOV     R0, #0x22 \n" 
      "    STR     R0, [SP, #0x18] \n" 
      "    MOV     R0, #0x98 \n" 
      "    STR     R0, [SP, #0x1C] \n" 
      "    LDR     R0, =0x1E2 \n" 
      "    LDR     R1, =0x549C00 \n" 
      "    STR     R2, [SP, #0x10] \n" 
      "    STR     R0, [SP, #0x20] \n" 
      "    MOV     R0, #0xF6 \n" 
      "    STR     R1, [SP] \n" 
      "    STR     R1, [SP, #0x14] \n" 
      "    STR     R0, [SP, #0x24] \n" 
      "    MOV     R0, #0xB6 \n" 
      "    STR     R0, [SP, #0x28] \n" 
      "    MOV     R0, #0x85 \n" 
      "    STR     R0, [SP, #0x2C] \n" 
      "    MOV     R0, #0x40 \n" 
      "    STR     R0, [SP, #0x30] \n" 
      "    MOV     R0, #4 \n" 
      "    STR     R0, [SP, #0x34] \n" 
      "    MOV     R0, #0x10 \n" 
      "    STR     R0, [SP, #0x5C] \n" 
      "    MOV     R0, #0x800 \n" 
      "    STR     R0, [SP, #0x60] \n" 
      "    MOV     R0, #0xA0 \n" 
      "    STR     R0, [SP, #0x64] \n" 
      "    MOV     R0, #0x280 \n" 
      "    STR     R0, [SP, #0x68] \n" 
      //"    LDR     R1, =0xF8004264 \n" 
      "    LDR     R1, =sub_F8004264_my \n" 
      "    MOV     R2, #0 \n" 
      "    MOV     R0, SP \n" 
      "    BL      sub_F86488F4 \n" 
      "    ADD     SP, SP, #0x74 \n" 
      "    LDR     PC, [SP], #4 \n" 
	 );
}

void __attribute__((naked,noinline)) sub_F8004264_my() {
     
	 asm volatile (
      "    STMFD   SP!, {R4,LR} \n" 
      "    BL      sub_F8000AE8 \n" 
      "    BL      sub_F8005450 \n" 
      "    CMP     R0, #0 \n" 
      "    LDRLT   R0, =0xF80043D4 \n" 
      "    BLLT    sub_F80043B4 \n" 
      "    BL      sub_F8003E9C \n" 
      "    CMP     R0, #0 \n" 
      "    LDRLT   R0, =0xF80043DC \n" 
      "    BLLT    sub_F80043B4 \n" 
      "    LDR     R0, =0xF80043EC \n" 
      "    BL      sub_F8003F84 \n" 
      "    CMP     R0, #0 \n" 
      "    LDRLT   R0, =0xF80043F4 \n" 
      "    BLLT    sub_F80043B4 \n" 
      "    LDR     R0, =0xF80043EC \n" 
      "    BL      sub_F80029A4 \n" 
      "    CMP     R0, #0 \n" 
      "    LDRLT   R0, =0xF8004408 \n" 
      "    BLLT    sub_F80043B4 \n" 
      "    BL      sub_F8004DEC \n" 
      "    CMP     R0, #0 \n" 
      "    LDRLT   R0, =0xF8004414 \n" 
      "    BLLT    sub_F80043B4 \n" 
      "    BL      sub_F8007A50 \n" 
      "    CMP     R0, #0 \n" 
      "    LDRLT   R0, =0xF8004420 \n" 
      "    BLLT    sub_F80043B4 \n" 
      "    BL      sub_F8007AC8 \n" 
      "    CMP     R0, #0 \n" 
      "    LDRLT   R0, =0xF800442C \n" 
      "    BLLT    sub_F80043B4 \n" 
      "    BL      sub_F8006938 \n" 
      "    CMP     R0, #0 \n" 
      "    LDRLT   R0, =0xF800443C \n" 
      "    BLLT    sub_F80043B4 \n" 
      "    BL      sub_F8001664 \n" 
      "    CMP     R0, #0 \n" 
      "    LDRLT   R0, =0xF8004448 \n" 
      "    BLLT    sub_F80043B4 \n" 
      "    LDMFD   SP!, {R4,LR} \n" 
//      "    B       sub_F800CCB4 \n" 
      "    B       sub_F800CCB4_my \n" 
	);
};

void __attribute__((naked,noinline)) sub_F800CCB4_my() {

	asm volatile (
      "    STMFD   SP!, {R3,LR} \n" 
      "    BL      sub_F803B40C \n" 
      "    CMP     R0, #0 \n" 
      "    BNE     loc_F800CCD4 \n" 
      "    BL      sub_F8023348 \n" 
      "    CMP     R0, #0 \n" 
      "    MOVNE   R0, #1 \n" 
      "    BNE     loc_F800CCD8 \n" 
"loc_F800CCD4:\n"
      "    MOV     R0, #0 \n" 
"loc_F800CCD8:\n"
//      "    BL      sub_F80211F4 \n" 
      "    BL      sub_F80211F4_my \n" 
      "    CMP     R0, #0 \n" 
      "    BNE     loc_F800CCF8 \n" 
      "    BL      sub_F80205CC \n" 
      "    LDR     R1, =0x83DC00 \n" 
      "    MOV     R0, #0x54 \n" 
      "    BL      sub_F80399A0 \n" 
"loc_F800CCF4:\n"
      "    B       loc_F800CCF4 \n" 
"loc_F800CCF8:\n"
      "    BL      sub_F8648AB4 \n" 
      "    LDR     R1, =0x5CE000 \n" 
      "    MOV     R0, #0 \n" 
      "    BL      sub_F803979C \n" 
      "    BL      sub_F8648ABC \n" 
      "    MOV     R3, #0 \n" 
      "    STR     R3, [SP] \n" 
//      "    LDR     R3, =0xF800CC4C \n" 
      "    LDR     R3, =task_Startup_my \n" 
      "    MOV     R2, #0 \n" 
      "    MOV     R1, #0x19 \n" 
      "    LDR     R0, =0xF800CD3C \n" 
      "    BL      sub_F864899C \n" 
      "    MOV     R0, #0 \n" 
      "    LDMFD   SP!, {R3,PC} \n" 

	 );
}

void __attribute__((naked,noinline)) sub_F80211F4_my() {

   asm volatile (
      "    STMFD   SP!, {R4-R6,LR} \n" 
      "    MOV     R6, R0 \n" 
      "    BL      sub_F807B16C \n" 
      "    MOV     R0, #0x36 \n" 
      "    BL      sub_F807BC94 \n" 
      "    MOV     R4, #1 \n" 
      "    BIC     R5, R4, R0 \n" 
      "    MOV     R0, #0x37 \n" 
      "    BL      sub_F807BC94 \n" 
      "    CMP     R6, #0 \n" 
      "    BIC     R4, R4, R0 \n" 
      "    BEQ     loc_F802122C \n" 
      "    ORRS    R0, R5, R4 \n" 
      "    LDMEQFD SP!, {R4-R6,PC} \n" 
"loc_F802122C:\n"
      "    BL      sub_F803B40C \n" 
      "    MOV     R2, R0 \n" 
      "    MOV     R3, #0 \n" 
      "    MOV     R1, R4 \n" 
      "    MOV     R0, R5 \n" 
      //"    BL      sub_F807B174 \n" // Disable StartUpChecks
      "    BL      sub_F807B170 \n" 
      "    MOV     R0, #1 \n" 
      "    LDMFD   SP!, {R4-R6,PC} \n"
	 );
}

void __attribute__((naked,noinline)) task_Startup_my() { //TODO: check

   asm volatile (
      "    STMFD   SP!, {R4,LR} \n" 
      "    BL      sub_F8004954 \n" 
      "    BL      sub_F8022408 \n" 
      "    BL      sub_F8020434 \n" 
      "    BL      sub_F803B458 \n" 
      "    BL      sub_F803B644 \n" 
      //"    BL      sub_F803B4EC \n" // load DISKBOOT.BIN
      "    BL      sub_F803B7BC \n" 
      "    BL      sub_F803B988 \n" 
      "    BL      sub_F803B7B0 \n" 
      "    BL      sub_F803B674 \n" 
      "    BL      sub_F80396D0 \n" 
      "    BL      sub_F803B990 \n" 
      "    BL      CreateTask_spytask\n"
//	  "    BL      sub_F8021094 \n"  //"taskCreatePhySw
      "    BL      taskcreatePhySw_my\n"
      "    BL      sub_F8021094 \n" 
      "    BL      sub_F8036DF8 \n" 
      "    BL      sub_F803B9A8 \n" 
      "    BL      sub_F801DF88 \n" 
      "    BL      sub_F801FDA4 \n" 
      "    BL      sub_F803B198 \n" 
      "    BL      sub_F80203E8 \n" 
      "    BL      sub_F801FD40 \n" 
      "    BL      sub_F803B7B4 \n" 
      "    BL      sub_F801ECA4 \n" 
      "    BL      sub_F801FD04 \n" 
      "    LDMFD   SP!, {R4,LR} \n" 
      "    B       sub_F8004A90 \n"
    );
}

void __attribute__((naked,noinline)) taskcreatePhySw_my() {

    asm volatile (
      "    STMFD   SP!, {R3-R5,LR} \n" 
      "    LDR     R4, =0x1E80 \n" 
      "    LDR     R0, [R4, #4] \n" 
      "    CMP     R0, #0 \n" 
      "    BNE     loc_F80210C8 \n" 
      "    MOV     R3, #0 \n" 
      "    STR     R3, [SP] \n" 
      //"    LDR     R3, =0xF8021060 \n" 
      //"    MOV     R2, #0x800 \n" 
      "    LDR     R3, =mykbd_task\n"
      "    MOV     R2, #0x2000\n"             // stack size for new task_PhySw so we don't have to do stack switch

      "    MOV     R1, #0x17 \n" 
      "    LDR     R0, =0xF8021344 \n" 
      "    BL      sub_F8648A8C \n" 
      "    STR     R0, [R4, #4] \n" 
"loc_F80210C8:\n"
      "    BL      sub_F807C0E4 \n"  // Jogdial task create called from here
      "    BL      sub_F80D501C \n" 
      "    BL      sub_F8023298 \n" 
      "    CMP     R0, #0 \n" 
      "    BNE     loc_F80210E8 \n" 
      "    LDR     R1, =0x41628 \n" 
      "    MOV     R0, #0 \n" 
      "    BL      sub_F80D4F88 \n" 
"loc_F80210E8:\n"
      "    LDMFD   SP!, {R3-R5,PC} \n" 
    );
}

void spytask(long ua, long ub, long uc, long ud, long ue, long uf) {
    
	core_spytask();
}

void CreateTask_spytask() {

        _CreateTask("SpyTask", 0x19, 0x2000, core_spytask, 0);
};

void __attribute__((naked,noinline)) init_file_modules_task() {
    asm volatile(
 // f80675a8
      "    STMFD   SP!, {R4-R6,LR} \n" 
      "    BL      sub_F80D7584 \n" 
      "    LDR     R5, =0x5006 \n" 
      "    MOVS    R4, R0 \n" 
      "    MOVNE   R1, #0 \n" 
      "    MOVNE   R0, R5 \n" 
      "    BLNE    sub_F80D90A8 \n" 
      "    BL      sub_F80D75B0 \n" 
      "    BL      core_spytask_can_start\n" //
      "    CMP     R4, #0 \n" 
      "    LDMNEFD SP!, {R4-R6,PC} \n" 
      "    MOV     R0, R5 \n" 
      "    LDMFD   SP!, {R4-R6,LR} \n" 
      "    MOV     R1, #0 \n" 
      "    B       sub_F80D90A8 \n" 
    );
}

short *jog_position;
void __attribute__((naked,noinline)) JogDial_task_my() { //TODO: this is SX50HS
 asm volatile(
	        "STMFD   SP!, {R4-R11,LR}\n"
	        "SUB     SP, SP, #0x1C\n"
	        "BL      sub_FF05DFF4\n"
	        "LDR     R12, =0x2E30\n"
	        "LDR     R6, =0xFF49E73C\n"
	        "MOV     R0, #0\n"
	        "ADD     R10, SP, #8\n"
	        "ADD     R9, SP, #0xC\n"

			 // Save pointer for kbd.c routine
			 "LDR R2, =jog_position \n"
			 "STR R9, [R2] \n"

	"loc_FF05DC7C:\n"
	        "ADD     R2, SP, #0x14\n"
	        "MOV     R1, #0\n"
	        "ADD     R4, R2, R0,LSL#1\n"
	        "ADD     R3, SP, #0x10\n"
	        "STRH    R1, [R4]\n"
	        "ADD     R4, R3, R0,LSL#1\n"
	        "STRH    R1, [R4]\n"
	        "STR     R1, [R9,R0,LSL#2]\n"
	        "STR     R1, [R10,R0,LSL#2]\n"
	        "ADD     R0, R0, #1\n"
	        "CMP     R0, #1\n"
	        "BLT     loc_FF05DC7C\n"
	"loc_FF05DCAC:\n"
	        "LDR     R0, =0x2E30\n"
	        "MOV     R2, #0\n"
	        "LDR     R0, [R0,#8]\n"
	        "MOV     R1, SP\n"
	        "BL      sub_68BDE0\n"
	        "CMP     R0, #0\n"
	        "LDRNE   R1, =0x20B\n"
	        "LDRNE   R0, =0xFF05DF0C \n"
	        "BLNE    sub_68B104\n"
	        "LDR     R0, [SP]\n"
	        "AND     R4, R0, #0xFF\n"
	        "AND     R0, R0, #0xFF00\n"
	        "CMP     R0, #0x100\n"
	        "BEQ     loc_FF05DD1C\n"
	        "CMP     R0, #0x200\n"
	        "BEQ     loc_FF05DD54\n"
	        "CMP     R0, #0x300\n"
	        "BEQ     loc_FF05DF58\n"
	        "CMP     R0, #0x400\n"
	        "BNE     loc_FF05DCAC\n"
	        "CMP     R4, #0\n"
	        "LDRNE   R1, =0x285\n"
	        "LDRNE   R0, =0xFF05DF0C \n"
	        "BLNE    sub_68B104\n"
	        "RSB     R0, R4, R4,LSL#3\n"
	        "LDR     R0, [R6,R0,LSL#2]\n"
	"loc_FF05DD14:\n"
	        "BL      sub_FF05DFD8\n"
	        "B       loc_FF05DCAC\n"
	"loc_FF05DD1C:\n"
#ifndef TEST_RUN
	//PATCH BEGIN
	"labelA:\n"
	                 "LDR     R0, =jogdial_stopped\n"
	                 "LDR     R0, [R0]\n"
	                 "CMP     R0, #1\n"
	                 "BNE     labelB\n"
	                 "MOV     R0, #40\n"
	                 "BL      sub_68C4E4\n" // SleepTask
	                 "B       labelA\n"
	 "labelB:\n"

	//PATCH END
#endif
	        "LDR     R0, =0x2E3C\n"
	        "LDR     R0, [R0,R4,LSL#2]\n"
	        "BL      sub_FF020DF4\n"
	        "LDR     R2, =0xFF05DBA8\n"
	        "ORR     R3, R4, #0x200\n"
	        "MOV     R1, R2\n"
	        "MOV     R0, #0x28 \n"
	        "BL      sub_FF020D10\n"
	        "TST     R0, #1\n"
	        "CMPNE   R0, #0x15\n"
	        "STR     R0, [R10,R4,LSL#2]\n"
	        "BEQ     loc_FF05DCAC\n"
	        "LDR     R1, =0x21D\n"
	        "B       loc_FF05DEF8\n"
	"loc_FF05DD54:\n"
	        "RSB     R5, R4, R4,LSL#3\n"
	        "LDR     R0, [R6,R5,LSL#2]\n"
	        "LDR     R1, =0xC0240000\n"
	        "ADD     R0, R1, R0,LSL#8\n"
	        "LDR     R0, [R0,#0x104]\n"
	        "MOV     R1, R0,ASR#16\n"
	        "ADD     R0, SP, #0x14\n"
	        "ADD     R11, R0, R4,LSL#1\n"
	        "ADD     R0, SP, #0x10\n"
	        "ADD     R0, R0, R4,LSL#1\n"
	        "STRH    R1, [R11]\n"
	        "STR     R0, [SP,#0x18]\n"
	        "LDRSH   R3, [R0]\n"
	        "SUB     R2, R1, R3\n"
	        "CMP     R2, #0\n"
	        "BNE     loc_FF05DDD8\n"
	        "LDR     R0, [R9,R4,LSL#2]\n"
	        "CMP     R0, #0\n"
	        "BEQ     loc_FF05DEB4\n"
	        "LDR     R7, =0x2E3C\n"
	        "LDR     R0, [R7,R4,LSL#2]\n"
	        "BL      sub_FF020DF4\n"
	        "LDR     R2, =0xFF05DBB4\n"
	        "ORR     R3, R4, #0x300\n"
	        "MOV     R1, R2\n"
	        "MOV     R0, #0x1F4\n"
	        "BL      sub_FF020D10\n"
	        "TST     R0, #1\n"
	        "CMPNE   R0, #0x15\n"
	        "STR     R0, [R7,R4,LSL#2]\n"
	        "BEQ     loc_FF05DEB4\n"
	        "LDR     R1, =0x236\n"
	        "B       loc_FF05DEAC\n"
	"loc_FF05DDD8:\n"
	        "MOV     R0, R2\n"
	        "RSBLT   R0, R0, #0\n"
	        "MOVLE   R7, #0\n"
	        "MOVGT   R7, #1\n"
	        "CMP     R0, #0xFF\n"
	        "BLS     loc_FF05DE14\n"
	        "LDR     R0, =0x7FFF\n"
	        "CMP     R2, #0\n"
	        "SUBLE   R0, R0, R3\n"
	        "ADDLE   R0, R0, R1\n"
	        "SUBGT   R0, R0, R1\n"
	        "ADDGT   R0, R0, R3\n"
	        //"MOV     R1, byte_FFFF7FFF\n"
		    "MOV     R1, #0xFFFF7FFF \n"
	        "SUB     R0, R0, R1\n"
	        "EOR     R7, R7, #1\n"
	"loc_FF05DE14:\n"
	        "STR     R0, [SP,#4]\n"
	        "LDR     R0, [R9,R4,LSL#2]\n"
	        "CMP     R0, #0\n"
	        "ADDEQ   R0, R6, R5,LSL#2\n"
	        "LDREQ   R0, [R0,#8]\n"
	        "BEQ     loc_FF05DE4C\n"
	        "ADD     R8, R6, R5,LSL#2\n"
	        "ADD     R1, R8, R7,LSL#2\n"
	        "LDR     R1, [R1,#0x10]\n"
	        "CMP     R1, R0\n"
	        "BEQ     loc_FF05DE50\n"
	        "LDR     R0, [R8,#0xC]\n"
	        "BL      sub_FF019F18\n"
	        "LDR     R0, [R8,#8]\n"
	"loc_FF05DE4C:\n"
	        "BL      sub_FF019F18\n"
	"loc_FF05DE50:\n"
	        "ADD     R0, R6, R5,LSL#2\n"
	        "ADD     R7, R0, R7,LSL#2\n"
	        "LDR     R0, [R7,#0x10]\n"
	        "LDR     R1, [SP,#4]\n"
	        "BL      sub_FF019F28\n"
	        "LDR     R0, [R7,#0x10]\n"
	        "LDR     R7, =0x2E3C\n"
	        "STR     R0, [R9,R4,LSL#2]\n"
	        "LDRH    R1, [R11]\n"
	        "LDR     R0, [SP,#0x18]\n"
	        "STRH    R1, [R0]\n"
	        "LDR     R0, [R7,R4,LSL#2]\n"
	        "BL      sub_FF020DF4\n"
	        "LDR     R2, =0xFF05DBB4\n"
	        "ORR     R3, R4, #0x300\n"
	        "MOV     R1, R2\n"
	        "MOV     R0, #0x1F4\n"
	        "BL      sub_FF020D10\n"
	        "TST     R0, #1\n"
	        "CMPNE   R0, #0x15\n"
	        "STR     R0, [R7,R4,LSL#2]\n"
	        "BEQ     loc_FF05DEB4\n"
	        "LDR     R1, =0x267\n"
	"loc_FF05DEAC:\n"
	        "LDR     R0, =0xFF05DF0C \n"
	        "BL      sub_68B104\n"
	"loc_FF05DEB4:\n"
	        "ADD     R0, R6, R5,LSL#2\n"
	        "LDR     R0, [R0,#0x18]\n"
	        "CMP     R0, #1\n"
	        "BNE     loc_FF05DF50\n"
	        "LDR     R0, =0x2E30\n"
	        "LDR     R0, [R0,#0x10]\n"
	        "CMP     R0, #0\n"
	        "BEQ     loc_FF05DF50\n"
	        "LDR     R2, =0xFF05DBA8\n"
	        "ORR     R3, R4, #0x400\n"
	        "MOV     R1, R2\n"
	        "BL      sub_FF020D10\n"
	        "TST     R0, #1\n"
	        "CMPNE   R0, #0x15\n"
	        "STR     R0, [R10,R4,LSL#2]\n"
	        "BEQ     loc_FF05DCAC\n"
	        "LDR     R1, =0x26E\n"
	"loc_FF05DEF8:\n"
	        "LDR     R0, =0xFF05DF0C \n"
	        "BL      sub_68B104\n"
	        "B       loc_FF05DCAC\n"

	 //
	"loc_FF05DF50:\n"
	        "LDR     R0, [R6,R5,LSL#2]\n"
	        "B       loc_FF05DD14\n"
	"loc_FF05DF58:\n"
	        "LDR     R0, [R9,R4,LSL#2]\n"
	        "CMP     R0, #0\n"
	        "MOVEQ   R1, #0x278\n"
	        "LDREQ   R0, =0xFF05DF0C \n"
	        "BLEQ    sub_68B104\n"
	        "RSB     R0, R4, R4,LSL#3\n"
	        "ADD     R0, R6, R0,LSL#2\n"
	        "LDR     R0, [R0,#0xC]\n"
	        "BL      sub_FF019F18\n"
	        "MOV     R0, #0\n"
	        "STR     R0, [R9,R4,LSL#2]\n"
	        "B       loc_FF05DCAC\n"
	 );


 }
