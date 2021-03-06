/*
 * capt_seq.c - auto-generated by CHDK code_gen.
 */
#include "lolevel.h"
#include "platform.h"
#include "core.h"

#define USE_STUBS_NRFLAG 1 // see stubs_min.S
#define NR_AUTO (0)        // have to explictly reset value back to 0 to enable auto
#define PAUSE_FOR_FILE_COUNTER 350  // Enable delay in capt_seq_hook_raw_here to ensure file counter is updated

#include "../../../generic/capt_seq.c"

/*************************************************************/
//** capt_seq_task @ 0xFF890CCC - 0xFF890F98, length=180
void __attribute__((naked,noinline)) capt_seq_task() {
asm volatile (
"    STMFD   SP!, {R3-R7,LR} \n"
"    LDR     R5, =0x1F514 \n"
"    LDR     R7, =0x35A0 \n"
"    MOV     R6, #0 \n"

"loc_FF890CDC:\n"
"    LDR     R0, [R7, #4] \n"
"    MOV     R2, #0 \n"
"    MOV     R1, SP \n"
"    BL      sub_FF829E30 /*_ReceiveMessageQueue*/ \n"
"    TST     R0, #1 \n"
"    BEQ     loc_FF890D0C \n"
"    LDR     R2, =0x443 \n"
"    LDR     R1, =0xFF890640 /*'SsShootTask.c'*/ \n"
"    MOV     R0, #0 \n"
"    BL      _DebugAssert \n"
"    BL      _ExitTask \n"
"    LDMFD   SP!, {R3-R7,PC} \n"

"loc_FF890D0C:\n"
"    LDR     R0, [SP] \n"
"    LDR     R0, [R0] \n"
"    CMP     R0, #1 \n"
"    CMPNE   R0, #0x23 \n"
"    CMPNE   R0, #0x19 \n"
"    BLNE    sub_FF9B605C \n"
"    LDR     R0, [SP] \n"
"    LDR     R1, [R0] \n"
"    CMP     R1, #0x25 \n"
"    ADDCC   PC, PC, R1, LSL#2 \n"
"    B       loc_FF890F5C \n"
"    B       loc_FF890DCC \n"
"    B       loc_FF890DE4 \n"
"    B       loc_FF890DF0 \n"
"    B       loc_FF890E04 \n"
"    B       loc_FF890DFC \n"
"    B       loc_FF890E10 \n"
"    B       loc_FF890E18 \n"
"    B       loc_FF890E20 \n"
"    B       loc_FF890E2C \n"
"    B       loc_FF890E58 \n"
"    B       loc_FF890E38 \n"
"    B       loc_FF890E44 \n"
"    B       loc_FF890E4C \n"
"    B       loc_FF890E60 \n"
"    B       loc_FF890E68 \n"
"    B       loc_FF890E70 \n"
"    B       loc_FF890E78 \n"
"    B       loc_FF890E80 \n"
"    B       loc_FF890E8C \n"
"    B       loc_FF890E94 \n"
"    B       loc_FF890E9C \n"
"    B       loc_FF890EA4 \n"
"    B       loc_FF890EAC \n"
"    B       loc_FF890EB4 \n"
"    B       loc_FF890EBC \n"
"    B       loc_FF890EC4 \n"
"    B       loc_FF890ED0 \n"
"    B       loc_FF890ED8 \n"
"    B       loc_FF890EE0 \n"
"    B       loc_FF890EE8 \n"
"    B       loc_FF890EF0 \n"
"    B       loc_FF890EFC \n"
"    B       loc_FF890F04 \n"
"    B       loc_FF890F10 \n"
"    B       loc_FF890F3C \n"
"    B       loc_FF890F48 \n"
"    B       loc_FF890F6C \n"

"loc_FF890DCC:\n"
"    BL      shooting_expo_iso_override\n"      // added
"    BL      sub_FF891508 \n"
"    BL      shooting_expo_param_override\n"    // added
"    BL      sub_FF88E20C \n"
//To do: Check if this is needed.
//"    MOV     R0, #0\n"                          // added
//"    STR     R0, [R5,#0x28]\n"                  // added, fixes overrides behavior at short shutter press (from S95)
"    LDR     R0, [R5, #0x28] \n"
"    CMP     R0, #0 \n"
"    BLNE    sub_FF9B4F70_my \n"  // --> Patched. Old value = 0xFF9B4F70.
"    B       loc_FF890F6C \n"

"loc_FF890DE4:\n"
"    LDR     R0, [R0, #0x10] \n"
"    BL      sub_FF9B4DA4_my \n"  // --> Patched. Old value = 0xFF9B4DA4.
"    B       loc_FF890F6C \n"

"loc_FF890DF0:\n"
"    MOV     R0, #1 \n"
"    BL      sub_FF891860 \n"
"    B       loc_FF890F6C \n"

"loc_FF890DFC:\n"
"    BL      sub_FF891140 \n"
"    B       loc_FF890E08 \n"

"loc_FF890E04:\n"
"    BL      sub_FF8914E4 \n"

"loc_FF890E08:\n"
"    STR     R6, [R5, #0x28] \n"
"    B       loc_FF890F6C \n"

"loc_FF890E10:\n"
"    BL      sub_FF8914EC \n"
"    B       loc_FF890F6C \n"

"loc_FF890E18:\n"
"    BL      sub_FF891724 \n"
"    B       loc_FF890E30 \n"

"loc_FF890E20:\n"
"    LDR     R0, [R0, #0x10] \n"
"    BL      sub_FF9B5058 \n"
"    B       loc_FF890F6C \n"

"loc_FF890E2C:\n"
"    BL      sub_FF8917B8 \n"

"loc_FF890E30:\n"
"    BL      sub_FF88E20C \n"
"    B       loc_FF890F6C \n"

"loc_FF890E38:\n"
"    LDR     R0, [R5, #0x54] \n"
"    BL      sub_FF891E24 \n"
"    B       loc_FF890F6C \n"

"loc_FF890E44:\n"
"    BL      sub_FF892278 \n"
"    B       loc_FF890F6C \n"

"loc_FF890E4C:\n"
"    LDR     R0, [R0, #0xC] \n"
"    BL      sub_FF8922E0 \n"
"    B       loc_FF890F6C \n"

"loc_FF890E58:\n"
"    BL      sub_FF8914E4 \n"
"    B       loc_FF890F6C \n"

"loc_FF890E60:\n"
"    BL      sub_FF9B38B0 \n"
"    B       loc_FF890F6C \n"

"loc_FF890E68:\n"
"    BL      sub_FF9B3AE8 \n"
"    B       loc_FF890F6C \n"

"loc_FF890E70:\n"
"    BL      sub_FF9B3BA4 \n"
"    B       loc_FF890F6C \n"

"loc_FF890E78:\n"
"    BL      sub_FF9B3C7C \n"
"    B       loc_FF890F6C \n"

"loc_FF890E80:\n"
"    MOV     R0, #0 \n"
"    BL      sub_FF9B3F34 \n"
"    B       loc_FF890F6C \n"

"loc_FF890E8C:\n"
"    BL      sub_FF9B4094 \n"
"    B       loc_FF890F6C \n"

"loc_FF890E94:\n"
"    BL      sub_FF897E68 \n"
"    B       loc_FF890F6C \n"

"loc_FF890E9C:\n"
"    BL      sub_FF897E74 \n"
"    B       loc_FF890F6C \n"

"loc_FF890EA4:\n"
"    BL      sub_FF9B4130 \n"
"    B       loc_FF890F6C \n"

"loc_FF890EAC:\n"
"    BL      sub_FF9B41DC \n"
"    B       loc_FF890F6C \n"

"loc_FF890EB4:\n"
"    BL      sub_FF8919A8 \n"
"    B       loc_FF890F6C \n"

"loc_FF890EBC:\n"
"    MOV     R0, #0 \n"
"    B       loc_FF890EC8 \n"

"loc_FF890EC4:\n"
"    LDR     R0, [R0, #0xC] \n"

"loc_FF890EC8:\n"
"    BL      sub_FF891A38 \n"
"    B       loc_FF890F6C \n"

"loc_FF890ED0:\n"
"    BL      sub_FF9B3E14 \n"
"    B       loc_FF890F6C \n"

"loc_FF890ED8:\n"
"    BL      sub_FF9B3EA4 \n"
"    B       loc_FF890F6C \n"

"loc_FF890EE0:\n"
"    BL      sub_FF8947F8 \n"
"    B       loc_FF890F6C \n"

"loc_FF890EE8:\n"
"    BL      sub_FF8948E8 \n"
"    B       loc_FF890F6C \n"

"loc_FF890EF0:\n"
"    LDR     R0, [R0, #0xC] \n"
"    BL      sub_FF9B42EC \n"
"    B       loc_FF890F6C \n"

"loc_FF890EFC:\n"
"    BL      sub_FF9B4374 \n"
"    B       loc_FF890F6C \n"

"loc_FF890F04:\n"
"    BL      sub_FF894A78 \n"
"    BL      sub_FF894940 \n"
"    B       loc_FF890F6C \n"

"loc_FF890F10:\n"
"    BL      sub_FF896B18 \n"
"    LDR     R0, =0x1F614 \n"
"    LDRH    R0, [R0, #0x9C] \n"
"    CMP     R0, #4 \n"
"    LDRNEH  R0, [R5] \n"
"    SUBNE   R1, R0, #0x8200 \n"
"    SUBNES  R1, R1, #0x36 \n"
"    BNE     loc_FF890F6C \n"
"    BL      sub_FF8948E8 \n"
"    BL      sub_FF894E10 \n"
"    B       loc_FF890F6C \n"

"loc_FF890F3C:\n"
"    MOV     R2, #0 \n"
"    MOV     R1, #0x11 \n"
"    B       loc_FF890F50 \n"

"loc_FF890F48:\n"
"    MOV     R2, #0 \n"
"    MOV     R1, #0x10 \n"

"loc_FF890F50:\n"
"    MOV     R0, #0 \n"
"    BL      sub_FF88ED6C \n"
"    B       loc_FF890F6C \n"

"loc_FF890F5C:\n"
"    LDR     R2, =0x57B \n"
"    LDR     R1, =0xFF890640 /*'SsShootTask.c'*/ \n"
"    MOV     R0, #0 \n"
"    BL      _DebugAssert \n"

"loc_FF890F6C:\n"
"    LDR     R0, [SP] \n"
"    LDR     R1, [R0, #4] \n"
"    LDR     R0, [R7] \n"
"    BL      sub_FF829BC4 /*_SetEventFlag*/ \n"
"    LDR     R4, [SP] \n"
"    LDR     R0, [R4, #8] \n"
"    CMP     R0, #0 \n"
"    LDREQ   R1, =0xFF890640 /*'SsShootTask.c'*/ \n"
"    MOVEQ   R2, #0x124 \n"
"    BLEQ    _DebugAssert \n"
"    STR     R6, [R4, #8] \n"
"    B       loc_FF890CDC \n"
);
}

