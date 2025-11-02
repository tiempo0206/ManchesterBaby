#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORD_SIZE 32
#ifndef MEMORY_SIZE
#define MEMORY_SIZE 32  // Default memory size
#endif

// Extended instruction set
typedef enum {
    // Basic instructions (3-bit opcode -> 4-bit opcode, leftmost is the least significant bit)
    JMP = 0b0000,    // 0000 = 0, Jump to specified address
    JRP = 0b1000,    // 1000 = 1, Relative jump from current position
    LDN = 0b0100,    // 0100 = 2, Load negative value from memory
    STO = 0b1100,    // 1100 = 3, Store accumulator value to memory
    SUB = 0b0010,    // 0010 = 4, Subtract value from accumulator
    SUB2 = 0b1010,   // 1010 = 5, Alternative subtraction
    CMP = 0b0110,    // 0110 = 6, Compare values
    STP = 0b1110,    // 1110 = 7, Stop program execution
    // Extended arithmetic and bitwise operations
    ADD = 0b0001,    // 0001 = 8, Add value to accumulator
    MUL = 0b1001,    // 1001 = 9, Multiply accumulator by value
    DIV = 0b0101,    // 0101 = 10, Divide accumulator by value
    AND = 0b1101,    // 1101 = 11, Bitwise AND operation
    OR  = 0b0011,    // 0011 = 12, Bitwise OR operation
    XOR = 0b1011,    // 1011 = 13, Bitwise XOR operation
    SHL = 0b0111,    // 0111 = 14, Shift left operation
    SHR = 0b1111     // 1111 = 15, Shift right operation
} OpCode;

// Extended addressing mode
typedef enum {
    DIRECT = 0,     // Direct addressing
    INDIRECT = 1,   // Indirect addressing
    IMMEDIATE = 2,  // Immediate addressing
    RELATIVE = 3    // Relative addressing
} AddressingMode;

// Hardware components simulation
typedef struct {
    int** store;                // Dynamic memory array
    int memory_size;            // Current memory size configuration
    int accumulator;            // Accumulator register
    int CI;                     // Control Instruction (Program Counter)
    int PI;                     // Present Instruction register
    int running;                // Program execution state
    AddressingMode addr_mode;   // Current addressing mode
    int index_reg;              // Index register for address calculation
    int base_reg;               // Base register for address calculation
} BabyComputer;

// Function declarations
void initialize_computer(BabyComputer* computer, int memory_size);
int load_program(BabyComputer* computer, const char* filename);
void fetch(BabyComputer* computer);
void decode(BabyComputer* computer, int* opcode, int* operand);
void execute(BabyComputer* computer, int opcode, int operand);
void print_state(BabyComputer* computer);
int convert_to_decimal(int binary[], int size);
void convert_to_binary(int decimal, int binary[], int size);
void print_binary(int value, int width);
int get_effective_address(BabyComputer* computer, int operand);
int get_value_from_address(BabyComputer* computer, int address);
void store_value_to_address(BabyComputer* computer, int address, int value);

