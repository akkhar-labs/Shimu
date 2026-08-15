# Shimu Architecture

Shimu is a software-defined reference simulator for the **Nirdeshika-16** 16-bit instruction set architecture. It implements a complete fetch-decode-execute cycle, virtual memory, a register file, status flags, and stream-based I/O — all in portable C99.

---

## CPU Core Design

### State Structure (`include/cpu.h`)

```c
typedef struct {
    uint16_t registers[NUM_REGISTERS];
    uint16_t pc;
    uint16_t sp;
    uint16_t flags;
    int is_halted;
} CPU;
```

The `CPU` struct holds the entire processor state:

- **registers[16]** — 16 × 16-bit register file (S0–S15). Register S0 is hardwired to zero; writes to it are silently ignored in all opcodes.
- **pc** — Program Counter. Mirrors the value in `REG_S12` (PC_REG) and is updated on every instruction step.
- **sp** — Stack Pointer. Mirrors the value in `REG_S13` (SP_REG). Currently reserved for future stack operations.
- **flags** — Status flags. Stored in `REG_S14` (FLAG_REG), which is part of the register file.
- **is_halted** — Boolean flag that stops the execution loop when set.

### Initialization

`init_cpu()` zeroes the entire register file, sets PC/SP/flags to 0, and clears the halt flag. Register S0 is explicitly set to 0 after the memset as a safety measure.

### Instruction Cycle

The main loop in `main.c` calls `step_cpu()` repeatedly until `cpu.is_halted` is true. Each step:

1. **Fetch** — reads a 16-bit word from memory at `cpu.pc`
2. **Decode** — extracts opcode (bits 15–12), register ID (bits 11–8), and immediate operand (bits 7–0)
3. **Execute** — dispatches to the appropriate operation via a switch statement
4. **Advance PC** — increments `cpu.pc` by 2 (unless a jump or halt occurred)

```c
while (!cpu.is_halted) {
    step_cpu(&cpu, log_file);
    if (cpu.pc >= MEMORY_SIZE) {
        // out-of-bounds error
        break;
    }
}
```

---

## Memory Model

### Address Space

- **Size:** 65,536 bytes (64 KB), addressable by 16-bit addresses
- **Storage:** Static `uint8_t virtual_ram[MEMORY_SIZE]` array in `src/memory.c`
- **Initialization:** `init_memory()` zeroes the entire array

### Endianness

All 16-bit word accesses use **little-endian** byte ordering:

```
Address N:     low byte  (bits 7–0)
Address N+1:   high byte (bits 15–8)
```

`memory_read_word()` reconstructs a word as `low | (high << 8)`.
`memory_write_word()` splits a word and stores the low byte at the lower address.

### API

| Function | Description |
|----------|-------------|
| `memory_read_byte(address)` | Read 1 byte from virtual RAM |
| `memory_write_byte(address, value)` | Write 1 byte to virtual RAM |
| `memory_read_word(address)` | Read 16-bit word (little-endian) |
| `memory_write_word(address, value)` | Write 16-bit word (little-endian) |
| `get_memory_ptr()` | Returns raw pointer to the internal buffer (used by the binary loader in `main.c`) |

The binary loader in `main.c` uses `fread(get_memory_ptr(), 1, MEMORY_SIZE, file)` to load the entire `.ব` file directly into the memory array starting at address 0.

---

## Register Set

### General-Purpose Registers

| ID | Name | Purpose |
|----|------|---------|
| 0  | S0   | Hardwired zero (writes ignored) |
| 1  | S1   | General purpose |
| 2  | S2   | General purpose |
| 3  | S3   | General purpose |
| 4  | S4   | General purpose |
| 5  | S5   | General purpose |
| 6  | S6   | General purpose |
| 7  | S7   | General purpose |
| 8  | S8   | General purpose |
| 9  | S9   | General purpose |
| 10 | S10  | General purpose |
| 11 | S11  | General purpose |

### Special-Purpose Registers

| ID | Name | Alias | Description |
|----|------|-------|-------------|
| 12 | S12  | PC_REG | Program Counter — holds the address of the next instruction |
| 13 | S13  | SP_REG | Stack Pointer — reserved for stack operations |
| 14 | S14  | FLAG_REG | Status Flags — bitfield for Zero, Overflow, Negative |
| 15 | S15  | — | General purpose (no special mapping) |

The `cpu.pc` and `cpu.sp` struct fields mirror `registers[PC_REG]` and `registers[SP_REG]`. The `cpu.flags` field is separate from the register file; the actual flag bits are stored in `registers[FLAG_REG]`.

---

## Instruction Execution

### Instruction Encoding (4-4-8 Format)

```
15  14  13  12  |  11  10  9  8  |  7  6  5  4  3  2  1  0
    opcode      |    reg_id      |          operand
```

- **Bits 15–12:** Opcode (4 bits, 16 possible values)
- **Bits 11–8:** Register ID (4 bits, 16 registers)
- **Bits 7–0:** Immediate operand (8 bits, 0–255)

Decoding macros:

```c
#define GET_OPCODE(instr) ((instr >> 12) & 0x0F)
#define GET_REGID(instr)  ((instr >> 8) & 0x0F)
#define GET_VALUE(instr)  (instr & 0xFF)
```

### Opcode Table