/*************************************************************/
//** sub_FF9B4F70_my @ 0xFF9B4F70 - 0xFF9B5054, length=58
void __attribute__((naked,noinline)) sub_FF9B4F70_my() {
asm volatile (
"    STMFD   SP!, {R3-R7,LR} \n"
"    BL      sub_FF89024C \n"
"    MOV     R4, R0 \n"
"    MOV     R0, #0xC \n"
"    BL      sub_FF896440 \n"
"    LDR     R6, =0x7D7C \n"
"    TST     R0, #1 \n"
"    MOV     R5, #1 \n"
"    BNE     loc_FF9B5044 \n"
"    BL      sub_FF896494 \n"
"    TST     R0, #1 \n"
"    BNE     loc_FF9B5044 \n"
"    MOV     R2, #2 \n"
"    MOV     R1, SP \n"
"    ADD     R0, R5, #0x108 \n"
"    BL      _GetPropertyCase \n"
"    TST     R0, #1 \n"
"    ADDNE   R2, R5, #0x174 \n"
"    LDRNE   R1, =0xFF9B50EC /*'SsCaptureCtrl.c'*/ \n"
"    MOVNE   R0, #0 \n"
"    BLNE    _DebugAssert \n"
"    LDRSH   R0, [SP] \n"
"    BL      sub_FF82CBF8 \n"
"    BL      sub_FF82CA14 \n"
"    CMP     R0, #1 \n"
"    BLS     loc_FF9B504C \n"
"    MOV     R0, #0 \n"
"    BL      sub_FF82CB6C \n"
"    BL      sub_FF8914F4 \n"
"    BL      sub_FF892628 \n"
"    MOV     R1, R4 \n"
"    BL      sub_FF892684 \n"
"    LDR     R0, =0x10E \n"
"    MOV     R2, #4 \n"
"    ADD     R1, R4, #0x30 \n"
"    BL      _SetPropertyCase \n"
"    MOV     R2, #4 \n"
"    ADD     R1, R4, #0x34 \n"
"    MOV     R0, #0x2C \n"
"    BL      _SetPropertyCase \n"
"    MOV     R2, #4 \n"
"    ADD     R1, R4, #8 \n"
"    MOV     R0, #0x3F \n"
"    BL      _SetPropertyCase \n"
"    BL      sub_FF9B5848 \n"
"    MVN     R1, #0x1000 \n"
"    BL      sub_FF829C0C /*_ClearEventFlag*/ \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF9B4A68 \n"
"    MOV     R0, R4 \n"
"    BL      sub_FFB06E5C_my \n"  // --> Patched. Old value = 0xFFB06E5C.
"    TST     R0, #1 \n"
"    BEQ     loc_FF9B5048 \n"

"loc_FF9B5044:\n"
"    STR     R5, [R6] \n"

"loc_FF9B5048:\n"
"    LDMFD   SP!, {R3-R7,PC} \n"

"loc_FF9B504C:\n"
"    BL      sub_FF88EB04 \n"
"    BL      sub_FF8964F0 \n"
"    LDMFD   SP!, {R3-R7,PC} \n"
);
}