int main() {
    BabyComputer computer;
    char filename[100];
    int opcode, operand;
    int step = 0;
    int memory_size = 32;  // Default memory size
    char input[10];        // Used to receive user input

    // Display welcome menu
    printf("\n=== Welcome to the Manchester Baby ===\n");
    while (1) {
        printf("\n1. Start\n");
        printf("2. Exit\n");
        printf("\nPlease select (1-2): ");
        
        // Read user input
        if (scanf("%s", input) != 1) {
            printf("Input error, please try again\n");
            while (getchar() != '\n');
            continue;
        }

        // Check input length
        if (strlen(input) != 1) {
            printf("Invalid input, please enter 1 or 2\n");
            continue;
        }

        // Check input content
        if (input[0] == '2') {
            printf("\nThank you for using. Goodbye!\n");
            return 0;
        } else if (input[0] == '1') {
            // Select memory configuration
            while (1) {
                printf("\nPlease select memory configuration:\n");
                printf("1. 32 x 32\n");
                printf("2. 32 x 64\n");
                printf("\nPlease select (1-2): ");
                
                if (scanf("%s", input) != 1) {
                    printf("Input error, please try again\n");
                    while (getchar() != '\n');
                    continue;
                }

                if (strlen(input) != 1) {
                    printf("Invalid input, please enter 1 or 2\n");
                    continue;
                }

                if (input[0] == '1' || input[0] == '2') {
                    memory_size = (input[0] == '2') ? 64 : 32;
                    printf("Selected 32 x %d memory configuration\n", memory_size);
                    break;
                } else {
                    printf("Invalid selection, please enter 1 or 2\n");
                }
            }
            break;
        } else {
            printf("Invalid selection, please enter 1 or 2\n");
        }
    }

    // Initialize computer
    initialize_computer(&computer, memory_size);

    // Load program
    while (1) {
        printf("\nPlease enter program filename: ");
        if (scanf("%99s", filename) != 1) {
            printf("Input error, please try again\n");
            while (getchar() != '\n');
            continue;
        }

        if (load_program(&computer, filename) == 0) {
            break;
        }
        printf("Program loading failed, please try again\n");
    }

    // Select running mode
    while (1) {
        printf("\nPlease select running mode:\n");
        printf("1. Step by step (Press Enter to continue)\n");
        printf("2. Continuous run (Until STP instruction)\n");
        printf("\nPlease select (1-2): ");
        
        if (scanf("%s", input) != 1) {
            printf("Input error, please try again\n");
            while (getchar() != '\n');
            continue;
        }

        // Check input length
        if (strlen(input) != 1) {
            printf("Invalid input, please enter 1 or 2\n");
            continue;
        }

        // Check input content
        if (input[0] == '1' || input[0] == '2') {
            int run_mode = input[0] - '0';
            getchar();  // Consume the newline character from the input buffer
            
            // Run program
            printf("\n=== Program Execution Started ===\n");
            while (computer.running) {
                printf("\n=== Cycle %d ===\n", step++);
                
                printf("\n--- Fetch Stage ---\n");
                fetch(&computer);
                
                printf("\n--- Decode Stage ---\n");
                decode(&computer, &opcode, &operand);
                
                printf("\n--- Execute Stage ---\n");
                execute(&computer, opcode, operand);
                
                print_state(&computer);
                
                if (run_mode == 1) {
                    printf("\nPress Enter to continue...");
                    getchar();
                }
            }

            printf("\n=== Program Execution Completed ===\n");
            break;
        } else {
            printf("Invalid selection, please enter 1 or 2\n");
        }
    }

    // Release memory
    for (int i = 0; i < computer.memory_size; i++) {
        free(computer.store[i]);
    }
    free(computer.store);

    return 0;
}

// Initialize computer with specified memory size
void initialize_computer(BabyComputer* computer, int memory_size) {
    computer->memory_size = memory_size;
    // Dynamically allocate memory array
    computer->store = (int**)malloc(memory_size * sizeof(int*));
    for (int i = 0; i < memory_size; i++) {
        computer->store[i] = (int*)calloc(WORD_SIZE, sizeof(int));
    }
    computer->accumulator = 0;
    computer->CI = 0;
    computer->PI = 0;
    computer->running = 1;
    computer->addr_mode = DIRECT;
    computer->index_reg = 0;
    computer->base_reg = 0;
}

// Load program from file into memory
int load_program(BabyComputer* computer, const char* filename) {
    // First, check if the file exists
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: File '%s' does not exist\n", filename);
        return -1;
    }
    
    // Then, check if it is a valid machine code file
    char line[WORD_SIZE + 2];  // +2 for newline and string termination
    int valid = 1;
    
    // Check each line for valid machine code format
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;  // Remove newline character
        
        // Check length
        if (strlen(line) != WORD_SIZE) {
            valid = 0;
            break;
        }
        
        // Check if it contains only 0s and 1s
        for (int i = 0; i < WORD_SIZE; i++) {
            if (line[i] != '0' && line[i] != '1') {
                valid = 0;
                break;
            }
        }
        
        if (!valid) break;
    }
    
    // If not a valid machine code file, return error
    if (!valid) {
        printf("Error: File '%s' is not a valid machine code file\n", filename);
        printf("Machine code file should:\n");
        printf("1. Have exactly 32 characters per line\n");
        printf("2. Contain only 0s and 1s\n");
        printf("Please use the assembler to convert assembly code to machine code\n");
        fclose(file);
        return -1;
    }
    
    // Rewind to start of file
    rewind(file);
    
    // Load program into memory
    int address = 0;
    while (fgets(line, sizeof(line), file) && address < computer->memory_size) {
        line[strcspn(line, "\n")] = 0;
        
        // Load instruction
        for (int i = 0; i < WORD_SIZE; i++) {
            computer->store[address][i] = line[i] - '0';
        }
        address++;
    }
    
    printf("Successfully loaded %d instructions\n", address);
    fclose(file);
    return 0;
}

