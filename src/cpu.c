/* 
 * cpu.c - Implementation of the Nirdeshika-16 Core Engine
 * Updated: Implemented Multiplication (OP_MUL) and Division (OP_DIV) with safety checks.
 * Architect: Rahat Hasan | Akkhar Labs
 */

#include "cpu.h"
#include "memory.h"
#include "utils/formatting.h"
#include <string.h>

// সিপিইউ এবং রেজিস্টারসমূহ প্রাথমিক অবস্থায় আনা
void init_cpu(CPU *cpu) {
    memset(cpu->registers, 0, sizeof(cpu->registers));
    cpu->registers[0] = 0;
    cpu->pc = 0;
    cpu->sp = 0;
    cpu->flags = 0;
    cpu->is_halted = 0;
}

// অবস্থা-সাময়িক (Status Flags) বিট সেট করা
void set_flag(CPU *cpu, int flag_bit, int value) {
    if (value) {
        cpu->registers[FLAG_REG] |= (1 << flag_bit);
    } else {
        cpu->registers[FLAG_REG] &= ~(1 << flag_bit);
    }
}

// অবস্থা-সাময়িক (Status Flags) বিটের মান দেখা
int get_flag(CPU *cpu, int flag_bit) {
    return (cpu->registers[FLAG_REG] >> flag_bit) & 1;
}

// নির্দিষ্ট নির্দেশের সাইকেল চালানো (মেমোরি মডিউল ব্যবহার করে)
void step_cpu(CPU *cpu, FILE *output_stream) {
    if (cpu->is_halted) return;

    uint16_t instr = memory_read_word(cpu->pc);
    uint8_t opcode = GET_OPCODE(instr);
    uint8_t reg_id = GET_REGID(instr);
    uint8_t operand = GET_VALUE(instr);

    int pc_updated = 0;

    switch (opcode) {
        case OP_NOP: break;

        case OP_MOV:
            if (reg_id != 0) cpu->registers[reg_id] = operand;
            set_flag(cpu, FLAG_ZERO, (cpu->registers[reg_id] == 0));
            break;

        case OP_LOAD:
            if (reg_id != 0) cpu->registers[reg_id] = operand;
            break;

        case OP_STORE:
            memory_write_word(operand, cpu->registers[reg_id]);
            break;

        case OP_ADD:
            if (reg_id != 0) cpu->registers[reg_id] += operand;
            set_flag(cpu, FLAG_ZERO, (cpu->registers[reg_id] == 0));
            break;

        case OP_SUB:
            if (reg_id != 0) cpu->registers[reg_id] -= operand;
            set_flag(cpu, FLAG_ZERO, (cpu->registers[reg_id] == 0));
            break;

        case OP_MUL: {
            uint32_t result = (uint32_t)cpu->registers[reg_id] * (uint32_t)operand;
            if (reg_id != 0) cpu->registers[reg_id] = (uint16_t)(result & 0xFFFF);
            set_flag(cpu, FLAG_ZERO, (cpu->registers[reg_id] == 0));
            set_flag(cpu, FLAG_OVERFLOW, (result > 0xFFFF));
            break;
        }

        case OP_DIV: {
            if (operand == 0) {
                fprintf(output_stream, "ভুল: শূন্য দিয়ে ভাগ করার চেষ্টা (Division by Zero)!\n");
                cpu->is_halted = 1;
            } else {
                if (reg_id != 0) cpu->registers[reg_id] /= operand;
                set_flag(cpu, FLAG_ZERO, (cpu->registers[reg_id] == 0));
            }
            break;
        }

        case OP_AND:
            if (reg_id != 0) cpu->registers[reg_id] &= operand;
            set_flag(cpu, FLAG_ZERO, (cpu->registers[reg_id] == 0));
            break;

        case OP_OR:
            if (reg_id != 0) cpu->registers[reg_id] |= operand;
            set_flag(cpu, FLAG_ZERO, (cpu->registers[reg_id] == 0));
            break;

        case OP_XOR:
            if (reg_id != 0) cpu->registers[reg_id] ^= operand;
            set_flag(cpu, FLAG_ZERO, (cpu->registers[reg_id] == 0));
            break;

        case OP_NOT:
            if (reg_id != 0) cpu->registers[reg_id] = ~cpu->registers[reg_id];
            set_flag(cpu, FLAG_ZERO, (cpu->registers[reg_id] == 0));
            break;

        case OP_JMP:
            cpu->pc = operand;
            cpu->registers[PC_REG] = cpu->pc;
            pc_updated = 1;
            break;

        case OP_JZ:
            if (get_flag(cpu, FLAG_ZERO)) {
                cpu->pc = operand;
                cpu->registers[PC_REG] = cpu->pc;
                pc_updated = 1;
            }
            break;

        case OP_OUT:
            fprintf(output_stream, "[আউটপুট] সাময়িক-");
            fprint_bengali_int(output_stream, reg_id);
            fprintf(output_stream, " এর বর্তমান মান: ");
            fprint_bengali_int(output_stream, cpu->registers[reg_id]);
            fprintf(output_stream, " (ষোড়শিক: ");
            fprint_bengali_hex(output_stream, cpu->registers[reg_id]);
            fprintf(output_stream, ")\n");
            break;

        case OP_HALT:
            cpu->is_halted = 1;
            fprintf(output_stream, "\nসিস্টেম: প্রসেসর থামানো হয়েছে (থামো).\n");
            break;

        default:
            fprintf(output_stream, "ভুল: অজানা ওকোড ");
            fprint_bengali_hex(output_stream, opcode);
            fprintf(output_stream, "\n");
            cpu->is_halted = 1;
            break;
    }

    if (!cpu->is_halted && !pc_updated) {
        cpu->pc += 2;
        cpu->registers[PC_REG] = cpu->pc;
    }
}

// রেজিস্টারের অবস্থা নির্দিষ্ট স্ট্রিমে দেখানো
void dump_cpu_state(CPU *cpu, FILE *output_stream) {
    fprintf(output_stream, "\n--- রেজিস্টার অবস্থা (চূড়ান্ত অবস্থা) ---\n");
    for (int i = 0; i < NUM_REGISTERS; i++) {
        fprintf(output_stream, "সাময়িক-");
        fprint_bengali_int(output_stream, i);
        fprintf(output_stream, ": ");
        fprint_bengali_hex(output_stream, cpu->registers[i]);
        fprintf(output_stream, "   ");
        if ((i + 1) % 4 == 0) fprintf(output_stream, "\n");
    }
    fprintf(output_stream, "---------------------------------------\n");
}