#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "assembler.h"

// Instruction opcode definitions
enum Opcode {
    JMP  = 0b0000,    // 0000 Jump to specified address
    JRP  = 0b1000,    // 1000 Relative jump from current position
    LDN  = 0b0100,    // 0100 Load negative value from memory
    STO  = 0b1100,    // 1100 Store accumulator value to memory
    SUB  = 0b0010,    // 0010 Subtract value from accumulator
    SUB2 = 0b1010,    // 1010 Alternative subtraction
    CMP  = 0b0110,    // 0110 Compare values
    STP  = 0b1110,    // 1110 Stop program execution
    ADD  = 0b0001,    // 0001 Add value to accumulator
    MUL  = 0b1001,    // 1001 Multiply accumulator by value
    DIV  = 0b0101,    // 0101 Divide accumulator by value
    AND  = 0b1101,    // 1101 Bitwise AND operation
    OR   = 0b0011,    // 0011 Bitwise OR operation
    XOR  = 0b1011,    // 1011 Bitwise XOR operation
    SHL  = 0b0111,    // 0111 Shift left operation
    SHR  = 0b1111     // 1111 Shift right operation
};

// Display program usage information
void printUsage(const char *programName) {
    printf("Usage: %s <input file> <output file> [-q]\n", programName);
    printf("Options:\n");
    printf("  -q    Quiet mode (no verbose output)\n");
}

// Initialize assembler state
void initAssembler(AssemblerState *state, const char *inputFile, const char *outputFile) {
    state->symbolTable.count = 0;
    state->inputFileName = strdup(inputFile);
    state->outputFileName = strdup(outputFile);
    state->verbose = true;
}

// Add symbol to symbol table
int addSymbol(SymbolTable *table, const char *name, int address) {
    if (table->count >= MAX_SYMBOLS) {
        printf("Error: Symbol table is full\n");
        return -1;
    }
    
    // Check if symbol already exists
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            printf("Error: Symbol '%s' already defined\n", name);
            return -1;
        }
    }
    
    strcpy(table->symbols[table->count].name, name);
    table->symbols[table->count].address = address;
    table->count++;
    return 0;
}

// Find symbol in symbol table and return its address
int findSymbol(SymbolTable *table, const char *name) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return table->symbols[i].address;
        }
    }
    return -1;
}

// Parse instruction and convert to machine code
uint32_t parseInstruction(char* line, SymbolTable* table) {
    char* token;
    char* opcode;
    char* operand;
    uint32_t instruction = 0;

    // Skip whitespace and handle comments
    while (isspace(*line)) line++;
    if (*line == ';' || *line == '\0') {
        return 0;
    }
    
    // Handle labels
    token = strchr(line, ':');
    if (token) {
        *token = '\0';
        token++;
        while (isspace(*token)) token++;
    } else {
        token = line;
    }
    
    // Get opcode
    opcode = strtok(token, " \t\n");
    if (!opcode) return 0;
    
    // Get operand
    operand = strtok(NULL, ";");
    if (operand) {
        while (isspace(*operand)) operand++;
        char *end = operand + strlen(operand) - 1;
        while (end > operand && isspace(*end)) *end-- = '\0';
    }
    
    // Handle VAR instruction
    if (strcmp(opcode, "VAR") == 0) {
        if (operand) {
            uint32_t value = (uint32_t)atoi(operand);
            // Place bits from left to right, leftmost is 2^0
            uint32_t result = 0;
            for (int i = 0; i < 32; i++) {
                if (value & (1 << i)) {
                    result |= (1UL << (32 - 1 - i));
                }
            }
            return result;
        }
        return 0;
    }
    
    // Parse opcode (4 bits)
    int op = -1;
    if (strcmp(opcode, "JMP") == 0) op = 0b0000;
    else if (strcmp(opcode, "JRP") == 0) op = 0b1000;
    else if (strcmp(opcode, "LDN") == 0) op = 0b0100;
    else if (strcmp(opcode, "STO") == 0) op = 0b1100;
    else if (strcmp(opcode, "SUB") == 0) op = 0b0010;
    else if (strcmp(opcode, "SUB2") == 0) op = 0b1010;
    else if (strcmp(opcode, "CMP") == 0) op = 0b0110;
    else if (strcmp(opcode, "STP") == 0) {
        // STP instruction handling
        instruction = 0;  // Clear
        instruction |= ((uint32_t)0b1110 << (32 - 17));  // Place opcode 1110 in bits 14-17
        return instruction;
    }
    else if (strcmp(opcode, "ADD") == 0) op = 0b0001;
    else if (strcmp(opcode, "MUL") == 0) op = 0b1001;
    else if (strcmp(opcode, "DIV") == 0) op = 0b0101;
    else if (strcmp(opcode, "AND") == 0) op = 0b1101;
    else if (strcmp(opcode, "OR") == 0) op = 0b0011;
    else if (strcmp(opcode, "XOR") == 0) op = 0b1011;
    else if (strcmp(opcode, "SHL") == 0) op = 0b0111;
    else if (strcmp(opcode, "SHR") == 0) op = 0b1111;

    if (op == -1) {
        printf("Error: Unknown opcode '%s'\n", opcode);
        return 0;
    }

    // Build 32-bit instruction
    instruction = 0;
    
    // 1-13 bits are address (from left to right, leftmost is 2^0)
    int addr = 0;
    if (operand) {
        if (isdigit(*operand)) {
            addr = atoi(operand);
        } else {
            addr = findSymbol(table, operand);
            if (addr == -1) {
                printf("Error: Undefined symbol '%s'\n", operand);
                return 0;
            }
        }
    }
    
    // Place address (1-13 bits)
    for (int i = 0; i < 13; i++) {
        if (addr & (1 << i)) {
            instruction |= (1UL << (32 - 1 - i));
        }
    }
    
    // 14-17 bits are opcode
    instruction |= ((uint32_t)op << (32 - 17));  // Place opcode in bits 14-17
    
    return instruction;
}