// Fetch instruction from memory
void fetch(BabyComputer* computer) {
    printf("CI = %d\n", computer->CI);
    printf("Current instruction: ");
    for (int i = 0; i < WORD_SIZE; i++) {
        printf("%d", computer->store[computer->CI][i]);
    }
    printf("\n");

    // Load current instruction into PI register
    computer->PI = 0;
    for (int i = 0; i < WORD_SIZE; i++) {
        computer->PI = (computer->PI << 1) | computer->store[computer->CI][i];
    }
}

// Decode instruction
void decode(BabyComputer* computer, int* opcode, int* operand) {
    // Get the 14-17 bits as the opcode (read from left to right, but note the bit order)
    *opcode = (computer->store[computer->CI][13] << 3) |  // 14th bit (least significant bit) -> 3rd bit
              (computer->store[computer->CI][14] << 2) |  // 15th bit -> 2nd bit
              (computer->store[computer->CI][15] << 1) |  // 16th bit -> 1st bit
              (computer->store[computer->CI][16] << 0);   // 17th bit (most significant bit) -> 0th bit
    
    // Get the first 13 bits as the operand (calculate from left to right)
    *operand = 0;
    for (int i = 0; i < 13; i++) {
        if (computer->store[computer->CI][i]) {
            *operand |= (1 << i);  // Set the i-th bit to 1
        }
    }
    
    printf("\n--- Decode Stage ---\n");
    printf("Instruction analysis:\n");
    printf("- Opcode (14-17 bits): %d%d%d%d (%s)\n",
           computer->store[computer->CI][13],
           computer->store[computer->CI][14],
           computer->store[computer->CI][15],
           computer->store[computer->CI][16],
           *opcode == 0b0000 ? "JMP" :   // 0000
           *opcode == 0b1000 ? "JRP" :   // 1000
           *opcode == 0b0100 ? "LDN" :   // 0100
           *opcode == 0b1100 ? "STO" :   // 1100
           *opcode == 0b0010 ? "SUB" :   // 0010
           *opcode == 0b1010 ? "SUB2" :  // 1010
           *opcode == 0b0110 ? "CMP" :   // 0110
           *opcode == 0b1110 ? "STP" :   // 1110
           *opcode == 0b0001 ? "ADD" :   // 0001
           *opcode == 0b1001 ? "MUL" :   // 1001
           *opcode == 0b0101 ? "DIV" :   // 0101
           *opcode == 0b1101 ? "AND" :   // 1101
           *opcode == 0b0011 ? "OR"  :   // 0011
           *opcode == 0b1011 ? "XOR" :   // 1011
           *opcode == 0b0111 ? "SHL" :   // 0111
           *opcode == 0b1111 ? "SHR" :   // 1111
           "Unknown");
    
    printf("- Operand (first 13 bits): ");
    // Print operand in binary (first 13 bits)
    for (int i = 0; i < 13; i++) {
        printf("%d", computer->store[computer->CI][i]);
    }
    printf(" (binary) = %d (decimal)\n", *operand);
}

