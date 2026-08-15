[বাংলায় পড়ুন](README.bn.md)

# Shimu — A Virtual CPU for the Nirdeshika-16 Architecture

**Shimu** is a reference simulator (virtual CPU) for the **Nirdeshika-16** instruction set. It loads raw binary `.ব` files — produced by the Boyon assembler — directly into memory and executes them like a real processor.

All output appears in Bengali, using the native **Akkhar-Hex** numeral system.

---

## Features

- **16-opcode execution** — full Nirdeshika-16 ISA: arithmetic, logic, jumps, I/O, and halt
- **64KB virtual memory** — 65,536 bytes of software-defined RAM with little-endian word access
- **16 registers** — 13 general-purpose (S0–S11) + 3 special (PC, SP, Flags)
- **Fetch-decode-execute loop** — cycles through instructions at the hardware level
- **Bengali output** — register values, addresses, and results printed in Bengali numerals and Akkhar-Hex
- **Akkhar-Hex notation** — a sovereign hexadecimal system using Bengali digits (`০`–`৯`) and Bengali consonants (`ক`–`চ` for A–F)
- **Status flags** — Zero, Overflow, and Negative flags updated on every operation
- **Division-by-zero protection** — halts execution with a Bengali error message
- **Stream-based I/O** — writes a structured simulation report to `সিমুলেটর_ফলাফল.txt`

---

## Installation

### Prerequisites

- GCC (or any C99-compatible compiler)
- Make

### Build

```bash
make
```

Or manually:

```bash
gcc -Iinclude src/main.c src/cpu.c src/memory.c src/alu.c src/io.c src/utils/formatting.c -o bin/shimu
```

The binary is placed in `bin/shimu` (or `bin/shimu.exe` on Windows).

---

## Usage

```bash
./bin/shimu <file.ব>
```

Example:

```bash
./bin/shimu examples/গণিত.ব
```

Shimu loads the binary file into virtual memory starting at address 0, then runs the fetch-decode-execute loop until a `HALT` instruction is encountered or memory bounds are exceeded.

---

## Output Explanation

Shimu produces two outputs:

1. **Terminal** — shows the startup banner, bytes loaded, and the output file path
2. **`সিমুলেটর_ফলাফল.txt`** — a detailed simulation report containing:
   - Printed output from `OUT` instructions
   - Halt confirmation
   - Final register state dump (all 16 registers in Akkhar-Hex)

### Sample output

```
--- নির্দেশিকা-১৬ সিমুলেশন রিপোর্ট ---
আর্কিটেক্ট: রাহাত হাসান | অক্ষর ল্যাবস

[আউটপুট] সাময়িক-১ এর বর্তমান মান: ৪০ (ষোড়শিক: ০ষ২৮)

সিস্টেম: প্রসেসর থামানো হয়েছে (থামো).

--- রেজিস্টার অবস্থা (চূড়ান্ত অবস্থা) ---
সাময়িক-০: ০ষ০   সাময়িক-১: ০ষ২৮   সাময়িক-২: ০ষ০   ...
```

---

## Language Strategy

Shimu is part of the **Akkhar Labs** ecosystem, which is built on a simple principle: **computing in your own language**.

- All source code comments are in Bengali
- All runtime output is in Bengali
- The numeral system uses Bengali digits (`০ ১ ২ ৩ ৪ ৫ ৬ ৭ ৮ ৯`)
- The hexadecimal system (Akkhar-Hex) uses Bengali consonants (`ক খ গ ঘ ঙ চ`) for values 10–15

This means a Bengali-speaking user can understand the simulator's output without knowing English or the Latin alphabet.

## Development

### Building

```bash
make
```

### Adding a new opcode

1. Add the opcode to the `Opcode` enum in `include/arch/nirdeshika16.h`
2. Implement the case in the `switch` block inside `step_cpu()` in `src/cpu.c`
3. Update the operand decoder macros if the encoding changes

### Testing

Create a `.ব` binary file using the Boyon assembler, run it through Shimu, and inspect the `সিমুলেটর_ফলাফল.txt` output.

---

## License

GNU General Public License v3.0 — see [LICENSE](LICENSE).

---

**Architect:** Rahat Hasan  
**Lab:** Akkhar Labs