/*************************************************************/
//** sub_FF9B4DA4_my @ 0xFF9B4DA4 - 0xFF9B4F6C, length=115
void __attribute__((naked,noinline)) sub_FF9B4DA4_my() {
asm volatile (
"    STMFD   SP!, {R3-R7,LR} \n"
"    LDR     R7, =0x1F514 \n"
"    MOV     R5, R0 \n"
"    LDR     R0, [R7, #0x28] \n"
"    MOV     R4, #0 \n"
"    CMP     R0, #0 \n"
"    BEQ     loc_FF9B4DF0 \n"
"    LDR     R0, =0x7D7C \n"
"    MOV     R2, R5 \n"
"    LDR     R0, [R0] \n"
"    MOV     R1, #2 \n"
"    CMP     R0, #0 \n"
"    MOVNE   R4, #0x1D \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF88ED6C \n"
"    MOV     R1, R4 \n"
"    MOV     R0, R5 \n"
"    BL      sub_FF9B5DE4 \n"
"    B       loc_FF9B4F64 \n"

"loc_FF9B4DF0:\n"
"    BL      sub_FF892628 \n"
"    MOV     R1, R5 \n"
"    BL      sub_FF892684 \n"
"    LDR     R0, =0x10E \n"
"    MOV     R2, #4 \n"
"    ADD     R1, R5, #0x30 \n"
"    BL      _SetPropertyCase \n"
"    MOV     R2, #4 \n"
"    ADD     R1, R5, #0x34 \n"
"    MOV     R0, #0x2C \n"
"    BL      _SetPropertyCase \n"
"    LDR     R0, [R7, #0x104] \n"
"    LDR     R6, =0x1F614 \n"
"    CMP     R0, #0 \n"
"    BNE     loc_FF9B4E44 \n"
"    LDRH    R0, [R6, #0x9A] \n"
"    CMP     R0, #3 \n"
"    LDRNE   R0, [R5, #8] \n"
"    CMPNE   R0, #1 \n"
"    BHI     loc_FF9B4E74 \n"
"    B       loc_FF9B4E50 \n"

"loc_FF9B4E44:\n"
"    LDR     R0, [R5, #0xC] \n"
"    CMP     R0, #1 \n"
"    BNE     loc_FF9B4E74 \n"

"loc_FF9B4E50:\n"
"    MOV     R0, #0xC \n"
"    BL      sub_FF896440 \n"
"    TST     R0, #1 \n"
"    BNE     loc_FF9B4E6C \n"
"    BL      sub_FF896494 \n"
"    TST     R0, #1 \n"
"    BEQ     loc_FF9B4E74 \n"

"loc_FF9B4E6C:\n"
"    BL      sub_FF88EB04 \n"
"    MOV     R4, #1 \n"

"loc_FF9B4E74:\n"
"    TST     R4, #1 \n"
"    BNE     loc_FF9B4F48 \n"
"    LDR     R0, [R7, #0xE4] \n"
"    CMP     R0, #0 \n"
"    BEQ     loc_FF9B4EB0 \n"
"    LDRH    R0, [R6, #0x9A] \n"
"    CMP     R0, #3 \n"
"    LDRNE   R0, [R5, #8] \n"
"    CMPNE   R0, #1 \n"
"    BHI     loc_FF9B4F08 \n"
"    LDR     R0, [R7, #0x104] \n"
"    CMP     R0, #0 \n"
"    LDRNE   R0, [R5, #0xC] \n"
"    CMPNE   R0, #1 \n"
"    BHI     loc_FF9B4F08 \n"

"loc_FF9B4EB0:\n"
"    LDR     R0, =0x109 \n"
"    MOV     R2, #2 \n"
"    MOV     R1, SP \n"
"    BL      _GetPropertyCase \n"
"    TST     R0, #1 \n"
"    MOVNE   R2, #0xCA \n"
"    LDRNE   R1, =0xFF9B50EC /*'SsCaptureCtrl.c'*/ \n"
"    MOVNE   R0, #0 \n"
"    BLNE    _DebugAssert \n"
"    LDRSH   R0, [SP] \n"
"    BL      sub_FF82CBF8 \n"
"    BL      sub_FF82CA14 \n"
"    CMP     R0, #1 \n"
"    BLS     loc_FF9B4EF4 \n"
"    MOV     R0, #0 \n"
"    BL      sub_FF82CB6C \n"
"    B       loc_FF9B4F08 \n"

"loc_FF9B4EF4:\n"
"    BL      sub_FF88EB04 \n"
"    BL      sub_FF8964F0 \n"
"    MOV     R4, #1 \n"
"    CMP     R4, #0 \n"
"    BNE     loc_FF9B4F48 \n"

"loc_FF9B4F08:\n"
"    BL      sub_FF9B5848 \n"
"    MVN     R1, #0x1000 \n"
"    BL      sub_FF829C0C /*_ClearEventFlag*/ \n"
"    MOV     R0, R5 \n"
"    BL      sub_FFB06AB4 \n"
"    TST     R0, #1 \n"
"    MOV     R4, R0 \n"
"    BNE     loc_FF9B4F48 \n"
"    BL      sub_FF8914F4 \n"
"    MOV     R0, R5 \n"
"    BL      sub_FF9B4A68 \n"
"    MOV     R0, #2 \n"
"    BL      sub_FF897FF4 \n"
"    MOV     R0, R5 \n"
"    BL      sub_FFB06E5C_my \n"  // --> Patched. Old value = 0xFFB06E5C.
"    B       loc_FF9B4F64 \n"

"loc_FF9B4F48:\n"
"    MOV     R2, R5 \n"
"    MOV     R1, #2 \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF88ED6C \n"
"    MOV     R1, R4 \n"
"    MOV     R0, R5 \n"
"    BL      sub_FF9B5FF8 \n"

"loc_FF9B4F64:\n"
"    MOV     R0, #0 \n"
"    STR     R0, [R7, #0x28] \n"
"    LDMFD   SP!, {R3-R7,PC} \n"
);
}

