/*
 * movie_rec.c - auto-generated by CHDK code_gen.
 */
#include "conf.h"

void change_video_tables(int a, int b){
}

void  set_quality(int *x){ // -17 highest; +12 lowest
 if (conf.video_mode) *x=12-((conf.video_quality-1)*(12+17)/(99-1));
}

/*************************************************************/
//** movie_record_task @ 0xFF1EA0F0 - 0xFF1EA268, length=95
void __attribute__((naked,noinline)) movie_record_task() {
asm volatile (
"    STMFD   SP!, {R2-R10,LR} \n"
"    LDR     R6, =0xFF1E8D78 \n"
"    LDR     R7, =0xFF1E9AE4 \n"
"    LDR     R4, =0xAC60 \n"
"    LDR     R9, =0x69B \n"
"    LDR     R10, =0x2710 \n"
"    MOV     R8, #1 \n"
"    MOV     R5, #0 \n"

"loc_FF1EA110:\n"
"    LDR     R0, [R4, #0x24] \n"
"    MOV     R2, #0 \n"
"    ADD     R1, SP, #4 \n"
"    BL      sub_0068EF6C /*_ReceiveMessageQueue*/ \n"
"    LDR     R0, [R4, #0x2C] \n"
"    CMP     R0, #0 \n"
"    LDRNE   R0, [R4, #0xC] \n"
"    CMPNE   R0, #2 \n"
"    LDRNE   R0, [R4, #0x44] \n"
"    CMPNE   R0, #6 \n"
"    BNE     loc_FF1EA24C \n"
"    LDR     R0, [SP, #4] \n"
"    LDR     R1, [R0] \n"
"    SUB     R1, R1, #2 \n"
"    CMP     R1, #0xD \n"
"    ADDCC   PC, PC, R1, LSL#2 \n"
"    B       loc_FF1EA24C \n"
"    B       loc_FF1EA1EC \n"
"    B       loc_FF1EA210 \n"
"    B       loc_FF1EA220 \n"
"    B       loc_FF1EA228 \n"
"    B       loc_FF1EA230 \n"
"    B       loc_FF1EA238 \n"
"    B       loc_FF1EA1F4 \n"
"    B       loc_FF1EA240 \n"
"    B       loc_FF1EA200 \n"
"    B       loc_FF1EA24C \n"
"    B       loc_FF1EA248 \n"
"    B       loc_FF1EA1B8 \n"
"    B       loc_FF1EA188 \n"

"loc_FF1EA188:\n"
"    STR     R5, [R4, #0x40] \n"
"    STR     R5, [R4, #0x30] \n"
"    STR     R5, [R4, #0x34] \n"
"    STRH    R5, [R4, #6] \n"
"    STR     R6, [R4, #0xD4] \n"
"    STR     R7, [R4, #0xF0] \n"
"    LDR     R0, [R4, #0xC] \n"
"    ADD     R0, R0, #1 \n"
"    STR     R0, [R4, #0xC] \n"
"    MOV     R0, #6 \n"
"    STR     R0, [R4, #0x44] \n"
"    B       loc_FF1EA1D8 \n"

"loc_FF1EA1B8:\n"
"    STR     R5, [R4, #0x40] \n"
"    STR     R5, [R4, #0x30] \n"
"    STR     R6, [R4, #0xD4] \n"
"    STR     R7, [R4, #0xF0] \n"
"    LDR     R0, [R4, #0xC] \n"
"    ADD     R0, R0, #1 \n"
"    STR     R0, [R4, #0xC] \n"
"    STR     R8, [R4, #0x44] \n"

"loc_FF1EA1D8:\n"
"    LDR     R2, =0xFF1E8250 \n"
"    LDR     R1, =0x121A5C \n"
"    LDR     R0, =0xFF1E8364 \n"
"    BL      sub_FF055528 \n"
"    B       loc_FF1EA24C \n"

"loc_FF1EA1EC:\n"
);
if (conf.ext_video_time == 1)
{
asm volatile (
"    BL      sub_FF1E9670_my \n"  // --> Patched. Old value = 0xFF1E9670.
);
}
else
{
asm volatile (
"    BL      sub_FF1E9670 \n"
);
}
asm volatile (
"    B       loc_FF1EA24C \n"

"loc_FF1EA1F4:\n"
"    LDR     R1, [R4, #0xF0] \n"
"    BLX     R1 \n"
//begin patch
"    LDR     R0, =video_compression_rate\n" //added
"    BL      set_quality\n"                 //added
//end patch
"    B       loc_FF1EA24C \n"

"loc_FF1EA200:\n"
"    LDR     R1, [R0, #0x18] \n"
"    LDR     R0, [R0, #4] \n"
"    BL      sub_FF3BFAC8 \n"
"    B       loc_FF1EA24C \n"

"loc_FF1EA210:\n"
"    LDR     R0, [R4, #0x44] \n"
"    CMP     R0, #5 \n"
"    STRNE   R8, [R4, #0x34] \n"
"    B       loc_FF1EA24C \n"

"loc_FF1EA220:\n"
"    BL      sub_FF1E89E0 \n"
"    B       loc_FF1EA24C \n"

"loc_FF1EA228:\n"
"    BL      sub_FF1E86C4 \n"
"    B       loc_FF1EA24C \n"

"loc_FF1EA230:\n"
"    BL      sub_FF1E83C8 \n"
"    B       loc_FF1EA24C \n"

"loc_FF1EA238:\n"
"    BL      sub_FF1E7F78 \n"
"    B       loc_FF1EA24C \n"

"loc_FF1EA240:\n"
"    BL      sub_FF1E7EF8 \n"
"    B       loc_FF1EA24C \n"

"loc_FF1EA248:\n"
"    BL      sub_FF1EA824 \n"

"loc_FF1EA24C:\n"
"    LDR     R1, [SP, #4] \n"
"    LDR     R3, =0xFF1E7B90 /*'MovieRecorder.c'*/ \n"
"    STR     R5, [R1] \n"
"    STR     R9, [SP] \n"
"    LDR     R0, [R4, #0x28] \n"
"    MOV     R2, R10 \n"
"    BL      sub_0068AF18 /*_PostMessageQueueStrictly*/ \n"
"    B       loc_FF1EA110 \n"
);
}

