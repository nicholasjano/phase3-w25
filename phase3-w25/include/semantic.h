/* semantic.h */
#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"

// Define semantic error types
typedef enum {
    SEM_ERROR_NONE,
    SEM_ERROR_UNDECLARED_VARIABLE,
    SEM_ERROR_REDECLARED_VARIABLE,
    SEM_ERROR_TYPE_MISMATCH,
    SEM_ERROR_UNINITIALIZED_VARIABLE,
    SEM_ERROR_INVALID_OPERATION,
    SEM_ERROR_SEMANTIC_ERROR
} SemanticErrorType;

// Symbol structure for symbol table
typedef struct Symbol {
    char name[100];          // Variable name
    int type;                // Data type (using TokenType for types)
    int scope_level;         // Scope nesting level
    int line_declared;       // Line where declared
    int is_initialized;      // Has been assigned a value?
    struct Symbol* next;     // For linked list implementation
} Symbol;

// Symbol table structure
typedef struct {
    Symbol* head;            // First symbol in the table
    int current_scope;       // Current scope level
} SymbolTable;

// Symbol table functions
SymbolTable* init_symbol_table();
void add_symbol(SymbolTable* table, const char* name, int type, int line);
Symbol* lookup_symbol(SymbolTable* table, const char* name);
Symbol* lookup_symbol_current_scope(SymbolTable* table, const char* name);
void enter_scope(SymbolTable* table);
void exit_scope(SymbolTable* table);
void remove_symbols_in_current_scope(SymbolTable* table);
void free_symbol_table(SymbolTable* table);
void print_symbol_table(SymbolTable* table);

// Semantic analysis functions
int analyze_semantics(ASTNode* ast);
void semantic_error(SemanticErrorType error, const char* name, int line);
void proc_semantic_file(const char* filename);

// Helper functions for semantic analysis
int check_program(ASTNode* node, SymbolTable* table);
int check_statement(ASTNode* node, SymbolTable* table);
int check_declaration(ASTNode* node, SymbolTable* table);
int check_assignment(ASTNode* node, SymbolTable* table);
int check_expression(ASTNode* node, SymbolTable* table, int* result_type);
int check_block(ASTNode* node, SymbolTable* table);
int check_condition(ASTNode* node, SymbolTable* table);
int check_if_statement(ASTNode* node, SymbolTable* table);
int check_while_statement(ASTNode* node, SymbolTable* table);
int check_repeat_until_statement(ASTNode* node, SymbolTable* table);
int check_function_declaration(ASTNode* node, SymbolTable* table);
int check_return_statement(ASTNode* node, SymbolTable* table);
int check_factorial(ASTNode* node, SymbolTable* table);

#endif /* SEMANTIC_H */