/* semantic.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/semantic.h"
#include "../../include/parser.h"
#include "../../include/tokens.h"
#include "../../include/lexer.h"

// Global variable for semantic analyzer
static int semantic_error_count = 0;

// Symbol Table Management Functions

// Initialize symbol table
SymbolTable* init_symbol_table() {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    if (table) {
        table->head = NULL;
        table->current_scope = 0;
    }
    return table;
}

// Add symbol to table
void add_symbol(SymbolTable* table, const char* name, int type, int line) {
    Symbol* symbol = malloc(sizeof(Symbol));
    if (symbol) {
        strcpy(symbol->name, name);
        symbol->type = type;
        symbol->scope_level = table->current_scope;
        symbol->line_declared = line;
        symbol->is_initialized = 0;
        
        // Add to beginning of list
        symbol->next = table->head;
        table->head = symbol;
    }
}

// Look up symbol by name, most recent scope first
Symbol* lookup_symbol(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    Symbol* found = NULL;
    int max_scope = -1;
    
    // Find the symbol with matching name at the most recent scope
    while (current) {
        if (strcmp(current->name, name) == 0 && current->scope_level <= table->current_scope) {
            if (current->scope_level > max_scope) {
                found = current;
                max_scope = current->scope_level;
            }
        }
        current = current->next;
    }
    
    return found;
}

// Look up symbol in current scope only
Symbol* lookup_symbol_current_scope(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0 && 
            current->scope_level == table->current_scope) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Enter a new scope level
void enter_scope(SymbolTable* table) {
    table->current_scope++;
}

// Exit the current scope level
void exit_scope(SymbolTable* table) {
    if (table->current_scope > 0) {
        table->current_scope--;
    }
}

// Remove symbols from the current scope
void remove_symbols_in_current_scope(SymbolTable* table) {
    Symbol* current = table->head;
    Symbol* prev = NULL;
    
    while (current) {
        if (current->scope_level == table->current_scope) {
            Symbol* to_remove = current;
            
            if (prev) {
                prev->next = current->next;
                current = current->next;
            } else {
                table->head = current->next;
                current = table->head;
            }
            
            free(to_remove);
        } else {
            prev = current;
            current = current->next;
        }
    }
}

// Free the symbol table
void free_symbol_table(SymbolTable* table) {
    Symbol* current = table->head;
    
    while (current) {
        Symbol* next = current->next;
        free(current);
        current = next;
    }
    
    free(table);
}

// Utility Functions

// Helper to count symbols in table
static int get_symbol_count(SymbolTable* table) {
    int count = 0;
    Symbol* current = table->head;
    
    while (current) {
        count++;
        current = current->next;
    }
    
    return count;
}

// Print the symbol table contents for debugging
void print_symbol_table(SymbolTable* table) {
    Symbol* current = table->head;
    
    printf("\n== SYMBOL TABLE DUMP ==\n");
    printf("Total symbols: %d\n\n", get_symbol_count(table));
    
    int i = 0;
    while (current) {
        printf("Symbol[%d]:\n", i++);
        printf("  Name: %s\n", current->name);
        
        // Print type name instead of enum value
        printf("  Type: ");
        switch(current->type) {
            case TOKEN_INT: printf("int"); break;
            case TOKEN_FLOAT_KEY: printf("float"); break;
            case TOKEN_CHAR: printf("char"); break;
            case TOKEN_VOID: printf("void"); break;
            default: printf("unknown(%d)", current->type);
        }
        printf("\n");
        
        printf("  Scope Level: %d\n", current->scope_level);
        printf("  Line Declared: %d\n", current->line_declared);
        printf("  Initialized: %s\n\n", current->is_initialized ? "Yes" : "No");
        
        current = current->next;
    }
    
    printf("===================\n");
}

// Get string representation of a type
const char* type_to_string(int type) {
    switch(type) {
        case TOKEN_INT: return "int";
        case TOKEN_FLOAT_KEY: return "float";
        case TOKEN_CHAR: return "char";
        case TOKEN_VOID: return "void";
        default: return "unknown";
    }
}

// Check if two types are compatible
int are_types_compatible(int type1, int type2) {
    // Same types are always compatible
    if (type1 == type2) {
        return 1;
    }
    
    // Numeric types are compatible with each other
    if ((type1 == TOKEN_INT || type1 == TOKEN_FLOAT_KEY) &&
        (type2 == TOKEN_INT || type2 == TOKEN_FLOAT_KEY)) {
        return 1;
    }
    
    return 0;
}

// Report semantic errors
void semantic_error(SemanticErrorType error, const char* name, int line) {
    semantic_error_count++;
    printf("Semantic Error at line %d: ", line);
    
    switch (error) {
        case SEM_ERROR_UNDECLARED_VARIABLE:
            printf("Undeclared variable '%s'\n", name);
            break;
        case SEM_ERROR_REDECLARED_VARIABLE:
            printf("Variable '%s' already declared in this scope\n", name);
            break;
        case SEM_ERROR_TYPE_MISMATCH:
            printf("Type mismatch involving '%s'\n", name);
            break;
        case SEM_ERROR_UNINITIALIZED_VARIABLE:
            printf("Variable '%s' may be used uninitialized\n", name);
            break;
        case SEM_ERROR_INVALID_OPERATION:
            printf("Invalid operation involving '%s'\n", name);
            break;
        default:
            printf("Unknown semantic error with '%s'\n", name);
    }
}

// Semantic Analysis Functions

// Check semantic correctness of a factorial call
int check_factorial(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != AST_FACTORIAL) {
        return 0;
    }
    
    // Factorial should have one argument
    if (!node->left) {
        semantic_error(SEM_ERROR_INVALID_OPERATION, "factorial", node->token.line);
        return 0;
    }
    
    // Check that the argument is of numeric type
    int arg_type;
    int valid = check_expression(node->left, table, &arg_type);
    
    // Factorial is only valid for integers
    if (valid && arg_type != TOKEN_INT) {
        semantic_error(SEM_ERROR_TYPE_MISMATCH, "factorial", node->token.line);
        return 0;
    }
    
    return valid;
}

// Check semantic correctness of an expression
int check_expression(ASTNode* node, SymbolTable* table, int* result_type) {
    if (!node) {
        *result_type = TOKEN_ERROR;
        return 0;
    }
    
    switch (node->type) {
        case AST_NUMBER:
            // Assume all numeric literals are ints for now
            *result_type = TOKEN_INT;
            return 1;
            
        case AST_STRING:
            // String literals
            *result_type = TOKEN_STRING;
            return 1;
            
        case AST_IDENTIFIER: {
            // Variable reference (check if declared)
            Symbol* symbol = lookup_symbol(table, node->token.lexeme);
            if (!symbol) {
                semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, node->token.lexeme, node->token.line);
                *result_type = TOKEN_ERROR;
                return 0;
            }
            
            // Check if initialized
            if (!symbol->is_initialized) {
                semantic_error(SEM_ERROR_UNINITIALIZED_VARIABLE, node->token.lexeme, node->token.line);
                // Continue with analysis, but mark that there was an error
                *result_type = symbol->type;
                return 0;
            }
            
            *result_type = symbol->type;
            return 1;
        }
            
        case AST_BINOP: {
            // Binary operation (check operand types)
            int left_type, right_type;
            int left_valid = check_expression(node->left, table, &left_type);
            int right_valid = check_expression(node->right, table, &right_type);
            
            if (!left_valid || !right_valid) {
                *result_type = TOKEN_ERROR;
                return 0;
            }
            
            // Check for division by zero in constant expressions
            if (node->token.lexeme[0] == '/' && 
                node->right->type == AST_NUMBER &&
                atoi(node->right->token.lexeme) == 0) {
                semantic_error(SEM_ERROR_INVALID_OPERATION, "division by zero", node->token.line);
                *result_type = TOKEN_ERROR;
                return 0;
            }
            
            // Type compatibility check
            if (!are_types_compatible(left_type, right_type)) {
                char error_msg[200];
                sprintf(error_msg, "incompatible types: %s and %s", 
                        type_to_string(left_type), type_to_string(right_type));
                semantic_error(SEM_ERROR_TYPE_MISMATCH, error_msg, node->token.line);
                *result_type = TOKEN_ERROR;
                return 0;
            }
            
            // For math operators, result type follows operands
            char op = node->token.lexeme[0];
            if (op == '+' || op == '-' || op == '*' || op == '/') {
                // If either operand is float, result is float
                if (left_type == TOKEN_FLOAT_KEY || right_type == TOKEN_FLOAT_KEY) {
                    *result_type = TOKEN_FLOAT_KEY;
                } else {
                    *result_type = TOKEN_INT;
                }
            } 
            // For comparison operators, result is boolean (represented as int)
            else if (op == '>' || op == '<' || 
                     strcmp(node->token.lexeme, "==") == 0 || 
                     strcmp(node->token.lexeme, "!=") == 0 || 
                     strcmp(node->token.lexeme, ">=") == 0 || 
                     strcmp(node->token.lexeme, "<=") == 0 || 
                     strcmp(node->token.lexeme, "&&") == 0 || 
                     strcmp(node->token.lexeme, "||") == 0) {
                *result_type = TOKEN_INT; // Boolean result is int
            }
            
            return 1;
        }
            
        case AST_FACTORIAL:
            // Check factorial argument
            if (check_factorial(node, table)) {
                *result_type = TOKEN_INT; // Factorial result is int
                return 1;
            }
            *result_type = TOKEN_ERROR;
            return 0;
            
        case AST_FUNCTION_CALL: {
            // Function call (look up function in symbol table)
            Symbol* func = lookup_symbol(table, node->token.lexeme);
            if (!func) {
                semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, node->token.lexeme, node->token.line);
                *result_type = TOKEN_ERROR;
                return 0;
            }
            
            *result_type = func->type; // Return type of the function
            return 1;
        }
            
        default:
            *result_type = TOKEN_ERROR;
            return 0;
    }
}

// Check semantic correctness of a declaration
int check_declaration(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != AST_VARDECL) {
        return 0;
    }
    
    const char* var_name = node->token.lexeme;
    
    // Determine variable type based on token
    int var_type = TOKEN_INT; // Default to int
    
    // Check if variable already declared in current scope
    Symbol* existing = lookup_symbol_current_scope(table, var_name);
    if (existing) {
        semantic_error(SEM_ERROR_REDECLARED_VARIABLE, var_name, node->token.line);
        return 0;
    }
    
    // Add to symbol table
    add_symbol(table, var_name, var_type, node->token.line);
    
    // If there's an initialization, check it
    if (node->right) {
        int init_type;
        int valid = check_expression(node->right, table, &init_type);
        
        if (valid) {
            // Check type compatibility
            if (!are_types_compatible(var_type, init_type)) {
                char error_msg[200];
                sprintf(error_msg, "cannot initialize %s with %s", 
                        type_to_string(var_type), type_to_string(init_type));
                semantic_error(SEM_ERROR_TYPE_MISMATCH, error_msg, node->token.line);
                return 0;
            }
            
            // Mark as initialized
            Symbol* symbol = lookup_symbol_current_scope(table, var_name);
            if (symbol) {
                symbol->is_initialized = 1;
            }
        }
        
        return valid;
    }
    
    return 1;
}

// Check semantic correctness of an assignment
int check_assignment(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != AST_ASSIGN || !node->left || !node->right) {
        return 0;
    }
    
    if (node->left->type != AST_IDENTIFIER) {
        semantic_error(SEM_ERROR_INVALID_OPERATION, "assignment target must be a variable", node->token.line);
        return 0;
    }
    
    const char* var_name = node->left->token.lexeme;
    
    // Check if variable exists
    Symbol* symbol = lookup_symbol(table, var_name);
    if (!symbol) {
        semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, var_name, node->token.line);
        return 0;
    }
    
    // Check expression
    int expr_type;
    int expr_valid = check_expression(node->right, table, &expr_type);
    
    if (expr_valid) {
        // Check type compatibility
        if (!are_types_compatible(symbol->type, expr_type)) {
            char error_msg[200];
            sprintf(error_msg, "cannot assign %s to %s", 
                    type_to_string(expr_type), type_to_string(symbol->type));
            semantic_error(SEM_ERROR_TYPE_MISMATCH, error_msg, node->token.line);
            return 0;
        }
        
        // Mark as initialized
        symbol->is_initialized = 1;
    }
    
    return expr_valid;
}

// Check semantic correctness of a condition
int check_condition(ASTNode* node, SymbolTable* table) {
    if (!node) {
        return 0;
    }
    
    int expr_type;
    int valid = check_expression(node, table, &expr_type);
    
    return valid;
}

// Check semantic correctness of an if statement
int check_if_statement(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != AST_IF) {
        return 0;
    }
    
    // Check condition
    int condition_valid = check_condition(node->left, table);
    
    // Check the 'then' part (could be an else node)
    int then_valid = 1;
    if (node->right) {
        if (node->right->type == AST_ELSE) {
            // Check 'then' block
            then_valid = check_block(node->right->left, table);
            
            // Check 'else' block
            int else_valid = check_block(node->right->right, table);
            
            return condition_valid && then_valid && else_valid;
        } else {
            // Just 'then' block, no else
            then_valid = check_block(node->right, table);
        }
    }
    
    return condition_valid && then_valid;
}

// Check semantic correctness of a while statement
int check_while_statement(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != AST_WHILE) {
        return 0;
    }
    
    // Check condition
    int condition_valid = check_condition(node->left, table);
    
    // Check loop body
    int body_valid = check_block(node->right, table);
    
    return condition_valid && body_valid;
}

// Check semantic correctness of a repeat-until statement
int check_repeat_until_statement(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != AST_FOR) { // repeat-until uses AST_FOR
        return 0;
    }
    
    // Check loop body
    int body_valid = check_block(node->left, table);
    
    // Check condition
    int condition_valid = check_condition(node->right, table);
    
    return body_valid && condition_valid;
}

// Check semantic correctness of a print statement
int check_print_statement(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != AST_PRINT) {
        return 0;
    }
    
    // Check the expression being printed
    if (node->left) {
        int expr_type;
        return check_expression(node->left, table, &expr_type);
    }
    
    return 1;
}

// Check semantic correctness of a return statement
int check_return_statement(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != AST_RETURN) {
        return 0;
    }
    
    // Check return expression
    if (node->left) {
        int expr_type;
        return check_expression(node->left, table, &expr_type);
    }
    
    return 1; // Valid if no return expression (void function)
}

// Check semantic correctness of a function declaration
int check_function_declaration(ASTNode* node, SymbolTable* table) {
    if (!node || node->type != AST_FUNCTION_DECL) {
        return 0;
    }
    
    const char* func_name = node->token.lexeme;
    
    // Check if function already declared in current scope
    Symbol* existing = lookup_symbol_current_scope(table, func_name);
    if (existing) {
        semantic_error(SEM_ERROR_REDECLARED_VARIABLE, func_name, node->token.line);
        return 0;
    }
    
    // Add function to symbol table (assuming int return type for now)
    add_symbol(table, func_name, TOKEN_INT, node->token.line);
    Symbol* func_symbol = lookup_symbol_current_scope(table, func_name);
    if (func_symbol) {
        func_symbol->is_initialized = 1; // Functions are always "initialized"
    }
    
    // Enter a new scope for parameters and body
    enter_scope(table);
    
    // Process parameters (if any)
    ASTNode* param = node->left;
    while (param) {
        if (param->type == AST_VARDECL) {
            // Add parameter to symbol table (assuming int type for now)
            add_symbol(table, param->token.lexeme, TOKEN_INT, param->token.line);
            Symbol* param_symbol = lookup_symbol_current_scope(table, param->token.lexeme);
            if (param_symbol) {
                param_symbol->is_initialized = 1; // Parameters are initialized
            }
        }
        
        param = param->right; // Next parameter
    }
    
    // Process function body
    int body_valid = 1;
    if (node->right) {
        body_valid = check_block(node->right, table);
    }
    
    // Exit function scope
    exit_scope(table);
    
    return body_valid;
}

// Check semantic correctness of a block
int check_block(ASTNode* node, SymbolTable* table) {
    if (!node) {
        return 1; // Empty block is valid
    }
    
    if (node->type != AST_BLOCK) {
        // Single statement, not a block
        return check_statement(node, table);
    }
    
    // Enter a new scope for the block
    enter_scope(table);
    
    int valid = 1;
    
    // Process statements in the block
    ASTNode* current = node;
    while (current && current->type == AST_BLOCK) {
        if (current->left) {
            valid = check_statement(current->left, table) && valid;
        }
        
        current = current->right;
    }
    
    // Exit block scope
    exit_scope(table);
    
    return valid;
}

// Check semantic correctness of a statement
int check_statement(ASTNode* node, SymbolTable* table) {
    if (!node) {
        return 1; // Empty statement is valid
    }
    
    switch (node->type) {
        case AST_VARDECL:
            return check_declaration(node, table);
            
        case AST_ASSIGN:
            return check_assignment(node, table);
            
        case AST_IF:
            return check_if_statement(node, table);
            
        case AST_WHILE:
            return check_while_statement(node, table);
            
        case AST_FOR: // repeat-until
            return check_repeat_until_statement(node, table);
            
        case AST_PRINT:
            return check_print_statement(node, table);
            
        case AST_RETURN:
            return check_return_statement(node, table);
            
        case AST_BLOCK:
            return check_block(node, table);
            
        case AST_FUNCTION_DECL:
            return check_function_declaration(node, table);
            
        case AST_FACTORIAL: {
            int result_type;
            return check_expression(node, table, &result_type);
        }
            
        case AST_FUNCTION_CALL: {
            int result_type;
            return check_expression(node, table, &result_type);
        }
            
        default:
            // Non-declared statement type
            return 0;
    }
}

// Check semantic correctness of a program
int check_program(ASTNode* node, SymbolTable* table) {
    if (!node) {
        return 1; // Empty program is valid
    }
    
    if (node->type != AST_PROGRAM) {
        return check_statement(node, table);
    }
    
    int valid = 1;
    
    // Check left child (statement)
    if (node->left) {
        valid = check_statement(node->left, table) && valid;
    }
    
    // Check right child (rest of program)
    if (node->right) {
        valid = check_program(node->right, table) && valid;
    }
    
    return valid;
}

// Main semantic analysis function
int analyze_semantics(ASTNode* ast) {
    // Reset error count
    semantic_error_count = 0;
    
    // Initialize symbol table
    SymbolTable* table = init_symbol_table();
    
    // Perform semantic analysis
    int valid = check_program(ast, table);
    
    // Print symbol table for debugging
    print_symbol_table(table);
    
    // Print summary
    printf("\nSemantic analysis %s. Found %d error(s).\n", 
           semantic_error_count == 0 ? "successful" : "failed",
           semantic_error_count);
    
    // Free symbol table
    free_symbol_table(table);
    
    return valid && (semantic_error_count == 0);
}

// Process semantic analysis on a file
void proc_semantic_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }
    
    char buffer[2048];
    size_t len = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[len] = '\0';
    fclose(file);
    
    printf("\n==============================\n");
    printf("SEMANTIC ANALYSIS OF FILE: %s\n", filename);
    printf("==============================\n");
    printf("Input:\n%s\n\n", buffer);
    
    // Reset lexical and parsing state
    reset_lexer();
    
    // Parse to get AST
    parser_init(buffer);
    ASTNode *ast = parse();
    
    // Perform semantic analysis
    printf("\nPERFORMING SEMANTIC ANALYSIS...\n");
    int semantically_valid = analyze_semantics(ast);
    
    if (semantically_valid) {
        printf("\nSemantic analysis completed successfully. No errors found.\n");
    } else {
        printf("\nSemantic analysis failed. Errors detected.\n");
    }
    
    printf("==============================\n");

    // Free AST memory
    free_ast(ast);
}
