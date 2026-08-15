# Shimu Usage Guide

Shimu is a virtual CPU that runs Nirdeshika-16 binary files. This guide covers everything you need to use it.

---

## Command-Line Usage

```bash
./bin/shimu <path-to-file.ব>
```

Shimu accepts exactly one argument: the path to a `.ব` (Bengali script `.b`) binary file.

### Example

```bash
./bin/shimu examples/গণিত.ব
```

If no argument is provided, Shimu prints:

```
--- শিমু (Shimu) সিমুলেটর v১.০ ---
স্থাপত্য: নির্দেশিকা-১৬ | আর্কিটেক্ট: রাহাত হাসান

ব্যবহার: shimu <ফাইল_পাথ.ব>
```

---

## Input File Format

Shimu reads **raw binary `.ব` files** — the output of the Boyon assembler. These files contain Nirdeshika-16 machine code with no headers, metadata, or relocation information.

### Instruction Encoding

Each instruction is exactly **2 bytes (16 bits)** in little-endian format:

```
Bit:  15  14  13  12  |  11  10  9  8  |  7  6  5  4  3  2  1  0
      [    opcode    ]  [   reg_id    ]  [       operand        ]
```

- **opcode** (4 bits): selects the operation
- **reg_id** (4 bits): selects the target register (S0–S15)
- **operand** (8 bits): immediate value or address (0–255)

### Example: Binary for `LOAD S1, 40`

The instruction `LOAD S1, 40` encodes as:
- opcode = 0x2 (LOAD)
- reg_id = 0x1 (S1)
- operand = 0x28 (40)

Binary: `0010 0001 0010 1000` → `0x2128`

In little-endian bytes in the `.ব` file:
```
Address 0: 0x28  (low byte)
Address 1: 0x21  (high byte)
```

---

## Output File Explanation

Shimu creates a file named `সিমুলেটর_ফলাফল.txt` in the same directory as the input file.

### Output Structure

The report contains these sections:

1. **Header** — architecture name and architect credit
2. **OUTPUT lines** — one line per `OUT` instruction executed (shows register ID and value)
3. **Halt message** — confirms the processor stopped
4. **Register dump** — all 16 registers (S0–S15) in Akkhar-Hex

### Sample Output

```
--- নির্দেশিকা-১৬ সিমুলেশন রিপোর্ট ---
আর্কিটেক্ট: রাহাত হাসান | অক্ষর ল্যাবস

[আউটপুট] সাময়িক-১ এর বর্তমান মান: ৪০ (ষোড়শিক: ০ষ২৮)
[আউটপুট] সাময়িক-২ এর বর্তমান মান: ১২ (ষোড়শিক: ০ষগ)

সিস্টেম: প্রসেসর থামানো হয়েছে (থামো).

সিস্টেম: এক্সিকিউশন সফলভাবে সম্পন্ন হয়েছে।

--- রেজিস্টার অবস্থা (চূড়ান্ত অবস্থা) ---
সাময়িক-০: ০ষ০   সাময়িক-১: ০ষগ   সাময়িক-২: ০ষ১৯   সাময়িক-৩: ০ষ০
সাময়িক-৪: ০ষ০   সাময়িক-৫: ০ষ০   সাময়িক-৬: ০ষ০   সাময়িক-৭: ০ষ০
সাময়িক-৮: ০ষ০   সাময়িক-৯: ০ষ০   সাময়িক-১০: ০ষ০   সাময়িক-১১: ০ষ০
সাময়িক-১২: ০ষঙ   সাময়িক-১৩: ০ষ০   সাময়িক-১৪: ০ষ০   সাময়িক-১৫: ০ষ০
---------------------------------------
```

### Reading the Output

- **সাময়িক-N** — Register S_N (e.g., `সাময়িক-১` = register S1)
- **ষোড়শিক** — Akkhar-Hex (Bengali hexadecimal), prefixed with `০ষ`
- **০ষ২৮** — Akkhar-Hex value 0x28 (decimal 40)
- **০ষগ** — Akkhar-Hex value 0x0C (decimal 12), where `গ` = C
- **০ষঙ** — Akkhar-Hex value 0x0E (decimal 14), where `ঙ` = E

---

## Error Messages

