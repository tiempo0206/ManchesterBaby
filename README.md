# 🖥️ Assembly Language Simulator

## 📋 Project Overview

This project consists of an **assembler** and **simulator** for a custom assembly language. The system supports both base instructions and extended instructions with comprehensive error recognition capabilities.

## 🗂️ File Structure

### project_folder/ ├── assembler.h 
### 🔧 Assembler header file ├── assembler.c 
### 🔧 Assembler implementation ├── simulator.c 
### 🎮 Simulator implementation ├── input1.txt 
### 📝 Example assembly code (provided) └── output1.txt 
### 📊 Example machine code output (provided)

## 🚀 Installation & Setup

### Prerequisites
- **Linux system** 🐧
- **GCC compiler** ⚙️
- Basic terminal knowledge

### Step-by-Step Setup

1.  **Prepare the Environment** 📁
    ```bash
    # Ensure all files are in the same directory
    ls -la
    # Should show: assembler.h, assembler.c, simulator.c, input1.txt
    ```
2.  **Compile the Assembler** 🔨
    ```bash
    gcc assembler.c -o assembler
    ```
3.  **Run the Assembler** ▶️
    ```bash
    ./assembler
    ```
    * The program will prompt you to enter the assembly file name or path.
    * You can use files from outside the current folder by providing the full path.
    * Example input: `input1.txt`

4.  **Run the Simulator** 🎮
    ```bash
    gcc simulator.c -o simulator
    ```
    ```bash
    ./simulator
    ```
    * The program will guide you to enter the machine code file name.

## 💡 Features

✅ **Error Recognition**
Both the assembler and simulator include comprehensive error checking:
* Syntax validation ✅
* Instruction format verification 🔍
* Runtime error detection 🚨

📚 **Supported Instructions**

**Base Instructions**
The system supports all standard base instructions of the assembly language.

**Extended Instructions** 🔥

| Instruction | Opcode  | Decimal | Function                                                        |
| :---------- | :------ | :------ | :-------------------------------------------------------------- |
| ADD         | 0b0001  | 8       | ➕ Performs addition on two operands and stores the result      |
| MUL         | 0b1001  | 9       | ✖️ Multiplies two operands and stores the result                |
| DIV         | 0b0101  | 10      | ➗ Divides two operands and stores the quotient                  |
| AND         | 0b1101  | 11      | 🔗 Performs bitwise AND on two operands and stores the result   |
| OR          | 0b0011  | 12      | 🔌 Performs bitwise OR on two operands and stores the result    |
| XOR         | 0b1011  | 13      | ⚡ Performs bitwise XOR on two operands and stores the result   |
| SHL         | 0b0111  | 14      | ⬅️ Performs arithmetic left shift (multiply by 2)               |
| SHR         | 0b1111  | 15      | ➡️ Performs arithmetic right shift (divide by 2)                 |

## 🎯 Usage Examples

**Assembly File Example (`input1.txt`)**
```assembly
; Example assembly code demonstrating all functionality
START:  ADD R1, R2, R3    ; R1 = R2 + R3
        MUL R4, R1, #2    ; R4 = R1 * 2
        DIV R5, R4, R2    ; R5 = R4 / R2
        AND R6, R1, R3    ; R6 = R1 & R3
        ; ... more instructions



