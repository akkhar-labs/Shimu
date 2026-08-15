/* 
 * nirdeshika16.h - Hardware Mapping for Shimu Simulator
 * This defines the registers, memory limits, and opcode logic for the 16-bit stack.
 * Architect: Rahat Hasan | Akkhar Labs
 */

#ifndef NIRDESHIKA16_H
#define NIRDESHIKA16_H

#include <stdint.h>

/* --- মেমোরি কনফিগারেশন --- */
// ১৬-বিট অ্যাড্রেস স্পেস = ২^১৬ = ৬৫,৫৩৬ বাইট
#define MEMORY_SIZE 65536

/* --- ওকোড (Opcode) সারণী --- 
 * নির্দেশিকা-১৬ এর ১৬টি ওকোড (৪-বিট)
 */
typedef enum {
    OP_NOP   = 0x0, // শূণ্য
    OP_MOV   = 0x1, // স্থানান্তর
    OP_LOAD  = 0x2, // আদান
    OP_STORE = 0x3, // প্রদান
    OP_ADD   = 0x4, // সংযোজন
    OP_SUB   = 0x5, // বিয়োজন
    OP_MUL   = 0x6, // গুনন
    OP_DIV   = 0x7, // বিভাজন
    OP_AND   = 0x8, // উভয়
    OP_OR    = 0x9, // অথবা
    OP_XOR   = 0xA, // বিচিত্র
    OP_NOT   = 0xB, // বিপরীত
    OP_JMP   = 0xC, // প্রস্থান
    OP_JZ    = 0xD, // সাম্য-প্রস্থান
    OP_OUT   = 0xE, // প্রদর্শন
    OP_HALT  = 0xF  // থামো
} Opcode;

/* --- রেজিস্টার ফাইল (Register File) --- 
 * মোট ১৬টি সাধারণ সাময়িক এবং ৩টি বিশেষ সাময়িক
 */
typedef enum {
    REG_S0 = 0,  REG_S1,  REG_S2,  REG_S3,
    REG_S4,      REG_S5,  REG_S6,  REG_S7,
    REG_S8,      REG_S9,  REG_S10, REG_S11,
    REG_S12,     REG_S13, REG_S14, REG_S15,
    NUM_REGISTERS = 16
} RegisterID;

/* --- বিশেষ সাময়িক ম্যাপিং --- */
#define PC_REG    REG_S12 // ন-নি (Program Counter)
#define SP_REG    REG_S13 // শ-নি (Stack Pointer)
#define FLAG_REG  REG_S14 // অ-সা (Status Flags)

/* --- অবস্থা-সাময়িক (Status Flags) বিট ম্যাপ --- 
 * অ-সা রেজিস্টারের ভেতরের বিট বিন্যাস
 */
#define FLAG_ZERO     0  // বিট ০: ফলাফল শূন্য হলে ১ হবে
#define FLAG_OVERFLOW 1  // বিট ১: উপচয় বা ওভারফ্লো হলে ১ হবে
#define FLAG_NEGATIVE 2  // bit ২: ফলাফল ঋণাত্মক হলে ১ হবে

/* --- ইনস্ট্রাকশন ডিকোডিং ম্যাক্রো (৪-৪-৮ ফরম্যাট) --- */
#define GET_OPCODE(instr) ((instr >> 12) & 0x0F)
#define GET_REGID(instr)  ((instr >> 8) & 0x0F)
#define GET_VALUE(instr)  (instr & 0xFF)

#endif // NIRDESHIKA16_H