/*************************************************************/
//** sub_FF1E9670_my @ 0xFF1E9670 - 0xFF1E99A8, length=207
void __attribute__((naked,noinline)) sub_FF1E9670_my() {
asm volatile (
"    STMFD   SP!, {R0-R10,LR} \n"
"    LDR     R6, =0xAC60 \n"
"    MOV     R0, #0 \n"
"    STR     R0, [R6, #0x34] \n"
"    STR     R0, [R6, #0x38] \n"
"    MOV     R0, R6 \n"
"    LDR     R4, [R0, #0x58] \n"
"    LDRH    R1, [R6, #6] \n"
"    MOV     R0, #0x3E8 \n"
"    MUL     R0, R4, R0 \n"
"    LDR     R8, =0xFFF00 \n"
"    CMP     R1, #0 \n"
"    MOV     R2, #1 \n"
"    BNE     loc_FF1E96B8 \n"
"    LDR     R1, [R6, #0x90] \n"
"    CMP     R1, #0 \n"
"    BNE     loc_FF1E96C8 \n"
"    B       loc_FF1E96C0 \n"

"loc_FF1E96B8:\n"
"    CMP     R1, #3 \n"
"    BNE     loc_FF1E96C8 \n"

"loc_FF1E96C0:\n"
"    STR     R2, [R6, #0x48] \n"
"    B       loc_FF1E96D4 \n"

"loc_FF1E96C8:\n"
"    MOV     R1, #0x3E8 \n"
"    BL      sub_006ACF38 /*__divmod_unsigned_int*/ \n"
"    STR     R0, [R6, #0x48] \n"

"loc_FF1E96D4:\n"
"    LDR     R5, =0x121A90 \n"
"    MOV     R7, #2 \n"
"    LDR     R0, [R5, #8] \n"
"    CMP     R0, #0 \n"
"    BEQ     loc_FF1E9740 \n"
"    CMP     R4, #0x18 \n"
"    MOV     R0, #4 \n"
"    BEQ     loc_FF1E972C \n"
"    BGT     loc_FF1E9714 \n"
"    CMP     R4, #0xA \n"
"    CMPNE   R4, #0xF \n"
"    STREQ   R7, [R5, #0x14] \n"
"    BEQ     loc_FF1E9740 \n"
"    CMP     R4, #0x14 \n"
"    BNE     loc_FF1E9734 \n"
"    B       loc_FF1E972C \n"

"loc_FF1E9714:\n"
"    CMP     R4, #0x19 \n"
"    CMPNE   R4, #0x1E \n"
"    BEQ     loc_FF1E972C \n"
"    CMP     R4, #0x3C \n"
"    BNE     loc_FF1E9734 \n"
"    MOV     R0, #8 \n"

"loc_FF1E972C:\n"
"    STR     R0, [R5, #0x14] \n"
"    B       loc_FF1E9740 \n"

"loc_FF1E9734:\n"
"    LDR     R0, =0xFF1E7B90 /*'MovieRecorder.c'*/ \n"
"    MOV     R1, #0x790 \n"
"    BL      _DebugAssert \n"

"loc_FF1E9740:\n"
"    LDR     R0, [R6, #0x64] \n"
"    CMP     R0, #1 \n"
"    BNE     loc_FF1E9758 \n"
"    BL      sub_FF372B78 \n"
"    LDR     R0, =0x121AA8 \n"
"    BL      sub_FF3BBF18 \n"

"loc_FF1E9758:\n"
"    LDR     R2, =0xAC62 \n"
"    LDR     R0, [R6, #0xB4] \n"
"    MOV     R3, #2 \n"
"    MOV     R1, #0xAA \n"
"    BL      sub_FF0B6B68 \n"
"    LDR     R2, =0xAC64 \n"
"    LDR     R0, [R6, #0xB4] \n"
"    MOV     R3, #2 \n"
"    MOV     R1, #0xA9 \n"
"    BL      sub_FF0B6B68 \n"
"    LDR     R2, =0xACB0 \n"
"    LDR     R0, [R6, #0xB4] \n"
"    MOV     R3, #4 \n"
"    MOV     R1, #0xA2 \n"
"    BL      sub_FF0B6B68 \n"
"    LDR     R2, =0xACB4 \n"
"    LDR     R0, [R6, #0xB4] \n"
"    MOV     R3, #4 \n"
"    MOV     R1, #0xA3 \n"
"    BL      sub_FF0B6B68 \n"
"    LDR     R0, [R6, #0x90] \n"
"    CMP     R0, #0 \n"
"    LDRNE   R2, =0xAD60 \n"
"    MOVNE   R1, #0 \n"
"    MOVNE   R0, #0xE \n"
"    BLNE    sub_FF0B0EE8 \n"
"    LDR     R0, [R6, #0x4C] \n"
"    LDR     R4, =0xAD60 \n"
"    LDR     R9, =0x121A78 \n"
"    CMP     R0, #1 \n"
"    CMPNE   R0, #2 \n"
"    BNE     loc_FF1E988C \n"
"    LDR     R0, [R6, #0x90] \n"
"    CMP     R0, #0 \n"
"    LDRNE   R0, =0x483FC000 \n"
"    STRNE   R0, [R4] \n"
"    BNE     loc_FF1E988C \n"
"    LDR     R1, =0x499EBCAC \n"
"    LDR     R0, =0x2520754 \n"
"    STR     R1, [R4] \n"
"    STR     R0, [R4, #4] \n"
"    LDMIA   R9, {R0,R2} \n"
"    MUL     R0, R2, R0 \n"
"    MOV     R3, R0, LSL#1 \n"
"    CMP     R3, R8 \n"
"    STR     R3, [R6, #0x9C] \n"
"    MOVCC   R2, #0 \n"
"    STR     R1, [R6, #0xA0] \n"
"    BCC     loc_FF1E9854 \n"
"    MOV     R2, #0 \n"
"    STMEA   SP, {R1,R2,R8} \n"
"    MOV     R3, R2 \n"
"    MOV     R2, #9 \n"
"    MOV     R1, #5 \n"
"    MOV     R0, #0x16 \n"
"    BL      sub_FF36CBDC \n"
"    LDR     R0, [R6, #0x9C] \n"
"    MOV     R2, #0 \n"
"    SUB     R3, R0, #0xF0000 \n"
"    LDR     R0, [R6, #0xA0] \n"
"    SUB     R3, R3, #0xFF00 \n"
"    ADD     R1, R0, #0xF0000 \n"
"    ADD     R1, R1, #0xFF00 \n"

"loc_FF1E9854:\n"
"    STMEA   SP, {R1-R3} \n"
"    MOV     R3, #0 \n"
"    MOV     R2, #9 \n"
"    MOV     R1, #5 \n"
"    MOV     R0, #0x16 \n"
"    BL      sub_FF36CBDC \n"
"    LDR     R1, [R4] \n"
"    LDR     R0, [R6, #0x9C] \n"
"    ADD     R1, R1, R0, LSL#1 \n"
"    STR     R1, [R4] \n"
"    LDR     R1, [R4, #4] \n"
"    RSB     R0, R0, #0 \n"
"    ADD     R0, R1, R0, LSL#1 \n"
"    STR     R0, [R4, #4] \n"

"loc_FF1E988C:\n"
"    LDR     R2, =0x121A90 \n"
"    LDR     R3, =0xFF1E963C \n"
"    LDR     R0, [R4, #4] \n"
"    LDR     R1, [R4] \n"
"    STRD    R2, [SP] \n"
"    SUB     R3, R2, #0x18 \n"
"    MOV     R2, R0 \n"
"    LDR     R0, [R6, #0x90] \n"
"    BL      sub_FF3BE21C_my \n"  // --> Patched. Old value = 0xFF3BE21C.
"    LDRD    R0, [R6, #0xF8] \n"
"    LDR     R2, [R6, #0xB4] \n"
"    BL      sub_FF3BE7F8 \n"
"    LDR     R3, =0xACE8 \n"
"    STR     R3, [SP] \n"
"    LDR     R0, [R6, #0x64] \n"
"    LDR     R1, [R9, #8] \n"
"    AND     R2, R0, #0xFF \n"
"    LDR     R0, [R9] \n"
"    SUB     R3, R3, #4 \n"
"    BL      sub_FF3BBA80 \n"
"    LDRH    R0, [R6, #6] \n"
"    CMP     R0, #2 \n"
"    LDREQ   R0, =0xFF1E9310 \n"
"    STREQ   R0, [R6, #0xF0] \n"
"    LDR     R0, [R6, #0x90] \n"
"    CMP     R0, #0 \n"
"    LDREQ   R1, =0xFF1E8EE0 \n"
"    STREQ   R1, [R6, #0xF0] \n"
"    LDR     R2, [R6, #0xC] \n"
"    LDR     R1, =0xFF60404C \n"
"    CMP     R2, #2 \n"
"    BNE     loc_FF1E9930 \n"
"    LDR     R0, [R6, #0x4C] \n"
"    ADD     R0, R1, R0, LSL#3 \n"
"    LDR     R1, [R9, #0xC] \n"
"    LDR     R0, [R0, R1, LSL#2] \n"
"    BL      sub_FF37190C \n"
"    LDR     R0, =0xFF1E8E54 \n"
"    MOV     R1, #0 \n"
"    BL      sub_FF371E90 \n"
"    B       loc_FF1E9968 \n"

"loc_FF1E9930:\n"
"    CMP     R0, #0 \n"
"    LDR     R0, [R6, #0x4C] \n"
"    ADD     R0, R1, R0, LSL#3 \n"
"    LDR     R1, [R9, #0xC] \n"
"    LDR     R0, [R0, R1, LSL#2] \n"
"    BNE     loc_FF1E9958 \n"
"    LDR     R1, [R6, #0x98] \n"
"    BL      sub_FF371F70 \n"
"    BL      sub_FF371FF4 \n"
"    B       loc_FF1E9968 \n"

"loc_FF1E9958:\n"
"    BL      sub_FF36FAF8 \n"
"    LDR     R0, =0xFF1E8E54 \n"
"    MOV     R1, #0 \n"
"    BL      sub_FF370308 \n"

"loc_FF1E9968:\n"
"    LDR     R0, [R5, #8] \n"
"    CMP     R0, #0 \n"
"    BEQ     loc_FF1E999C \n"
"    ADD     R0, SP, #0xC \n"
"    BL      sub_FF3BF830 \n"
"    LDR     R1, [R5, #0xC] \n"
"    LDR     R0, [SP, #0xC] \n"
"    BL      sub_FF068250 \n"
"    ADD     R0, SP, #0xC \n"
"    BL      sub_FF3BF830 \n"
"    LDR     R1, [R5, #0xC] \n"
"    LDR     R0, [SP, #0xC] \n"
"    BL      sub_FF068250 \n"

"loc_FF1E999C:\n"
"    LDR     R0, =0xFF1E8E0C \n"
"    STR     R7, [R6, #0x44]! \n"
"    STR     R0, [R6, #0x90] \n"
"    LDMFD   SP!, {R0-R10,PC} \n"
);
}