/*************************************************************/
//** sub_FFB06E5C_my @ 0xFFB06E5C - 0xFFB070E0, length=162
void __attribute__((naked,noinline)) sub_FFB06E5C_my() {
asm volatile (
"    STMFD   SP!, {R2-R10,LR} \n"
"    MOV     R4, R0 \n"
"    BL      sub_FF9B49D0 \n"
"    BL      sub_FF9B56A4 \n"
"    LDR     R7, =0x1F514 \n"
"    LDR     R0, [R7, #0x190] \n"
"    CMP     R0, #2 \n"
"    BNE     loc_FFB06E98 \n"
"    ADD     R0, R7, #0x100 \n"
"    LDRH    R0, [R0, #0x9A] \n"
"    CMP     R0, #3 \n"
"    LDRNE   R0, [R4, #8] \n"
"    CMPNE   R0, #1 \n"
"    MOVLS   R0, #4 \n"
"    BLLS    sub_FF9137C4 \n"

"loc_FFB06E98:\n"
"    LDR     R0, =0x12F \n"
"    MOV     R2, #4 \n"
"    ADD     R1, SP, #4 \n"
"    BL      _GetPropertyCase \n"
"    TST     R0, #1 \n"
"    LDRNE   R2, =0x191 \n"
"    LDRNE   R1, =0xFFB07108 /*'SsStandardCaptureSeq.c'*/ \n"
"    MOVNE   R0, #0 \n"
"    BLNE    _DebugAssert \n"
"    LDR     R0, [SP, #4] \n"
"    AND     R0, R0, #0xFF00 \n"
"    CMP     R0, #0x600 \n"
"    LDRNE   R0, =0xFF9B4664 \n"
"    LDREQ   R0, =0xFFB06DD8 \n"
"    MOVNE   R1, R4 \n"
"    MOVEQ   R1, #0 \n"
"    BL      sub_FF8D2A64 \n"
"    LDR     R0, [R7, #0x184] \n"
"    CMP     R0, #0 \n"
"    BLNE    sub_FF9B48BC \n"
"    MOV     R0, #0 \n"
//"  BL      _sub_FF8938AC \n"  // --> Nullsub call removed.
"    MOV     R0, R4 \n"
"    BL      sub_FF9B4AF8_my \n"  // --> Patched. Old value = 0xFF9B4AF8.
"    LDR     R1, =0xDC20 \n"
"    MOV     R2, #4 \n"
"    MOV     R0, #0x8A \n"
"    BL      _GetPropertyCase \n"
"    TST     R0, #1 \n"
"    LDRNE   R2, =0x1A5 \n"
"    LDRNE   R1, =0xFFB07108 /*'SsStandardCaptureSeq.c'*/ \n"
"    MOVNE   R0, #0 \n"
"    BLNE    _DebugAssert \n"
"    BL      sub_FF893A68 \n"
"    BL      sub_FF87EE74 \n"
"    MOV     R1, #0 \n"
"    MOV     R0, R4 \n"
"    BL      sub_FFB06DDC \n"
"    MOV     R6, R0 \n"
"    LDR     R0, [SP, #4] \n"
"    LDR     R8, =0xDC20 \n"
"    AND     R0, R0, #0xFF00 \n"
"    CMP     R0, #0x600 \n"
"    LDRH    R0, [R4, #0x18] \n"
"    LDRNE   R2, =0xFF9B48A4 \n"
"    LDREQ   R2, =0xFF9B4810 \n"
"    CMP     R0, #0 \n"
"    BEQ     loc_FFB06F68 \n"
"    CMP     R0, #1 \n"
"    BEQ     loc_FFB06F8C \n"
"    CMP     R0, #4 \n"
"    BNE     loc_FFB07000 \n"

"loc_FFB06F68:\n"
"    STR     R6, [SP] \n"
"    MOV     R3, R2 \n"
"    LDR     R1, [R4, #0x34] \n"
"    LDR     R2, [R8] \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF9B4478 \n"
"    MOV     R5, R0 \n"
"    BL      sub_FF8CF948 \n"
"    B       loc_FFB07014 \n"

"loc_FFB06F8C:\n"
"    STR     R6, [SP] \n"
"    MOV     R3, R2 \n"
"    LDR     R1, [R4, #0x34] \n"
"    LDR     R2, [R8] \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF9B4504 \n"
"    MOV     R5, R0 \n"
"    MOV     R1, #0 \n"
"    MOV     R0, #0x45 \n"
"    BL      sub_FF834324 \n"
"    TST     R5, #1 \n"
"    BNE     loc_FFB07014 \n"
"    LDR     R0, =0xFFB06DD8 \n"
"    MOV     R1, #0 \n"
"    BL      sub_FF8D2A64 \n"
"    MOV     R1, #1 \n"
"    MOV     R0, R4 \n"
"    BL      sub_FFB06DDC \n"
"    MOV     R6, R0 \n"
"    LDR     R0, [R8] \n"
"    BL      sub_FF9B4954 \n"
"    LDR     R1, [R4, #0x34] \n"
"    LDR     R2, [R8] \n"
"    MOV     R3, R6 \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF9B459C \n"
"    MOV     R5, R0 \n"
"    BL      sub_FF9B4400 \n"
"    B       loc_FFB07014 \n"

"loc_FFB07000:\n"
"    MOV     R2, #0x1D8 \n"
"    LDR     R1, =0xFFB07108 /*'SsStandardCaptureSeq.c'*/ \n"
"    MOV     R0, #0 \n"
"    BL      _DebugAssert \n"
"    MOV     R5, #0x1D \n"

"loc_FFB07014:\n"
"    BL      sub_FF87EE8C \n"
"    LDR     R8, =0xFFB06DD8 \n"
"    TST     R5, #1 \n"
"    BNE     loc_FFB07094 \n"
"    LDR     R0, [R7, #0x100] \n"
"    CMP     R0, #0 \n"
"    BNE     loc_FFB0704C \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF9B65E8 \n"
"    TST     R0, #1 \n"
"    LDRNE   R2, =0x1FD \n"
"    LDRNE   R1, =0xFFB07108 /*'SsStandardCaptureSeq.c'*/ \n"
"    MOVNE   R0, #0 \n"
"    BLNE    _DebugAssert \n"
"    BL      capt_seq_hook_raw_here \n"         // added

"loc_FFB0704C:\n"
"    MOV     R0, R4 \n"
"    BL      sub_FF9B4BE0 \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF9B4B98 \n"
"    CMP     R6, R8 \n"
"    BEQ     loc_FFB07094 \n"
"    BL      sub_FF9B5848 \n"
"    LDR     R9, =0x20A \n"
"    LDR     R2, =0x3A98 \n"
"    LDR     R3, =0xFFB07108 /*'SsStandardCaptureSeq.c'*/ \n"
"    MOV     R1, #4 \n"
"    STR     R9, [SP] \n"
"    BL      sub_FF89671C \n"
"    CMP     R0, #0 \n"
"    MOVNE   R2, R9 \n"
"    LDRNE   R1, =0xFFB07108 /*'SsStandardCaptureSeq.c'*/ \n"
"    MOVNE   R0, #0 \n"
"    BLNE    _DebugAssert \n"

"loc_FFB07094:\n"
"    MOV     R2, R4 \n"
"    MOV     R1, #2 \n"
"    MOV     R0, R5 \n"
"    BL      sub_FF88ED6C \n"
"    LDR     R0, [R7, #0x28] \n"
"    CMP     R0, #0 \n"
"    BEQ     loc_FFB070C8 \n"
"    SUBS    R1, R6, R8 \n"
"    MOVNE   R1, #1 \n"
"    MOV     R2, R5 \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF9B5D7C \n"
"    B       loc_FFB070DC \n"

"loc_FFB070C8:\n"
"    SUBS    R1, R6, R8 \n"
"    MOVNE   R1, #1 \n"
"    MOV     R2, R5 \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF9B5D10 \n"

"loc_FFB070DC:\n"
"    MOV     R0, R5 \n"
"    LDMFD   SP!, {R2-R10,PC} \n"
"    .ltorg\n"         // added
);
}