// Execute instruction
void execute(BabyComputer* computer, int opcode, int operand) {
    if (computer->CI == 0) {
        printf("Skip initialization instruction, move to the next instruction\n");
        computer->CI = 1;
        return;
    }

    int address = operand;  // Directly use the operand as the address

    switch (opcode) {
        case 0b0000: {  // JMP (0000 = 0)
            printf("Executing: JMP - Jump to address %d\n", address);
            computer->CI = address;
        } break;
        
        case 0b1000: {  // JRP (1000 = 1)
            printf("Executing: JRP - Relative jump, current position %d plus offset %d\n", 
                   computer->CI, address);
            computer->CI += address;
        } break;
        
        case 0b0100: {  // LDN (0100 = 2)
            printf("Executing: LDN - Load from address %d ", address);
            int value = get_value_from_address(computer, address);
            computer->accumulator = -value;
            printf("negative value to the accumulator: ");
            print_binary(computer->accumulator, WORD_SIZE);
            printf(" (%d)\n", computer->accumulator);
            computer->CI++;
        } break;
        
        case 0b1100: {  // STO (1100 = 3)
            printf("Executing: STO - Store accumulator value ");
            print_binary(computer->accumulator, WORD_SIZE);
            printf(" (%d) to address %d\n", computer->accumulator, address);
            store_value_to_address(computer, address, computer->accumulator);
            computer->CI++;
        } break;
        
        case 0b0010:    // SUB (0010 = 4)
        case 0b1010: {  // SUB2 (1010 = 5)
            int value = get_value_from_address(computer, address);
            int old_acc = computer->accumulator;
            computer->accumulator -= value;
            printf("Executing: SUB - Subtract from accumulator ");
            print_binary(old_acc, WORD_SIZE);
            printf(" (%d) the value at address %d ", old_acc, address);
            print_binary(value, WORD_SIZE);
            printf(" (%d), result: ", value);
            print_binary(computer->accumulator, WORD_SIZE);
            printf(" (%d)\n", computer->accumulator);
            printf("Executing: SUB - Calculation: %d - %d = %d\n", 
                   old_acc, value, computer->accumulator);
            computer->CI++;
        } break;
        
        case 0b0110: {  // CMP (0110 = 6)
            int value = get_value_from_address(computer, address);
            printf("Executing: CMP - Compare accumulator ");
            print_binary(computer->accumulator, WORD_SIZE);
            printf(" (%d) with the value at address %d ", computer->accumulator, address);
            print_binary(value, WORD_SIZE);
            printf(" (%d)\n", value);
            computer->CI++;
        } break;
        
        case 0b1110: {  // STP (1110 = 7)
            printf("Executing: STP - Program stop\n");
            computer->running = 0;
        } break;

        case 0b0001: {  // ADD
            int value = get_value_from_address(computer, address);
            int old_acc = computer->accumulator;
            computer->accumulator += value;
            printf("Executing: ADD - Calculation: %d + %d = %d\n", 
                   old_acc, value, computer->accumulator);
            computer->CI++;
        } break;
        
        case 0b1001: {  // MUL
            int value = get_value_from_address(computer, address);
            int old_acc = computer->accumulator;
            computer->accumulator *= value;
            printf("Executing: MUL - Calculation: %d * %d = %d\n", 
                   old_acc, value, computer->accumulator);
            computer->CI++;
        } break;
        
        case 0b0101: {  // DIV
            int value = get_value_from_address(computer, address);
            if (value != 0) {
                int old_acc = computer->accumulator;
                computer->accumulator /= value;
                printf("Executing: DIV - Calculation: %d / %d = %d\n", 
                       old_acc, value, computer->accumulator);
            } else {
                printf("Error: Division by zero\n");
            }
            computer->CI++;
        } break;
        
        case 0b1101: {  // AND
            int value = get_value_from_address(computer, address);
            int old_acc = computer->accumulator;
            computer->accumulator &= value;
            printf("Executing: AND - Calculation: %d & %d = %d\n", 
                   old_acc, value, computer->accumulator);
            computer->CI++;
        } break;
        
        case 0b0011: {  // OR
            int value = get_value_from_address(computer, address);
            int old_acc = computer->accumulator;
            computer->accumulator |= value;
            printf("Executing: OR - Calculation: %d | %d = %d\n", 
                   old_acc, value, computer->accumulator);
            computer->CI++;
        } break;
        
        case 0b1011: {  // XOR
            int value = get_value_from_address(computer, address);
            int old_acc = computer->accumulator;
            computer->accumulator ^= value;
            printf("Executing: XOR - Calculation: %d ^ %d = %d\n", 
                   old_acc, value, computer->accumulator);
            computer->CI++;
        } break;
        
        case 0b0111: {  // SHL
            int value = get_value_from_address(computer, address);
            int old_acc = computer->accumulator;
            computer->accumulator <<= value;
            printf("Executing: SHL - Calculation: %d << %d = %d\n", 
                   old_acc, value, computer->accumulator);
            computer->CI++;
        } break;
        
        case 0b1111: {  // SHR
            int value = get_value_from_address(computer, address);
            int old_acc = computer->accumulator;
            computer->accumulator >>= value;
            printf("Executing: SHR - Calculation: %d >> %d = %d\n", 
                   old_acc, value, computer->accumulator);
            computer->CI++;
        } break;
        
        default:
            printf("Unknown instruction: %d\n", opcode);
            computer->CI++;
            break;
    }

    printf("Post-execution state: CI=%d, A=", computer->CI);
    print_binary(computer->accumulator, WORD_SIZE);
    printf(" (%d)\n", computer->accumulator);
}

