# Compiler Semantic Analyzer Phase

## Semantic Analyzer Overview

This semantic analyzer is the third phase for the "Backwards C" compiler. It takes the Abstract Syntax Tree (AST) from the parser (Phase 2) and performs contextual checks to ensure the program is not only syntactically correct but also semantically valid according to the language rules. The semantic analyzer uses a symbol table to track variables, their types, and scopes.

Key components of the semantic analyzer implementation include:
- Symbol table creation and management
- Variable declaration and usage validation
- Scope management and variable visibility rules
- Type checking and compatibility verification
- Initialization tracking to prevent use of uninitialized variables
- Error detection, reporting, and recovery mechanisms

## Features

- **Symbol Table Management**
  - Linked-list implementation of symbol records
  - Scope level tracking for nested blocks
  - Variable initialization status tracking
  - Line number information for detailed error reporting
  - Support for looking up symbols across scopes

- **Variable Checking**
  - Declaration before use verification
  - Detection of redeclarations in the same scope
  - Tracking of variable initialization status
  - Warning for potentially uninitialized variable usage
  - Proper handling of variable shadowing

- **Type System**
  - Support for basic types (int, float, char, void)
  - Type compatibility checking in expressions
  - Implicit type conversion where appropriate
  - Validation of operand types for operators
  - Special handling for arithmetic operations

- **Scope Analysis**
  - Block-level variable scoping
  - Proper nested scope handling
  - Access control for variables based on scope
  - Validation of variable references across scopes
  - Support for function parameter scoping

- **Error Handling**
  - Detailed error messages with line information
  - Specific error types for different semantic issues
  - Continuation of analysis after errors when possible
  - Error count tracking and summary reporting
  - Prevention of cascading errors

- **Special Feature Validation**
  - Factorial function argument type checking
  - Control statement condition validation
  - Block statement scope management
  - Function declaration and parameter handling

## Semantic Rules Enforced

### Variable Declaration and Usage
```c
// Variables must be declared before use
x = 10;                 // Error: x not declared

// Variables cannot be redeclared in the same scope
tni a = 5;
tni a = 10;             // Error: a already declared

// Variables are initialized with declaration+assignment or assignment
tni x = 10;             // x is initialized
tni y;                  // y is not initialized
y = 5;                  // y is now initialized

// Using uninitialized variables generates a warning
tni z;
tnirp z;                // Warning: z may be used uninitialized
```

### Scope Rules
```c
// Variables are only accessible in their scope or inner scopes
{
    tni a = 10;
    {
        tni b = 20;
        // a and b are accessible here
    }
    // Only a is accessible here
}
// Neither a nor b are accessible here

// Inner scopes can shadow outer variables
tni x = 10;
{
    tni x = 20;         // New variable that shadows outer x
    tnirp x;            // Prints 20
}
tnirp x;                // Prints 10
```

### Type Checking
```c
// Operands must have compatible types
tni a = 5;
taolf b = 3.14;
a + b;                  // Valid: numeric types are compatible

// Comparison operations produce boolean results (represented as int)
fi (a > b) { }          // Valid: comparison produces boolean

// Special operations have type requirements
tni fact = lairotcaf(5);     // Valid: factorial requires integer
tni invalid = lairotcaf(3.5); // Error: factorial requires integer
```

### Function Handling
```c
// Functions must be declared before use
tni add(tni a, tni b) {
    nruter a + b;
}
tni result = add(5, 10); // Valid: add is declared

// Function parameters create new scope
tni mul(tni x, tni y) {
    nruter x * y;       // x and y are in function scope
}
```

### Control Flow
```c
// Conditions in control statements are validated
fi (condition) {        // condition is checked for validity
    // Then branch
}

// Block statements create new scopes
elihw (x > 0) {
    tni y = x;          // y is only accessible in the loop
    x = x - 1;
}
// y is not accessible here
```

## Error Types and Messages

The semantic analyzer produces specific error messages for different types of semantic errors:

```
Semantic Error at line 4: Undeclared variable 'a'
Semantic Error at line 11: Variable 'c' may be used uninitialized
Semantic Error at line 14: Variable 'b' already declared in this scope
Semantic Error at line 32: Undeclared variable 'd'
Semantic Error at line 39: Type mismatch involving 'factorial'
```

## Symbol Table Structure

The symbol table is implemented as a linked list of symbol records:

```c
typedef struct Symbol {
    char name[100];          // Variable name
    int type;                // Data type (int, etc.)
    int scope_level;         // Scope nesting level
    int line_declared;       // Line where declared
    int is_initialized;      // Has been assigned a value?
    struct Symbol* next;     // For linked list implementation
} Symbol;

typedef struct {
    Symbol* head;            // First symbol in the table
    int current_scope;       // Current scope level
} SymbolTable;
```

The semantic analyzer uses the following operations on the symbol table:

- `init_symbol_table()`: Create a new symbol table
- `add_symbol()`: Add a new symbol to the current scope
- `lookup_symbol()`: Find a symbol across all accessible scopes
- `lookup_symbol_current_scope()`: Find a symbol in the current scope only
- `enter_scope()`: Increase the scope level when entering a block
- `exit_scope()`: Decrease the scope level when exiting a block
- `free_symbol_table()`: Release memory allocated for the symbol table

## Scoping Mechanism

Scopes are represented by integer levels:
- Global scope: level 0
- Function body: level 1
- Nested blocks: level 2, 3, etc.

When looking up a variable, the semantic analyzer searches from the current scope outward:
1. First look in the current scope
2. If not found, look in outer scopes
3. If found in multiple scopes, use the one from the innermost scope

Variable shadowing is handled by having multiple entries with the same name but different scope levels.

## Future Enhancements

Future enhancements could include:
- Enhanced function signature validation
- User-defined types and structures
- More sophisticated type inference and conversion
- Constant expression evaluation
- Warning for potential null pointer or division by zero