/*************************************************************/
//** sub_FF9B4AF8_my @ 0xFF9B4AF8 - 0xFF9B4B38, length=17
void __attribute__((naked,noinline)) sub_FF9B4AF8_my() {
asm volatile (
"    STMFD   SP!, {R4-R6,LR} \n"
"    MOV     R4, R0 \n"
"    LDR     R0, =0x1F514 \n"
"    LDR     R5, =0x7D74 \n"
"    LDR     R0, [R0, #0xF4] \n"
"    CMP     R0, #0 \n"
"    LDRNE   R0, =0x1F614 \n"
"    LDRNEH  R0, [R0, #0x9A] \n"
"    CMPNE   R0, #3 \n"
"    LDRNE   R0, [R4, #8] \n"
"    CMPNE   R0, #1 \n"
"    BHI     loc_FF9B4B38 \n"
"    LDR     R1, [R4, #0x1C] \n"
"    MOV     R0, #0 \n"
"    BL      sub_FFA4F79C \n"
"    STR     R0, [R5] \n"

"loc_FF9B4B38:\n"
"    BL      wait_until_remote_button_is_released\n" // added
"    BL      capt_seq_hook_set_nr\n"                 // added
"    LDR     R0, [R4, #0x20] \n"
"    LDR     PC, =0xFF9B4B3C \n"  // Continue in firmware
);
}

