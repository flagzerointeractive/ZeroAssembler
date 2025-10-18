**ZSM Assembler**

[![CodeFactor](https://www.codefactor.io/repository/github/flagzerointeractive/zeroassembler/badge/main)](https://www.codefactor.io/repository/github/flagzerointeractive/zeroassembler/overview/main)

**About:** 
The ZSM Assmbler assembles binaries for the [ZVM Project](https://github.com/flagzerointeractive/ZeroVirtualMachine).  Information about ZCode and the Virtual Hardware Can be found in [this repository](https://github.com/flagzerointeractive/ZVM-SPEC). 

**Usage** 

    ZSM [YourZSMFile].zsm -o [YourBinaryName].zx
   
   
   **Assembler flags (so far)**

    
|-d|  Generates Debug Symbols at the end of the File|
|--|--|


**Build Info**

This Project does not have any dependancies. 
Alls you need is a C++ Compiler. 

***Mac OS - Terminal Build***

    make macos
    sudo make install
 
***GNU Linux - Terminal Build***

    make linux
    sudo make install

***Windows -  ([w64devkit](https://github.com/skeeto/w64devkit))***

    make windows 
 
**Instructions**

## Instruction Set

  

### General I/O Instructions

  

| Mnemonic | Operands | Opcode | Description |

|----------|----------|--------|-------------|

| `ldr` | reg, value | 40 | Load immediate value into register |

| `lda` | bank, reg, address | 12 | Load value from address into register |

| `ldar` | bank, reg, reg(address) | 19 | Load value from address (in register) into register |

| `sta` | bank, reg, address | 13 | Store register value to address |

| `star` | bank, reg, reg(address) | 20 | Store register value to address (in register) |

| `cpr` | reg1, reg2 | 23 | Copy reg2 to reg1 |

| `swapr` | reg1, reg2 | 27 | Swap values of reg1 and reg2 |

| `fconv` | reg | 28 | Convert register from int to float |

| `iconv` | reg | 29 | Convert register from float to int |

  

---

  

### Stack Manipulation Instructions

  

| Mnemonic | Operands | Opcode | Description |

|----------|----------|--------|-------------|

| `pushr` | reg | 14 | Push register value onto stack |

| `pullr` | reg | 15 | Pull value from stack into register |

| `pop` | - | 16 | Pop top value from stack |

  

---

  

### Integer Math Instructions

  

| Mnemonic | Operands | Opcode | Description |

|----------|----------|--------|-------------|

| `iaddr` | reg1, reg2 | 17 | Add: reg1 = reg1 + reg2 |

| `isubr` | reg1, reg2 | 18 | Subtract: reg1 = reg1 - reg2 |

| `imulr` | reg1, reg2 | 57 | Multiply: reg1 = reg1 * reg2 |

| `idivr` | reg1, reg2 | 21 | Divide: reg1 = reg1 / reg2 |

  

---

  

### Float Math Instructions

  

| Mnemonic | Operands | Opcode | Description |

|----------|----------|--------|-------------|

| `faddr` | reg1, reg2 | 22 | Add: reg1 = reg1 + reg2 (float) |

| `fsubr` | reg1, reg2 | 24 | Subtract: reg1 = reg1 - reg2 (float) |

| `fmulr` | reg1, reg2 | 25 | Multiply: reg1 = reg1 * reg2 (float) |

| `fdivr` | reg1, reg2 | 26 | Divide: reg1 = reg1 / reg2 (float) |

  

---

  

### Integer Vector Math Instructions

  

| Mnemonic | Operands | Opcode | Description |

|----------|----------|--------|-------------|

| `ivaddr` | r1(x), r2(y), r3(z), r4(x), r5(y), r6(z) | 30 | Vector add: v1 = v1 + v2 (int) |

| `ivsubr` | r1(x), r2(y), r3(z), r4(x), r5(y), r6(z) | 31 | Vector subtract: v1 = v1 - v2 (int) |

| `ivmulr` | r1(x), r2(y), r3(z), r4(x), r5(y), r6(z) | 32 | Vector multiply: v1 = v1 * v2 (int) |

| `ivdivr` | r1(x), r2(y), r3(z), r4(x), r5(y), r6(z) | 33 | Vector divide: v1 = v1 / v2 (int) |

  

---

  

### Float Vector Math Instructions

  

| Mnemonic | Operands | Opcode | Description |

|----------|----------|--------|-------------|

| `fvaddr` | r1(x), r2(y), r3(z), r4(x), r5(y), r6(z) | 34 | Vector add: v1 = v1 + v2 (float) |

| `fvsubr` | r1(x), r2(y), r3(z), r4(x), r5(y), r6(z) | 35 | Vector subtract: v1 = v1 - v2 (float) |

| `fvmulr` | r1(x), r2(y), r3(z), r4(x), r5(y), r6(z) | 36 | Vector multiply: v1 = v1 * v2 (float) |

| `fvdivr` | r1(x), r2(y), r3(z), r4(x), r5(y), r6(z) | 37 | Vector divide: v1 = v1 / v2 (float) |

  

---

  

### Branching Instructions

  

| Mnemonic | Operands | Opcode | Description |

|----------|----------|--------|-------------|

| `jmp` | address | 38 | Unconditional jump to address |

| `jmpr` | reg(address) | 39 | Jump to address stored in register |

| `ret` | - | 41 | Return from subroutine |

| `call` | address | 42 | Call function at address |

| `cmpr` | reg1, reg2 | 43 | Compare two registers |

  

---

  

### Conditional Branch Instructions

  

| Mnemonic | Operands | Opcode | Description |

|----------|----------|--------|-------------|

| `bie` | address | 44 | Branch if equal |

| `bin` | address | 45 | Branch if not equal |

| `big` | address | 46 | Branch if greater |

| `bil` | address | 47 | Branch if lesser |

| `bige` | address | 48 | Branch if greater than or equal |

| `bile` | address | 49 | Branch if lesser or equal |

  

---

  

### Bitwise Instructions

  

| Mnemonic | Operands | Opcode | Description |

|----------|----------|--------|-------------|

| `andr` | reg1, reg2, reg3 | 50 | Bitwise AND: reg1 = reg2 & reg3 |

| `orr` | reg1, reg2, reg3 | 51 | Bitwise OR: reg1 = reg2 \| reg3 |

| `xorr` | reg1, reg2, reg3 | 52 | Bitwise XOR: reg1 = reg2 ^ reg3 |

| `notr` | reg1, reg2 | 53 | Bitwise NOT: reg1 = ~reg2 |

  

---

  

### Bit Shift Instructions

  

| Mnemonic | Operands | Opcode | Description |

|----------|----------|--------|-------------|

| `shlr` | reg1, reg2, reg3 | 54 | Shift left: reg1 = reg2 << reg3 |

| `shrr` | reg1, reg2, reg3 | 55 | Shift right: reg1 = reg2 >> reg3 |

| `sarr` | reg1, reg2, reg3 | 56 | Shift right arithmetic: reg1 = (int64_t)reg2 >> reg3 |

  

---

## Memory Map

  

### TRAM Memory Map

  

| Address Range | Purpose |

|---------------|---------|

| `0x00000000 - 0x00000064` | PVM Control Space |

| `0x00000064 - 0x00002864` | Variable Space |

| `0x00002864 - 0x01000000` | Free Memory (Program Start) |

  

### Special TRAM Addresses

  

| Address | Purpose |

|---------|---------|

| `0x00000001` | Program Exit Flag |

| `0x00000002` | Generate Random Number Flag |

| `0x00000003` | Random Number Min (int) |

| `0x00000004` | Random Number Max (int) |

| `0x00000005` | Random Number Result (int) |

| `0x00000006` | Stack Pointer |

  

---