/*************************************************************/
//** sub_FF3BE21C_my @ 0xFF3BE21C - 0xFF3BE79C, length=353
void __attribute__((naked,noinline)) sub_FF3BE21C_my() {
asm volatile (
"    STMFD   SP!, {R0-R11,LR} \n"
"    MOV     R9, R0 \n"
"    LDR     R0, [R3, #0x10] \n"
"    SUB     SP, SP, #0xC \n"
"    CMP     R0, #0 \n"
"    LDR     R7, [SP, #0x40] \n"
"    LDREQ   R1, =0x36A \n"
"    LDREQ   R0, =0xFF3BC50C /*'MovWriter.c'*/ \n"
"    MOV     R6, #0 \n"
"    MOV     R4, R3 \n"
"    MOV     R5, R6 \n"
"    BLEQ    _DebugAssert \n"
"    LDR     R8, =0x10DA0 \n"
"    CMP     R9, #0 \n"
"    MOV     R11, #0 \n"
"    STR     R9, [R8, #0x94] \n"
"    MOVEQ   R0, #1 \n"
"    STRNE   R11, [R8, #0xD8] \n"
"    STREQ   R0, [R8, #0xD8] \n"
"    LDR     R0, [R4] \n"
"    LDR     R3, =0x61A8 \n"
"    STR     R0, [R8, #0xE0] \n"
"    LDR     R0, [R4, #4] \n"
"    LDR     R1, =0x7530 \n"
"    STR     R0, [R8, #0xE4] \n"
"    LDR     R0, [R4, #0x10] \n"
"    MOV     R2, #0x18 \n"
"    STR     R0, [R8, #0xF0] \n"
"    LDR     R9, [R4, #8] \n"
"    LDR     R0, =0x3E9 \n"
"    CMP     R9, #0xF \n"
"    MOV     R12, #0x3E8 \n"
"    MOV     R10, #0x1E \n"
"    ADDCC   PC, PC, R9, LSL#2 \n"
"    B       loc_FF3BE374 \n"
"    B       loc_FF3BE2FC \n"
"    B       loc_FF3BE2F4 \n"
"    B       loc_FF3BE2E4 \n"
"    B       loc_FF3BE350 \n"
"    B       loc_FF3BE360 \n"
"    B       loc_FF3BE374 \n"
"    B       loc_FF3BE374 \n"
"    B       loc_FF3BE374 \n"
"    B       loc_FF3BE374 \n"
"    B       loc_FF3BE31C \n"
"    B       loc_FF3BE314 \n"
"    B       loc_FF3BE30C \n"
"    B       loc_FF3BE348 \n"
"    B       loc_FF3BE340 \n"
"    B       loc_FF3BE32C \n"

"loc_FF3BE2E4:\n"
"    LDR     R5, =0x5DC0 \n"
"    STR     R2, [R8, #0xE8] \n"
"    STR     R5, [R8, #0x13C] \n"
"    B       loc_FF3BE304 \n"

"loc_FF3BE2F4:\n"
"    MOV     R5, R3 \n"
"    B       loc_FF3BE330 \n"

"loc_FF3BE2FC:\n"
"    MOV     R5, R1 \n"
"    B       loc_FF3BE320 \n"

"loc_FF3BE304:\n"
"    STR     R0, [R8, #0x140] \n"
"    B       loc_FF3BE380 \n"

"loc_FF3BE30C:\n"
"    LDR     R5, =0x5DC \n"
"    B       loc_FF3BE320 \n"

"loc_FF3BE314:\n"
"    LDR     R5, =0xBB8 \n"
"    B       loc_FF3BE320 \n"

"loc_FF3BE31C:\n"
"    LDR     R5, =0x1770 \n"

"loc_FF3BE320:\n"
"    STR     R1, [R8, #0x13C] \n"
"    STR     R10, [R8, #0xE8] \n"
"    B       loc_FF3BE304 \n"

"loc_FF3BE32C:\n"
"    LDR     R5, =0x4E2 \n"

"loc_FF3BE330:\n"
"    STR     R2, [R8, #0xE8] \n"
"    STR     R3, [R8, #0x13C] \n"
"    STR     R12, [R8, #0x140] \n"
"    B       loc_FF3BE380 \n"

"loc_FF3BE340:\n"
"    LDR     R5, =0x9C4 \n"
"    B       loc_FF3BE330 \n"

"loc_FF3BE348:\n"
"    LDR     R5, =0x1388 \n"
"    B       loc_FF3BE330 \n"

"loc_FF3BE350:\n"
"    STR     R1, [R8, #0x13C] \n"
"    LDR     R5, =0x57600000 \n"  // --> Patched. Old value = 0x3A980. 2hrs 240fps
"    MOV     R1, #0xF0 \n"
"    B       loc_FF3BE36C \n"

"loc_FF3BE360:\n"
"    LDR     R5, =0x28800000 \n"  // --> Patched. Old value = 0x1D4C0. 2hrs 120fps
"    STR     R1, [R8, #0x13C] \n"
"    MOV     R1, #0x78 \n"

"loc_FF3BE36C:\n"
"    STR     R1, [R8, #0xE8] \n"
"    B       loc_FF3BE304 \n"

"loc_FF3BE374:\n"
"    LDR     R0, =0xFF3BC50C /*'MovWriter.c'*/ \n"
"    MOV     R1, #0x3C4 \n"
"    BL      _DebugAssert \n"

"loc_FF3BE380:\n"
"    LDR     R0, [R8, #0xE8] \n"
"    LDR     R1, =0x1C20 \n"  // --> Patched. Old value = 0xE0F. 2hrs
"    MOV     R0, R0, LSR#1 \n"
"    STR     R0, [R8, #0xEC] \n"
"    LDR     R0, [R7] \n"
"    STR     R0, [R8, #0xF4] \n"
"    LDRH    R0, [R7, #0x10] \n"
"    STR     R0, [R8, #0xF8] \n"
"    LDR     R0, [R7, #4] \n"
"    STRH    R0, [R8, #2] \n"
"    LDR     R0, [R7, #8] \n"
"    STRH    R0, [R8, #4] \n"
"    LDR     R0, [R7, #0x14] \n"
"    STR     R0, [R8, #0xFC] \n"
"    LDR     R0, [SP, #0x44] \n"
"    STR     R0, [R8, #0x138] \n"
"    LDR     R0, [R8, #0xE0] \n"
"    CMP     R0, #0x140 \n"
"    MOVEQ   R0, #0x20000 \n"
"    MOVEQ   R6, #1 \n"
"    STREQ   R0, [R8, #0xB8] \n"
"    BEQ     loc_FF3BE418 \n"
"    CMP     R0, #0x280 \n"
"    LDREQ   R0, =0x7A760 \n"
"    MOVEQ   R6, #2 \n"
"    STREQ   R0, [R8, #0xB8] \n"
"    BEQ     loc_FF3BE418 \n"
"    CMP     R0, #0x500 \n"
"    LDREQ   R0, =0x11DA50 \n"
"    LDR     R1, =0x1C20 \n"  // --> Patched. Old value = 0x257. 2hrs
"    MOVEQ   R6, #4 \n"
"    STREQ   R0, [R8, #0xB8] \n"
"    BEQ     loc_FF3BE418 \n"
"    CMP     R0, #0x780 \n"
"    BNE     loc_FF3BE420 \n"
//"  MOV     R0, #0x200000 \n"
"    MOV     R0, #2097152 \n" // 10 mins
"    MOV     R6, #5 \n"
"    STR     R0, [R8, #0xB8] \n"

"loc_FF3BE418:\n"
"    STR     R1, [R8, #0x54] \n"
"    B       loc_FF3BE42C \n"

"loc_FF3BE420:\n"
"    LDR     R0, =0xFF3BC50C /*'MovWriter.c'*/ \n"
"    MOV     R1, #0x3EC \n"
"    BL      _DebugAssert \n"

"loc_FF3BE42C:\n"
"    LDR     R0, [R8, #0x54] \n"
"    LDR     R1, [R8, #0x140] \n"
"    MUL     R0, R5, R0 \n"
"    STR     R1, [SP, #8] \n"
"    BL      sub_006ACF38 /*__divmod_unsigned_int*/ \n"
"    MOV     R1, #5 \n"
"    BL      sub_006ACF38 /*__divmod_unsigned_int*/ \n"
"    ADD     R0, R0, #1 \n"
"    ADD     R0, R0, R0, LSL#2 \n"
"    STR     R0, [R8, #0x50] \n"
"    LDR     R9, [R4, #8] \n"
"    CMP     R9, #0xB \n"
"    CMPNE   R9, #0xA \n"
"    CMPNE   R9, #9 \n"
"    BNE     loc_FF3BE47C \n"
"    LDR     R1, [SP, #8] \n"
"    MUL     R0, R1, R0 \n"
"    MOV     R1, R5 \n"
"    BL      sub_006ACF38 /*__divmod_unsigned_int*/ \n"
"    STR     R0, [R8, #0x54] \n"

"loc_FF3BE47C:\n"
"    CMP     R9, #3 \n"
"    CMPNE   R9, #4 \n"
"    BNE     loc_FF3BE4B4 \n"
"    RSB     R0, R5, R5, LSL#4 \n"
"    STR     R10, [R8, #0x54] \n"
"    LDR     R1, [SP, #8] \n"
"    MOV     R0, R0, LSL#1 \n"
"    BL      sub_006ACF38 /*__divmod_unsigned_int*/ \n"
"    LDR     R1, [R8, #0xF0] \n"
"    MOV     R10, R1 \n"
"    BL      sub_006ACF38 /*__divmod_unsigned_int*/ \n"
"    ADD     R0, R0, #1 \n"
"    MUL     R0, R10, R0 \n"
"    STR     R0, [R8, #0x50] \n"

"loc_FF3BE4B4:\n"
"    ADD     R0, R5, R5, LSL#8 \n"
"    LDR     R1, [SP, #8] \n"
"    RSB     R0, R0, R0, LSL#3 \n"
"    BL      sub_006ACF38 /*__divmod_unsigned_int*/ \n"
"    STR     R0, [R8, #0x80] \n"
"    LDR     R0, [R4, #0xC] \n"
"    CMP     R0, #0 \n"
"    MOVNE   R0, #1 \n"
"    STR     R0, [SP, #4] \n"
"    LDR     R5, [R7, #0xC] \n"
"    MOV     R0, R5, LSR#1 \n"
"    STR     R5, [R8, #0xA0] \n"
"    STR     R0, [R8, #0xA4] \n"
"    LDR     R0, [SP, #0x10] \n"
"    ADD     R0, R0, #3 \n"
"    BIC     R1, R0, #3 \n"
"    STR     R1, [R8, #0x104] \n"
"    LDR     R0, [R8, #0x50] \n"
"    MOV     R0, R0, LSL#2 \n"
"    ADD     R1, R1, R0 \n"
"    STR     R1, [R8, #0x108] \n"
"    LDRH    R7, [R8, #4] \n"
"    LDR     R2, [SP, #0x14] \n"
"    LDR     R3, [SP, #0x10] \n"
"    CMP     R7, #0 \n"
"    ADD     R10, R3, R2 \n"
"    BEQ     loc_FF3BE750 \n"
"    ADD     R9, R1, R0 \n"
"    STR     R9, [R8, #0x10C] \n"
"    LDR     R1, [R8, #0xEC] \n"
"    BL      sub_006ACF38 /*__divmod_unsigned_int*/ \n"
"    ADD     R0, R0, R9 \n"
"    ADD     R0, R0, #0x1F \n"
"    BIC     R0, R0, #0x1F \n"
"    STR     R0, [R8, #0x110] \n"
"    LDR     R1, [R8, #0xD8] \n"
"    CMP     R1, #0 \n"
"    STREQ   R11, [R8, #0x84] \n"
"    LDREQ   R2, [R8, #0xB8] \n"
"    ADDEQ   R0, R0, R2 \n"
"    STREQ   R0, [R8, #0x114] \n"
"    MOVEQ   R0, #2 \n"
"    STREQ   R0, [R8, #0x40] \n"
"    BEQ     loc_FF3BE604 \n"
"    LDR     R2, [R8, #0xB8] \n"
"    CMP     R7, #1 \n"
"    ADD     R0, R0, R2 \n"
"    STR     R0, [R8, #0x84] \n"
"    ADD     R0, R0, #0x100000 \n"
"    STR     R0, [R8, #0x114] \n"
"    MOV     R2, #5 \n"
"    STR     R2, [R8, #0x40] \n"
"    ADDEQ   R2, R5, R5, LSL#1 \n"
"    SUBEQ   R0, R10, R0 \n"
"    MOVEQ   R2, R2, LSL#1 \n"
"    BEQ     loc_FF3BE614 \n"

"loc_FF3BE594:\n"
//SKIP DATA

"loc_FF3BE604:\n"
"    LDR     R2, [R8, #0x40] \n"
"    LDR     R0, [R8, #0x114] \n"
"    MUL     R2, R5, R2 \n"
"    SUB     R0, R10, R0 \n"

"loc_FF3BE614:\n"
"    SUB     R0, R0, R2 \n"
"    MOV     R0, R0, LSR#15 \n"
"    MOV     R0, R0, LSL#15 \n"
"    STR     R0, [R8, #0x120] \n"
"    LDR     R2, [R8, #0x114] \n"
"    CMP     R1, #0 \n"
"    ADD     R0, R0, R2 \n"
"    STR     R0, [R8, #0x118] \n"
"    BEQ     loc_FF3BE648 \n"
"    CMP     R7, #1 \n"
"    BICEQ   R0, R0, #3 \n"
"    STREQ   R0, [R8, #0x28] \n"
"    ADDEQ   R0, R0, R5 \n"

"loc_FF3BE648:\n"
"    LDR     R1, =0x10DA0 \n"
"    STR     R0, [R8, #0x11C] \n"
"    LDR     R10, =0x2299F0 \n"
"    LDR     R8, [R1, #0x40] \n"
"    LDR     R9, [R1, #0x11C] \n"
"    MOV     R0, #0 \n"
"    SUB     LR, R10, #0x28 \n"

"loc_FF3BE664:\n"
"    CMP     R0, R8 \n"
"    MLACC   R1, R0, R5, R9 \n"
"    LDR     R3, =0x2299A0 \n"
"    ADDCC   R1, R1, #3 \n"
"    BICCC   R1, R1, #3 \n"
"    STRCS   R11, [R10, R0, LSL#2] \n"
"    STRCC   R1, [R10, R0, LSL#2] \n"
"    MOV     R1, #0 \n"
"    ADD     R3, R3, R0, LSL#3 \n"
"    ADD     R12, LR, R0, LSL#3 \n"

"loc_FF3BE68C:\n"
"    STR     R2, [R3, R1, LSL#2] \n"
"    STR     R2, [R12, R1, LSL#2] \n"
"    ADD     R1, R1, #1 \n"
"    CMP     R1, #2 \n"
"    BLT     loc_FF3BE68C \n"
"    ADD     R0, R0, #1 \n"
"    CMP     R0, #5 \n"
"    BLT     loc_FF3BE664 \n"
"    LDR     R5, =0x10DA0 \n"
"    CMP     R7, #1 \n"
"    STRNE   R11, [R5, #0xDC] \n"
"    BNE     loc_FF3BE70C \n"
"    LDRH    R0, [R5, #2] \n"
"    CMP     R0, #0x10 \n"
"    LDRNE   R1, =0x48D \n"
"    LDRNE   R0, =0xFF3BC50C /*'MovWriter.c'*/ \n"
"    BLNE    _DebugAssert \n"
"    MOV     R0, #1 \n"
"    STR     R0, [R5, #0xDC] \n"
"    LDRH    R0, [R5, #4] \n"
"    MVN     R1, #0x10000 \n"
"    AND     R0, R1, R0, LSL#1 \n"
"    STRH    R0, [R5, #4] \n"
"    LDR     R0, [R5, #0xF8] \n"
"    MOV     R0, R0, LSL#1 \n"
"    STR     R0, [R5, #0xF8] \n"
"    LDR     R0, [R5, #0xA0] \n"
"    MOV     R0, R0, LSL#1 \n"
"    STR     R0, [R5, #0xA0] \n"
"    LDR     R0, [R5, #0xA4] \n"
"    MOV     R0, R0, LSL#1 \n"
"    STR     R0, [R5, #0xA4] \n"

"loc_FF3BE70C:\n"
"    LDR     R0, [R5, #0xD8] \n"
"    CMP     R0, #0 \n"
"    BEQ     loc_FF3BE728 \n"
"    LDR     R1, [R4, #8] \n"
"    MOV     R0, R6 \n"
"    BL      sub_FF1078AC \n"
"    B       loc_FF3BE73C \n"

"loc_FF3BE728:\n"
"    LDRH    R3, [R4, #0x14] \n"
"    LDR     R2, [R4, #8] \n"
"    LDR     R1, [SP, #4] \n"
"    MOV     R0, R6 \n"
"    BL      sub_FF1077E0 \n"

"loc_FF3BE73C:\n"
"    LDR     R1, [R5, #0xA0] \n"
"    ADD     R0, R0, R1 \n"
"    STR     R0, [R5, #0x98] \n"

"loc_FF3BE748:\n"
"    ADD     SP, SP, #0x1C \n"
"    LDMFD   SP!, {R4-R11,PC} \n"

"loc_FF3BE750:\n"
"    ADD     R0, R0, R1 \n"
"    ADD     R0, R0, #0x1F \n"
"    BIC     R0, R0, #0x1F \n"
"    STR     R0, [R8, #0x110] \n"
"    LDR     R1, [R8, #0xB8] \n"
"    MOV     R2, R9 \n"
"    ADD     R0, R0, R1 \n"
"    SUB     R1, R10, R0 \n"
"    MOV     R1, R1, LSR#15 \n"
"    MOV     R1, R1, LSL#15 \n"
"    STR     R0, [R8, #0x114] \n"
"    ADD     R0, R0, R1 \n"
"    STR     R1, [R8, #0x120] \n"
"    STR     R0, [R8, #0x118] \n"
"    LDRH    R3, [R4, #0x14] \n"
"    LDR     R1, [SP, #4] \n"
"    MOV     R0, R6 \n"
"    BL      sub_FF1077E0 \n"
"    STR     R0, [R8, #0x98] \n"
"    B       loc_FF3BE748 \n"
);
}