/*************************************************************/
//** exp_drv_task @ 0xFF8E0DE0 - 0xFF8E147C, length=424
void __attribute__((naked,noinline)) exp_drv_task() {
asm volatile (
"    STMFD   SP!, {R4-R9,LR} \n"
"    SUB     SP, SP, #0x2C \n"
"    LDR     R6, =0x4514 \n"
"    LDR     R7, =0xBB8 \n"
"    LDR     R4, =0x5BC8C \n"
"    MOV     R0, #0 \n"
"    ADD     R5, SP, #0x1C \n"
"    STR     R0, [SP, #0xC] \n"

"loc_FF8E0E00:\n"
"    LDR     R0, [R6, #0x20] \n"
"    MOV     R2, #0 \n"
"    ADD     R1, SP, #0x28 \n"
"    BL      sub_FF829E30 /*_ReceiveMessageQueue*/ \n"
"    LDR     R0, [SP, #0xC] \n"
"    CMP     R0, #1 \n"
"    BNE     loc_FF8E0E50 \n"
"    LDR     R0, [SP, #0x28] \n"
"    LDR     R0, [R0] \n"
"    CMP     R0, #0x14 \n"
"    CMPNE   R0, #0x15 \n"
"    CMPNE   R0, #0x16 \n"
"    CMPNE   R0, #0x17 \n"
"    CMPNE   R0, #0x18 \n"
"    BEQ     loc_FF8E0FB4 \n"
"    CMP     R0, #0x2E \n"
"    BEQ     loc_FF8E0F38 \n"
"    ADD     R1, SP, #0xC \n"
"    MOV     R0, #0 \n"
"    BL      sub_FF8E0D80 \n"

"loc_FF8E0E50:\n"
"    LDR     R0, [SP, #0x28] \n"
"    LDR     R1, [R0] \n"
"    CMP     R1, #0x34 \n"
"    BNE     loc_FF8E0E7C \n"
"    BL      sub_FF8E261C \n"
"    LDR     R0, [R6, #0x1C] \n"
"    MOV     R1, #1 \n"
"    BL      sub_FF829BC4 /*_SetEventFlag*/ \n"
"    BL      _ExitTask \n"
"    ADD     SP, SP, #0x2C \n"
"    LDMFD   SP!, {R4-R9,PC} \n"

"loc_FF8E0E7C:\n"
"    CMP     R1, #0x33 \n"
"    BNE     loc_FF8E0E98 \n"
"    LDR     R2, [R0, #0xAC]! \n"
"    LDR     R1, [R0, #4] \n"
"    MOV     R0, R1 \n"
"    BLX     R2 \n"
"    B       loc_FF8E1474 \n"

"loc_FF8E0E98:\n"
"    CMP     R1, #0x2C \n"
"    BNE     loc_FF8E0EE8 \n"
"    LDR     R0, [R6, #0x1C] \n"
"    MOV     R1, #0x80 \n"
"    BL      sub_FF829C0C /*_ClearEventFlag*/ \n"
"    LDR     R0, =0xFF8DC148 \n"
"    MOV     R1, #0x80 \n"
"    BL      sub_FF887400 \n"
"    LDR     R0, [R6, #0x1C] \n"
"    MOV     R2, R7 \n"
"    MOV     R1, #0x80 \n"
"    BL      sub_FF829AF8 /*_WaitForAllEventFlag*/ \n"
"    TST     R0, #1 \n"
"    LDRNE   R2, =0x1798 \n"
"    BNE     loc_FF8E0FA4 \n"

"loc_FF8E0ED4:\n"
"    LDR     R1, [SP, #0x28] \n"
"    LDR     R0, [R1, #0xB0] \n"
"    LDR     R1, [R1, #0xAC] \n"
"    BLX     R1 \n"
"    B       loc_FF8E1474 \n"

"loc_FF8E0EE8:\n"
"    CMP     R1, #0x2D \n"
"    BNE     loc_FF8E0F30 \n"
"    ADD     R1, SP, #0xC \n"
"    BL      sub_FF8E0D80 \n"
"    LDR     R0, [R6, #0x1C] \n"
"    MOV     R1, #0x100 \n"
"    BL      sub_FF829C0C /*_ClearEventFlag*/ \n"
"    LDR     R0, =0xFF8DC158 \n"
"    MOV     R1, #0x100 \n"
"    BL      sub_FF888210 \n"
"    LDR     R0, [R6, #0x1C] \n"
"    MOV     R2, R7 \n"
"    MOV     R1, #0x100 \n"
"    BL      sub_FF829AF8 /*_WaitForAllEventFlag*/ \n"
"    TST     R0, #1 \n"
"    BEQ     loc_FF8E0ED4 \n"
"    LDR     R2, =0x17A2 \n"
"    B       loc_FF8E0FA4 \n"

"loc_FF8E0F30:\n"
"    CMP     R1, #0x2E \n"
"    BNE     loc_FF8E0F48 \n"

"loc_FF8E0F38:\n"
"    LDR     R0, [SP, #0x28] \n"
"    ADD     R1, SP, #0xC \n"
"    BL      sub_FF8E0D80 \n"
"    B       loc_FF8E0ED4 \n"

"loc_FF8E0F48:\n"
"    CMP     R1, #0x31 \n"
"    BNE     loc_FF8E0F60 \n"
"    BL      sub_FF8CA544 \n"
"    BL      sub_FF8CB3BC \n"
"    BL      sub_FF8CAE9C \n"
"    B       loc_FF8E0ED4 \n"

"loc_FF8E0F60:\n"
"    CMP     R1, #0x32 \n"
"    BNE     loc_FF8E0FB4 \n"
"    LDR     R0, [R6, #0x1C] \n"
"    MOV     R1, #4 \n"
"    BL      sub_FF829C0C /*_ClearEventFlag*/ \n"
"    LDR     R1, =0xFF8DC178 \n"
"    LDR     R0, =0xFFFFF400 \n"
"    MOV     R2, #4 \n"
"    BL      sub_FF8E2680 \n"
"    BL      sub_FF8CA1D8 \n"
"    LDR     R0, [R6, #0x1C] \n"
"    MOV     R2, R7 \n"
"    MOV     R1, #4 \n"
"    BL      sub_FF829A0C /*_WaitForAnyEventFlag*/ \n"
"    TST     R0, #1 \n"
"    BEQ     loc_FF8E0ED4 \n"
"    LDR     R2, =0x17DA \n"

"loc_FF8E0FA4:\n"
"    LDR     R1, =0xFF8DC920 /*'ExpDrv.c'*/ \n"
"    MOV     R0, #0 \n"
"    BL      _DebugAssert \n"
"    B       loc_FF8E0ED4 \n"

"loc_FF8E0FB4:\n"
"    LDR     R0, [SP, #0x28] \n"
"    MOV     R8, #1 \n"
"    LDR     R1, [R0] \n"
"    CMP     R1, #0x12 \n"
"    CMPNE   R1, #0x13 \n"
"    BNE     loc_FF8E101C \n"
"    LDR     R1, [R0, #0x94] \n"
"    ADD     R1, R1, R1, LSL#1 \n"
"    ADD     R1, R0, R1, LSL#2 \n"
"    SUB     R1, R1, #8 \n"
"    LDMIA   R1, {R2,R3,R9} \n"
"    STMIA   R5, {R2,R3,R9} \n"
"    BL      sub_FF8DF1B8 \n"
"    LDR     R0, [SP, #0x28] \n"
"    LDR     R1, [R0, #0x94] \n"
"    LDR     R3, [R0, #0xAC] \n"
"    LDR     R2, [R0, #0xB0] \n"
"    ADD     R0, R0, #4 \n"
"    BLX     R3 \n"
"    LDR     R0, [SP, #0x28] \n"
"    BL      sub_FF8E296C \n"
"    LDR     R0, [SP, #0x28] \n"
"    LDR     R1, [R0, #0x94] \n"
"    LDR     R2, [R0, #0xB8] \n"
"    LDR     R3, [R0, #0xB4] \n"
"    B       loc_FF8E1338 \n"

"loc_FF8E101C:\n"
"    CMP     R1, #0x14 \n"
"    CMPNE   R1, #0x15 \n"
"    CMPNE   R1, #0x16 \n"
"    CMPNE   R1, #0x17 \n"
"    CMPNE   R1, #0x18 \n"
"    BNE     loc_FF8E10C8 \n"
"    ADD     R3, SP, #0xC \n"
"    MOV     R2, SP \n"
"    ADD     R1, SP, #0x1C \n"
"    BL      sub_FF8DF428 \n"
"    CMP     R0, #1 \n"
"    MOV     R9, R0 \n"
"    CMPNE   R9, #5 \n"
"    BNE     loc_FF8E1074 \n"
"    LDR     R0, [SP, #0x28] \n"
"    MOV     R2, R9 \n"
"    LDR     R1, [R0, #0x94] \n"
"    LDR     R12, [R0, #0xAC] \n"
"    LDR     R3, [R0, #0xB0] \n"
"    ADD     R0, R0, #4 \n"
"    BLX     R12 \n"
"    B       loc_FF8E10AC \n"

"loc_FF8E1074:\n"
"    LDR     R0, [SP, #0x28] \n"
"    CMP     R9, #2 \n"
"    LDR     R1, [R0, #0x94] \n"
"    LDR     R12, [R0, #0xAC] \n"
"    LDR     R3, [R0, #0xB0] \n"
"    CMPNE   R9, #6 \n"
"    MOV     R2, R9 \n"
"    ADD     R0, R0, #4 \n"
"    BNE     loc_FF8E10C0 \n"
"    BLX     R12 \n"
"    LDR     R0, [SP, #0x28] \n"
"    MOV     R2, SP \n"
"    ADD     R1, SP, #0x1C \n"
"    BL      sub_FF8E0A8C \n"

"loc_FF8E10AC:\n"
"    LDR     R0, [SP, #0x28] \n"
"    LDR     R2, [SP, #0xC] \n"
"    MOV     R1, R9 \n"
"    BL      sub_FF8E0D10 \n"
"    B       loc_FF8E1340 \n"

"loc_FF8E10C0:\n"
"    BLX     R12 \n"
"    B       loc_FF8E1340 \n"

"loc_FF8E10C8:\n"
"    CMP     R1, #0x28 \n"
"    CMPNE   R1, #0x29 \n"
"    BNE     loc_FF8E1114 \n"
"    LDR     R1, [R0, #0x94] \n"
"    ADD     R1, R1, R1, LSL#1 \n"
"    ADD     R1, R0, R1, LSL#2 \n"
"    SUB     R1, R1, #8 \n"
"    LDMIA   R1, {R2,R3,R9} \n"
"    STMIA   R5, {R2,R3,R9} \n"
"    BL      sub_FF8DE0E4 \n"
"    LDR     R0, [SP, #0x28] \n"
"    LDR     R1, [R0, #0x94] \n"
"    LDR     R3, [R0, #0xAC] \n"
"    LDR     R2, [R0, #0xB0] \n"
"    ADD     R0, R0, #4 \n"
"    BLX     R3 \n"
"    LDR     R0, [SP, #0x28] \n"
"    BL      sub_FF8DE540 \n"
"    B       loc_FF8E1340 \n"

"loc_FF8E1114:\n"
"    ADD     R1, R0, #4 \n"
"    LDMIA   R1, {R2,R3,R9} \n"
"    STMIA   R5, {R2,R3,R9} \n"
"    LDR     R1, [R0] \n"
"    CMP     R1, #0x2C \n"
"    ADDCC   PC, PC, R1, LSL#2 \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E11E0 \n"
"    B       loc_FF8E11E0 \n"
"    B       loc_FF8E11E8 \n"
"    B       loc_FF8E11F0 \n"
"    B       loc_FF8E11F0 \n"
"    B       loc_FF8E11F0 \n"
"    B       loc_FF8E11E0 \n"
"    B       loc_FF8E11E8 \n"
"    B       loc_FF8E11F0 \n"
"    B       loc_FF8E11F0 \n"
"    B       loc_FF8E1208 \n"
"    B       loc_FF8E1208 \n"
"    B       loc_FF8E1314 \n"
"    B       loc_FF8E131C \n"
"    B       loc_FF8E131C \n"
"    B       loc_FF8E131C \n"
"    B       loc_FF8E131C \n"
"    B       loc_FF8E1324 \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E11F8 \n"
"    B       loc_FF8E1200 \n"
"    B       loc_FF8E1200 \n"
"    B       loc_FF8E1200 \n"
"    B       loc_FF8E1214 \n"
"    B       loc_FF8E1214 \n"
"    B       loc_FF8E1214 \n"
"    B       loc_FF8E121C \n"
"    B       loc_FF8E1254 \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E128C \n"
"    B       loc_FF8E12C4 \n"
"    B       loc_FF8E12FC \n"
"    B       loc_FF8E12FC \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E1328 \n"
"    B       loc_FF8E1304 \n"
"    B       loc_FF8E130C \n"

"loc_FF8E11E0:\n"
"    BL      sub_FF8DC7A0 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E11E8:\n"
"    BL      sub_FF8DCA6C \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E11F0:\n"
"    BL      sub_FF8DCCAC \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E11F8:\n"
"    BL      sub_FF8DCFF4 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E1200:\n"
"    BL      sub_FF8DD224 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E1208:\n"
"    BL      sub_FF8DD614_my \n"  // --> Patched. Old value = 0xFF8DD614.
"    MOV     R8, #0 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E1214:\n"
"    BL      sub_FF8DD7E8 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E121C:\n"
"    LDRH    R1, [R0, #4] \n"
"    STRH    R1, [SP, #0x1C] \n"
"    LDRH    R1, [R4, #2] \n"
"    STRH    R1, [SP, #0x1E] \n"
"    LDRH    R1, [R4, #4] \n"
"    STRH    R1, [SP, #0x20] \n"
"    LDRH    R1, [R4, #6] \n"
"    STRH    R1, [SP, #0x22] \n"
"    LDRH    R1, [R0, #0xC] \n"
"    STRH    R1, [SP, #0x24] \n"
"    LDRH    R1, [R4, #0xA] \n"
"    STRH    R1, [SP, #0x26] \n"
"    BL      sub_FF8DDD50 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E1254:\n"
"    LDRH    R1, [R0, #4] \n"
"    STRH    R1, [SP, #0x1C] \n"
"    LDRH    R1, [R4, #2] \n"
"    STRH    R1, [SP, #0x1E] \n"
"    LDRH    R1, [R4, #4] \n"
"    STRH    R1, [SP, #0x20] \n"
"    LDRH    R1, [R4, #6] \n"
"    STRH    R1, [SP, #0x22] \n"
"    LDRH    R1, [R4, #8] \n"
"    STRH    R1, [SP, #0x24] \n"
"    LDRH    R1, [R4, #0xA] \n"
"    STRH    R1, [SP, #0x26] \n"
"    BL      sub_FF8E26E0 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E128C:\n"
"    LDRH    R1, [R4] \n"
"    STRH    R1, [SP, #0x1C] \n"
"    LDRH    R1, [R0, #6] \n"
"    STRH    R1, [SP, #0x1E] \n"
"    LDRH    R1, [R4, #4] \n"
"    STRH    R1, [SP, #0x20] \n"
"    LDRH    R1, [R4, #6] \n"
"    STRH    R1, [SP, #0x22] \n"
"    LDRH    R1, [R4, #8] \n"
"    STRH    R1, [SP, #0x24] \n"
"    LDRH    R1, [R4, #0xA] \n"
"    STRH    R1, [SP, #0x26] \n"
"    BL      sub_FF8E2818 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E12C4:\n"
"    LDRH    R1, [R4] \n"
"    STRH    R1, [SP, #0x1C] \n"
"    LDRH    R1, [R4, #2] \n"
"    STRH    R1, [SP, #0x1E] \n"
"    LDRH    R1, [R4, #4] \n"
"    STRH    R1, [SP, #0x20] \n"
"    LDRH    R1, [R4, #6] \n"
"    STRH    R1, [SP, #0x22] \n"
"    LDRH    R1, [R0, #0xC] \n"
"    STRH    R1, [SP, #0x24] \n"
"    LDRH    R1, [R4, #0xA] \n"
"    STRH    R1, [SP, #0x26] \n"
"    BL      sub_FF8E28C4 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E12FC:\n"
"    BL      sub_FF8DDE64 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E1304:\n"
"    BL      sub_FF8DE658 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E130C:\n"
"    BL      sub_FF8DE9E4 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E1314:\n"
"    BL      sub_FF8DEC54 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E131C:\n"
"    BL      sub_FF8DEE20 \n"
"    B       loc_FF8E1328 \n"

"loc_FF8E1324:\n"
"    BL      sub_FF8DEF98 \n"

"loc_FF8E1328:\n"
"    LDR     R0, [SP, #0x28] \n"
"    LDR     R1, [R0, #0x94] \n"
"    LDR     R2, [R0, #0xB0] \n"
"    LDR     R3, [R0, #0xAC] \n"

"loc_FF8E1338:\n"
"    ADD     R0, R0, #4 \n"
"    BLX     R3 \n"

"loc_FF8E1340:\n"
"    LDR     R0, [SP, #0x28] \n"
"    LDR     R0, [R0] \n"
"    CMP     R0, #0x10 \n"
"    BEQ     loc_FF8E1378 \n"
"    BGT     loc_FF8E1368 \n"
"    CMP     R0, #1 \n"
"    CMPNE   R0, #4 \n"
"    CMPNE   R0, #0xE \n"
"    BNE     loc_FF8E13AC \n"
"    B       loc_FF8E1378 \n"

"loc_FF8E1368:\n"
"    CMP     R0, #0x13 \n"
"    CMPNE   R0, #0x18 \n"
"    CMPNE   R0, #0x1B \n"
"    BNE     loc_FF8E13AC \n"

"loc_FF8E1378:\n"
"    LDRSH   R0, [R4] \n"
"    CMN     R0, #0xC00 \n"
"    LDRNESH R1, [R4, #8] \n"
"    CMNNE   R1, #0xC00 \n"
"    STRNEH  R0, [SP, #0x1C] \n"
"    STRNEH  R1, [SP, #0x24] \n"
"    BNE     loc_FF8E13AC \n"
"    ADD     R0, SP, #0x10 \n"
"    BL      sub_FF8E2B88 \n"
"    LDRH    R0, [SP, #0x10] \n"
"    STRH    R0, [SP, #0x1C] \n"
"    LDRH    R0, [SP, #0x18] \n"
"    STRH    R0, [SP, #0x24] \n"

"loc_FF8E13AC:\n"
"    LDR     R0, [SP, #0x28] \n"
"    CMP     R8, #1 \n"
"    BNE     loc_FF8E13FC \n"
"    LDR     R1, [R0, #0x94] \n"
"    MOV     R2, #0xC \n"
"    ADD     R1, R1, R1, LSL#1 \n"
"    ADD     R0, R0, R1, LSL#2 \n"
"    SUB     R8, R0, #8 \n"
"    LDR     R0, =0x5BC8C \n"
"    ADD     R1, SP, #0x1C \n"
"    BL      sub_006BA828 \n"
"    LDR     R0, =0x5BC98 \n"
"    MOV     R2, #0xC \n"
"    ADD     R1, SP, #0x1C \n"
"    BL      sub_006BA828 \n"
"    LDR     R0, =0x5BCA4 \n"
"    MOV     R2, #0xC \n"
"    MOV     R1, R8 \n"
"    BL      sub_006BA828 \n"
"    B       loc_FF8E1474 \n"

"loc_FF8E13FC:\n"
"    LDR     R0, [R0] \n"
"    MOV     R3, #1 \n"
"    CMP     R0, #0xB \n"
"    BNE     loc_FF8E1440 \n"
"    MOV     R2, #0 \n"
"    STRD    R2, [SP] \n"
"    MOV     R2, R3 \n"
"    MOV     R1, R3 \n"
"    MOV     R0, #0 \n"
"    BL      sub_FF8DC560 \n"
"    MOV     R3, #1 \n"
"    MOV     R2, #0 \n"
"    STRD    R2, [SP] \n"
"    MOV     R2, R3 \n"
"    MOV     R1, R3 \n"
"    MOV     R0, #0 \n"
"    B       loc_FF8E1470 \n"

"loc_FF8E1440:\n"
"    MOV     R2, #1 \n"
"    STRD    R2, [SP] \n"
"    MOV     R3, R2 \n"
"    MOV     R1, R2 \n"
"    MOV     R0, R2 \n"
"    BL      sub_FF8DC560 \n"
"    MOV     R3, #1 \n"
"    MOV     R2, R3 \n"
"    MOV     R1, R3 \n"
"    MOV     R0, R3 \n"
"    STR     R3, [SP] \n"
"    STR     R3, [SP, #4] \n"

"loc_FF8E1470:\n"
"    BL      sub_FF8DC6E8 \n"

"loc_FF8E1474:\n"
"    LDR     R0, [SP, #0x28] \n"
"    BL      sub_FF8E261C \n"
"    B       loc_FF8E0E00 \n"
);
}