// Print computer state
void print_state(BabyComputer* computer) {
    printf("\n=== Computer State ===\n");
    printf("Program Counter (CI): %d\n", computer->CI);
    printf("Present Instruction (PI): ");
    print_binary(computer->PI, WORD_SIZE);
    printf(" (%d)\n", computer->PI);
    printf("Accumulator (A): ");
    print_binary(computer->accumulator, WORD_SIZE);
    printf(" (%d)\n", computer->accumulator);
    
    printf("\nMemory Contents:\n");
    for (int i = 0; i < computer->memory_size; i++) {
        printf("%2d: ", i);
        // Print binary first
        for (int j = 0; j < WORD_SIZE; j++) {
            printf("%d", computer->store[i][j]);
        }
        
        // Read from left to right, the leftmost is the least significant bit
        int value = 0;
        for (int j = 0; j < WORD_SIZE; j++) {
            if (computer->store[i][j]) {
                value |= (1 << j);  // Set the j-th bit to 1
            }
        }
        printf(" (%d)\n", value);
    }
}

// Binary to decimal
int convert_to_decimal(int binary[], int size) {
    int decimal = 0;
    for (int i = 0; i < size; i++) {
        decimal = (decimal << 1) | binary[i];
    }
    return decimal;
}

// Decimal to binary
void convert_to_binary(int decimal, int binary[], int size) {
    // Store from left to right, the least significant bit on the left
    for (int i = 0; i < size; i++) {
        binary[i] = decimal & 1;
        decimal >>= 1;
    }
}

// Helper function: Print binary number
void print_binary(int value, int width) {
    // Display from left to right, the leftmost is the least significant bit
    for (int i = 0; i < width; i++) {
        printf("%d", (value >> i) & 1);
    }
}

// Addressing mode handling function
int get_effective_address(BabyComputer* computer, int operand) {
    switch (computer->addr_mode) {
        case DIRECT:
            return operand % computer->memory_size;
            
        case INDIRECT:
            {
                int indirect_addr = operand % computer->memory_size;
                int final_addr = 0;
                for (int i = 0; i < WORD_SIZE; i++) {
                    final_addr = (final_addr << 1) | computer->store[indirect_addr][i];
                }
                return final_addr % computer->memory_size;
            }
            
        case IMMEDIATE:
            return operand;  // Immediate addressing directly returns the operand
            
        case RELATIVE:
            return (computer->CI + operand) % computer->memory_size;
            
        default:
            return operand % computer->memory_size;
    }
}

int get_value_from_address(BabyComputer* computer, int address) {
    if (computer->addr_mode == IMMEDIATE) {
        return address;
    }
    
    // Ensure the address is within the valid range
    address = address % computer->memory_size;
    
    // Read from left to right, the leftmost is the least significant bit
    int value = 0;
    for (int j = 0; j < WORD_SIZE; j++) {
        if (computer->store[address][j]) {
            value |= (1 << j);  // Set the j-th bit to 1
        }
    }
    
    printf("Reading value from address %d: ", address);
    for (int i = 0; i < WORD_SIZE; i++) {
        printf("%d", computer->store[address][i]);
    }
    printf(" (%d)\n", value);
    
    // When displaying, also read from left to right
    printf("Loading value ");
    for (int i = 0; i < WORD_SIZE; i++) {
        printf("%d", computer->store[address][i]);
    }
    printf(" (%d)", value);
    
    return value;
}

// Store value to address
void store_value_to_address(BabyComputer* computer, int address, int value) {
    // Store from left to right, consistent with other numbers
    for (int i = 0; i < WORD_SIZE; i++) {
        computer->store[address][i] = value & 1;
        value >>= 1;
    }
}