| Hex | Mnemonic | Name | Description |
|-----|----------|------|-------------|
| 0x0 | NOP | শূণ্য | No operation |
| 0x1 | MOV | স্থানান্তর | Load immediate into register, set Zero flag |
| 0x2 | LOAD | আদান | Load immediate into register (no flags) |
| 0x3 | STORE | প্রদান | Write register value to memory address (operand = address) |
| 0x4 | ADD | সংযোজন | Add immediate to register, set Zero flag |
| 0x5 | SUB | বিয়োজন | Subtract immediate from register, set Zero flag |
| 0x6 | MUL | গুনন | Multiply register by immediate, set Zero and Overflow flags |
| 0x7 | DIV | বিভাজন | Divide register by immediate, set Zero flag; halt on zero divisor |
| 0x8 | AND | উভয় | Bitwise AND register with immediate, set Zero flag |
| 0x9 | OR | অথবা | Bitwise OR register with immediate, set Zero flag |
| 0xA | XOR | বিচিত্র | Bitwise XOR register with immediate, set Zero flag |
| 0xB | NOT | বিপরীত | Bitwise NOT register, set Zero flag |
| 0xC | JMP | প্রস্থান | Unconditional jump to operand address |
| 0xD | JZ | সাম্য-প্রস্থান | Conditional jump if Zero flag is set |
| 0xE | OUT | প্রদর্শন | Output register ID and value to the log stream |
| 0xF | HALT | থামো | Stop execution |

### Execution Details

**MOV vs LOAD:** Both load an immediate value into a register. MOV additionally sets the Zero flag based on the loaded value; LOAD does not update any flags.

**MUL:** Performs 16×16-bit multiplication using 32-bit intermediate. The result is truncated to the lower 16 bits. The Overflow flag is set if the result exceeds 16 bits.

**DIV:** Divides the register value by the immediate operand. If the operand is zero, the processor halts immediately with a Bengali error message. Division results are truncated (integer division).

**JMP/JZ:** Jumps set `cpu.pc` to the operand value and update `cpu.registers[PC_REG]` accordingly. The `pc_updated` flag prevents the automatic `pc += 2` advance.

**OUT:** Writes to the output stream (the log file). Output format: `[আউটপুট] সাময়িক-{reg_id} এর বর্তমান মান: {decimal} (ষোড়শিক: {akkhar_hex})`.

---

## Flags Handling

### Flag Register Layout

The status flags reside in `registers[FLAG_REG]` (S14):

| Bit | Name | Macro | Set when |
|-----|------|-------|----------|
| 0 | Zero | FLAG_ZERO | Result equals 0 |
| 1 | Overflow | FLAG_OVERFLOW | Multiplication result exceeds 16 bits |
| 2 | Negative | FLAG_NEGATIVE | Result is negative (reserved, not yet implemented) |

### Flag APIs

```c
void set_flag(CPU *cpu, int flag_bit, int value);
int  get_flag(CPU *cpu, int flag_bit);
```

- `set_flag` sets or clears the specified bit in `registers[FLAG_REG]` using bitwise operations
- `get_flag` reads the specified bit by shifting and masking

### Flag Update Rules

| Opcode | Zero | Overflow | Negative |
|--------|------|----------|----------|
| MOV | ✓ | — | — |
| ADD | ✓ | — | — |
| SUB | ✓ | — | — |
| MUL | ✓ | ✓ | — |
| DIV | ✓ | — | — |
| AND | ✓ | — | — |
| OR | ✓ | — | — |
| XOR | ✓ | — | — |
| NOT | ✓ | — | — |
| NOP/LOAD/STORE/JMP/JZ/OUT/HALT | — | — | — |

---

## I/O System

### Output Stream

Shimu uses a **stream-based output model**. All output is written to a `FILE*` stream — either the log file or stdout.

- `step_cpu()` accepts a `FILE *output_stream` parameter
- The `OUT` opcode writes to this stream using `fprintf`
- Error messages (unknown opcode, division by zero, out-of-bounds) are also written to this stream
- The final register dump is written to the same stream

### Output File

The output file is named `সিমুলেটর_ফলাফল.txt` (Simulator Results) and is created in the same directory as the input `.ব` file. If the input path has no directory component, the file is created in the current working directory.

### Formatting Engine (`src/utils/formatting.c`)

Three formatting functions transform numeric values into Bengali script:

| Function | Purpose |
|----------|---------|
| `fprint_bengali_int(stream, value)` | Converts an integer to Bengali digits (`০`–`৯`) |
| `fprint_bengali_hex(stream, value)` | Converts a 16-bit value to Akkhar-Hex (`০ষ` prefix + Bengali digits for 0–9, `ক`–`চ` for 10–15) |
| `fprint_bengali_address(stream, addr)` | Fixed 4-digit Akkhar-Hex address (no `০ষ` prefix, no leading-zero suppression) |

### Akkhar-Hex Mapping

| Hex Value | Bengali Symbol |
|-----------|---------------|
| 0–9 | `০`–`৯` |
| 10 (A) | `ক` |
| 11 (B) | `খ` |
| 12 (C) | `গ` |
| 13 (D) | `ঘ` |
| 14 (E) | `ঙ` |
| 15 (F) | `চ` |

Akkhar-Hex values are prefixed with `০ষ` (which reads as "0x" in Bengali). Leading zeros are suppressed in `fprint_bengali_hex` but preserved in `fprint_bengali_address`.

---

## Project Modules

| Module | File | Responsibility |
|--------|------|----------------|
| **Main** | `src/main.c` | CLI argument parsing, file loading, CPU lifecycle, output file creation |
| **CPU** | `src/cpu.c` | Processor state, instruction decode/execute, flag management, state dump |
| **Memory** | `src/memory.c` | 64KB virtual RAM, byte/word access with little-endian encoding |
| **ALU** | `src/alu.c` | Reserved for future ALU abstraction |
| **I/O** | `src/io.c` | Reserved for future I/O abstraction |
| **Formatting** | `src/utils/formatting.c` | Bengali numeral and Akkhar-Hex conversion |