/*************************************************************/
//** sub_FF8DD614_my @ 0xFF8DD614 - 0xFF8DD6D8, length=50
void __attribute__((naked,noinline)) sub_FF8DD614_my() {
asm volatile (
"    STMFD   SP!, {R4-R8,LR} \n"
"    LDR     R7, =0x4514 \n"
"    MOV     R4, R0 \n"
"    LDR     R0, [R7, #0x1C] \n"
"    MOV     R1, #0x3E \n"
"    BL      sub_FF829C0C /*_ClearEventFlag*/ \n"
"    MOV     R2, #0 \n"
"    LDRSH   R0, [R4, #4] \n"
"    MOV     R3, #1 \n"
"    MOV     R1, R2 \n"
"    BL      sub_FF8DC1E0 \n"
"    MOV     R6, R0 \n"
"    LDRSH   R0, [R4, #6] \n"
"    BL      sub_FF8DC3C0 \n"
"    LDRSH   R0, [R4, #8] \n"
"    BL      sub_FF8DC418 \n"
"    LDRSH   R0, [R4, #0xA] \n"
"    BL      sub_FF8DC470 \n"
"    LDRSH   R0, [R4, #0xC] \n"
"    MOV     R1, #0 \n"
"    BL      sub_FF8DC4C8 \n"
"    MOV     R5, R0 \n"
"    LDR     R0, [R4] \n"
"    LDR     R8, =0x5BCA4 \n"
"    CMP     R0, #0xB \n"
"    MOVEQ   R6, #0 \n"
"    MOVEQ   R5, R6 \n"
"    BEQ     loc_FF8DD6AC \n"
"    CMP     R6, #1 \n"
"    BNE     loc_FF8DD6AC \n"
"    LDRSH   R0, [R4, #4] \n"
"    LDR     R1, =0xFF8DC138 \n"
"    MOV     R2, #2 \n"
"    BL      sub_FF887764 \n"
"    STRH    R0, [R4, #4] \n"
"    MOV     R0, #0 \n"
"    STR     R0, [R7, #0x28] \n"
"    B       loc_FF8DD6B4 \n"

"loc_FF8DD6AC:\n"
"    LDRH    R0, [R8] \n"
"    STRH    R0, [R4, #4] \n"

"loc_FF8DD6B4:\n"
"    CMP     R5, #1 \n"
"    LDRNEH  R0, [R8, #8] \n"
"    BNE     loc_FF8DD6D0 \n"
"    LDRSH   R0, [R4, #0xC] \n"
"    LDR     R1, =0xFF8DC1BC \n"
"    MOV     R2, #0x20 \n"
"    BL      sub_FF8E269C \n"

"loc_FF8DD6D0:\n"
"    STRH    R0, [R4, #0xC] \n"
"    LDRSH   R0, [R4, #6] \n"
"    BL      sub_FF8CA250_my \n"  // --> Patched. Old value = 0xFF8CA250.
"    LDR     PC, =0xFF8DD6DC \n"  // Continue in firmware
);
}