| Error | Meaning |
|-------|---------|
| `ভুল: ফাইলটি খোলা সম্ভব হয়নি! পাথ চেক করুন।` | The `.ব` file was not found or cannot be read |
| `ভুল: আউটপুট লগ ফাইল তৈরি করা যায়নি।` | Cannot create `সিমুলেটর_ফলাফল.txt` (permissions or disk space) |
| `ভুল: শূন্য দিয়ে ভাগ করার চেষ্টা (Division by Zero)!` | A `DIV` instruction was executed with operand 0; execution halts |
| `ভুল: অজানা ওকোড {hex}` | An opcode value that is not in the 0x0–0xF range was encountered; execution halts |
| `ভুল: মেমোরি আউট-অফ-বাউন্ড এরর!` | Program counter exceeded 64KB; execution stops |

---

## Examples

### Example 1: Load and Display

**Input binary** (`test.ব`): bytes that encode:
```
LOAD S1, 40      → 0x2128
OUT S1           → 0xE100
HALT             → 0xF000
```

**In hex bytes** (little-endian):
```
28 21 00 E1 00 F0
```

**Run:**
```bash
./bin/shimu test.ব
```

**Output** (`সিমুলেটর_ফলাফল.txt`):
```
--- নির্দেশিকা-১৬ সিমুলেশন রিপোর্ট ---
আর্কিটেক্ট: রাহাত হাসান | অক্ষর ল্যাবস

[আউটপুট] সাময়িক-১ এর বর্তমান মান: ৪০ (ষোড়শিক: ০ষ২৮)

সিস্টেম: প্রসেসর থামানো হয়েছে (থামো).

সিস্টেম: এক্সিকিউশন সফলভাবে সম্পন্ন হয়েছে।

--- রেজিস্টার অবস্থা (চূড়ান্ত অবস্থা) ---
সাময়িক-০: ০ষ০   সাময়িক-১: ০ষ২৮   সাময়িক-২: ০ষ০   ...
```

### Example 2: Arithmetic

**Instructions:**
```
LOAD S1, 30       → 0x211E
LOAD S2, 80       → 0x2250
ADD S1, 10        → 0x410A
OUT S1            → 0xE100
HALT              → 0xF000
```

**In hex bytes** (little-endian):
```
1E 21 50 22 0A 41 00 E1 00 F0
```

Register S1 starts at 30, adds 10, and displays 40 (0x28).

### Example 3: Multiplication with Overflow

**Instructions:**
```
LOAD S1, 200      → 0x21C8
MUL S1, 5         → 0x6105
OUT S1            → 0xE100
HALT              → 0xF000
```

200 × 5 = 1000 (0x3E8), which fits in 16 bits. No overflow flag.
If the result exceeds 65535, the Overflow flag (bit 1 of S14) is set.

### Example 4: Conditional Jump

**Instructions:**
```
LOAD S1, 5        → 0x2105
SUB S1, 5         → 0x5105
JZ 12             → 0xD00C
OUT S1            → 0xE100
HALT              → 0xF000
```

S1 starts at 5, subtracts 5 (result is 0 → Zero flag set), then jumps to address 12 (0x0C). Since the jump target is at offset 12 (after the JZ instruction), execution continues from there. If the result were non-zero, the JZ would not jump and the OUT/HALT would execute.

### Example 5: Division by Zero (Error)

**Instructions:**
```
LOAD S1, 10       → 0x210A
DIV S1, 0         → 0x7100
HALT              → 0xF000
```

The `DIV` with operand 0 triggers:
```
ভুল: শূন্য দিয়ে ভাগ করার চেষ্টা (Division by Zero)!
```
Execution halts immediately.

---

## Workflow

### Typical Workflow

1. Write a Boyon program (`.বয়ন` file)
2. Assemble it with Boyon: `./bin/boyon program.বয়ন` → produces `program.ব`
3. Simulate it with Shimu: `./bin/shimu program.ব`
4. Read the results in `সিমুলেটর_ফলাফল.txt`

### Build Shimu

```bash
make
```

Or compile manually:

```bash
gcc -Iinclude src/main.c src/cpu.c src/memory.c src/alu.c src/io.c src/utils/formatting.c -o bin/shimu
```