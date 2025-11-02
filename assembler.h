#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>
#include <stdbool.h>

// Maximum allowed symbols in symbol table
#define MAX_SYMBOLS 100
// Maximum length for a single line of assembly code
#define MAX_LINE_LENGTH 256
// Maximum memory size (supports up to 64 storage units)
#define MEMORY_SIZE 64

// Symbol table entry structure
typedef struct {
    char name[MAX_LINE_LENGTH];  // Symbol name (label)
    int address;                 // Memory address for this symbol
} Symbol;

// Complete symbol table structure
typedef struct {
    Symbol symbols[MAX_SYMBOLS]; // Array of symbol entries
    int count;                  // Current number of symbols
} SymbolTable;

// Assembler state management structure
typedef struct {
    SymbolTable symbolTable;    // Table for storing symbols and their addresses
    char *inputFileName;        // Input assembly file path
    char *outputFileName;       // Output machine code file path
    bool verbose;               // Verbose output flag
} AssemblerState;

// Function declarations
int assemble(const char *inputFile, const char *outputFile, bool verbose);
void initAssembler(AssemblerState *state, const char *inputFile, const char *outputFile);
int firstPass(AssemblerState *state);
int secondPass(AssemblerState *state);
int addSymbol(SymbolTable *table, const char *name, int address);
int findSymbol(SymbolTable *table, const char *name);
uint32_t parseInstruction(char *line, SymbolTable *table);

#endif 