/*************************************************************/
//** sub_FF8CA250_my @ 0xFF8CA250 - 0xFF8CA298, length=19
void __attribute__((naked,noinline)) sub_FF8CA250_my() {
asm volatile (
"    STMFD   SP!, {R4-R6,LR} \n"
"    LDR     R5, =0x4134 \n"
"    MOV     R4, R0 \n"
"    LDR     R0, [R5, #4] \n"
"    CMP     R0, #1 \n"
"    LDRNE   R2, =0x16B \n"
"    LDRNE   R1, =0xFF8C9FF4 /*'Shutter.c'*/ \n"
"    MOVNE   R0, #0 \n"
"    BLNE    _DebugAssert \n"
"    CMN     R4, #0xC00 \n"
"    LDREQSH R4, [R5, #2] \n"
"    CMN     R4, #0xC00 \n"
"    LDREQ   R2, =0x171 \n"
"    LDREQ   R1, =0xFF8C9FF4 /*'Shutter.c'*/ \n"
"    MOVEQ   R0, #0 \n"
"    STRH    R4, [R5, #2] \n"
"    BLEQ    _DebugAssert \n"
"    MOV     R0, R4 \n"
"    BL      apex2us \n"  // --> Patched. Old value = _apex2us.
"    LDR     PC, =0xFF8CA29C \n"  // Continue in firmware
);
}