// First pass: collect all labels and their addresses
int firstPass(AssemblerState *state) {
    FILE *fp = fopen(state->inputFileName, "r");
    if (!fp) {
        printf("Error: Unable to open input file '%s'\n", state->inputFileName);
        return -1;
    }
    
    char line[MAX_LINE_LENGTH];
    int address = 0;
    
    while (fgets(line, sizeof(line), fp) && address < MEMORY_SIZE) {
        // Skip empty lines and pure comment lines
        char *p = line;
        while (isspace(*p)) p++;
        if (*p == ';' || *p == '\0') {
            continue;
        }
        
        // Find comment and remove
        char *comment = strchr(line, ';');
        if (comment) {
            *comment = '\0';
        }
        
        // Find label
        char *colon = strchr(line, ':');
        if (colon) {
            *colon = '\0';  // Temporarily cut the string
            
            // Clean up whitespace in the label
            char *start = line;
            while (isspace(*start)) start++;
            char *end = start + strlen(start) - 1;
            while (end > start && isspace(*end)) *end-- = '\0';
            
            // Only add to symbol table if the label is not empty and not a VAR instruction
            if (*start && strcmp(start, "VAR") != 0) {
                if (addSymbol(&state->symbolTable, start, address) < 0) {
                    fclose(fp);
                    return -1;
                }
                if (state->verbose) {
                    printf("Found label '%s' at address %d\n", start, address);
                }
            }
        }
        address++;
    }
    
    fclose(fp);
    return 0;
}

// Second pass: generate machine code
int secondPass(AssemblerState *state) {
    FILE *inFp = fopen(state->inputFileName, "r");
    FILE *outFp = fopen(state->outputFileName, "w");
    
    if (!inFp || !outFp) {
        printf("Error: Unable to open file\n");
        if (inFp) fclose(inFp);
        if (outFp) fclose(outFp);
        return -1;
    }
    
    char line[MAX_LINE_LENGTH];
    int lineNum = 0;
    
    while (fgets(line, sizeof(line), inFp)) {
        char *p = line;
        while (isspace(*p)) p++;
        if (*p == ';' || *p == '\0') continue;
        
        uint32_t instruction = parseInstruction(line, &state->symbolTable);
        
        // Output 32-bit from left to right
        for (int i = 31; i >= 0; i--) {
            fprintf(outFp, "%d", (instruction >> i) & 1);
        }
        fprintf(outFp, "\n");
        
        if (state->verbose) {
            printf("Line %2d: ", lineNum + 1);
            for (int i = 31; i >= 0; i--) {
                printf("%d", (instruction >> i) & 1);
            }
            printf("\n");
        }
        lineNum++;
    }
    
    fclose(inFp);
    fclose(outFp);
    return 0;
}

// Main assembly function
int assemble(const char *inputFile, const char *outputFile, bool verbose) {
    AssemblerState state;
    initAssembler(&state, inputFile, outputFile);
    state.verbose = verbose;
    
    printf("Starting assembly...\n");
    
    if (firstPass(&state) < 0) {
        printf("First pass failed\n");
        return -1;
    }
    
    if (secondPass(&state) < 0) {
        printf("Second pass failed\n");
        return -1;
    }
    
    printf("Assembly completed\n");
    return 0;
}

// Check if file exists
int fileExists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Check if file is a valid assembly file
int is_assembly_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return 0;

    char line[MAX_LINE_LENGTH];
    int valid = 0;
    
    // Read the first non-empty line
    while (fgets(line, sizeof(line), file)) {
        char* p = line;
        while (isspace(*p)) p++;
        if (*p == '\0' || *p == ';') continue;
        
        // Check if it contains typical assembly language features (labels, instructions, etc.)
        if (strstr(line, ":") ||          // Contains labels
            strstr(line, "VAR") ||        // VAR instruction
            strstr(line, "LDN") ||        // Other instructions
            strstr(line, "ADD") ||
            strstr(line, "SUB") ||
            strstr(line, "MUL") ||
            strstr(line, "STP")) {
            valid = 1;
        }
        break;
    }
    
    fclose(file);
    return valid;
}

// Main function
int main(int argc, char* argv[]) {
    bool verbose = true;
    char inputFileName[256];
    char outputFileName[256];

    // Handle input file
    while (1) {
        printf("Enter the name/path of the file you want to convert: ");
        if (scanf("%255s", inputFileName) != 1) {
            printf("Input error, please try again\n");
            while (getchar() != '\n');
            continue;
        }

        if (!fileExists(inputFileName)) {
            printf("Error: File '%s' does not exist, please re-enter\n", inputFileName);
            continue;
        }

        if (!is_assembly_file(inputFileName)) {
            printf("Error: File '%s' is not a valid assembly file\n", inputFileName);
            continue;
        }

        break;
    }

    // Handle output file
    while (1) {
        printf("Please enter the file name for the machine code output: ");
        if (scanf("%255s", outputFileName) != 1) {
            printf("Input error, please try again\n");
            while (getchar() != '\n');
            continue;
        }

        // Check if output file already exists
        if (fileExists(outputFileName)) {
            printf("Error: File '%s' already exists, please enter a new filename\n", outputFileName);
            continue;
        }

        break;
    }

    printf("The name of the file to be converted: %s\n", inputFileName);
    printf("File name converted to machine code: %s\n", outputFileName);

    return assemble(inputFileName, outputFileName, verbose);
}