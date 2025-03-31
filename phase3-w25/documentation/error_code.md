# Backwards C Error Codes and Error Handling

## 1. Parse Error Type Enumeration

### PARSE_ERROR_NONE (0)

- Normal parsing state
- No error condition present

```c
// Successful parsing
tni x = 10;  // No error
```

### PARSE_ERROR_UNEXPECTED_TOKEN (1)

- Token doesn't match expected grammar
- Example: Invalid tokens in expressions or statements

```c
tni 10;       // Expected identifier after 'tni'
fi $ (x > 0)  // Unexpected token '$'
```

### PARSE_ERROR_MISSING_SEMICOLON (2)

- Statement missing terminating semicolon
- Examples:

```c
tni x = 10    // Missing semicolon
tnirp "Hello" // Missing semicolon
```

### PARSE_ERROR_MISSING_IDENTIFIER (3)

- Expected identifier not found
- Examples:

```c
tni = 30;     // Missing variable name after type
tni niam( {   // Missing parameter name
```

### PARSE_ERROR_MISSING_EQUALS (4)

- Assignment without equals sign
- Examples:

```c
x 10;         // Missing '=' in assignment
tni y 20;     // Missing '=' in initialization
```

### PARSE_ERROR_MISSING_PARENTHESES (5)

- Unbalanced or missing parentheses
- Examples:

```c
fi (x > 10 {  // Missing closing parenthesis
lairotcaf);   // Missing opening parenthesis
```

### PARSE_ERROR_MISSING_CONDITION (6)

- Empty condition in control structures
- Examples:

```c
fi () {       // Empty condition in if
elihw () {    // Empty condition in while
```

### PARSE_ERROR_BLOCK_BRACES (7)

- Unbalanced or missing braces in blocks
- Examples:

```c
fi (x > 10) { // Missing closing brace
  tnirp x;
tni func() ;  // Function without body
```

### PARSE_ERROR_INVALID_OPERATOR (8)

- Operator in invalid context
- Examples:

```c
x + + y;      // Double operator
fi (a === b)  // Invalid operator '==='
```

### PARSE_ERROR_INVALID_FUNCTION_CALL (9)

- Malformed function call
- Examples:

```c
lairotcaf(;   // Missing argument
func(a, );    // Trailing comma
```

### PARSE_ERROR_INVALID_EXPRESSION (10)

- Malformed or empty expression
- Examples:

```c
tni x = ;     // Missing expression after '='
x = a + ;     // Incomplete expression
```

## 2. Semantic Error Type Enumeration

### SEM_ERROR_NONE (0)

- Normal semantic state
- No error condition present

### SEM_ERROR_UNDECLARED_VARIABLE (1)

- Using a variable that hasn't been declared
- Examples:

```c
a = 5;        // Error: 'a' not declared
tnirp b;      // Error: 'b' not declared
```

### SEM_ERROR_REDECLARED_VARIABLE (2)

- Declaring a variable that already exists in the same scope
- Examples:

```c
tni a = 10;
tni a = 20;   // Error: 'a' already declared in this scope
```

### SEM_ERROR_TYPE_MISMATCH (3)

- Incompatible types in operations or assignments
- Examples:

```c
tni a = "hello";  // Error: assigning string to int
lairotcaf(1.5);   // Error: factorial requires int argument
```

### SEM_ERROR_UNINITIALIZED_VARIABLE (4)

- Using a variable before it has been assigned a value
- Examples:

```c
tni x;
tnirp x;      // Error: 'x' may be used uninitialized
```

### SEM_ERROR_INVALID_OPERATION (5)

- Operation not valid for the given operands
- Examples:

```c
x = 5 / 0;    // Error: division by zero
```

### SEM_ERROR_SEMANTIC_ERROR (6)

- Generic semantic error for other cases
- Examples:

```c
// Out of scope access
{
    tni x = 10;
}
tnirp x;      // Error: 'x' not accessible here
```

## 3. Parser Synchronization Strategy

The parser uses synchronization to recover from errors and continue parsing:

### 3.1 Synchronization Points

- Semicolons (statement boundaries)
- Opening/closing braces (block boundaries)
- Keywords that start new statements

```c
static void synchronize(void) {
    advance(); // Skip the current token that caused the error
    
    while (!match(TOKEN_EOF)) {
        // Semicolon marks the end of most statements
        if (match(TOKEN_SEMICOLON)) {
            advance(); // Skip the semicolon
            return;
        }
        
        // Right brace might end a block
        if (match(TOKEN_RBRACE)) {
            return; // Don't advance yet, let the block parser handle it
        }
        
        // New statement starters
        if (match(TOKEN_INT) || match(TOKEN_FLOAT_KEY) || match(TOKEN_CHAR) ||
            match(TOKEN_VOID) || match(TOKEN_RETURN) || match(TOKEN_IF) || 
            match(TOKEN_WHILE) || match(TOKEN_PRINT) || match(TOKEN_LBRACE) ||
            match(TOKEN_REPEAT) || match(TOKEN_ELSE) || match(TOKEN_IDENTIFIER)) {
            return; // Don't advance, let the statement parser handle it
        }
        
        advance();
    }
}
```

## 4. Semantic Error Handling Strategy

The semantic analyzer detects and reports errors without attempting complex recovery:

### 4.1 Error Reporting

```c
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
```

### 4.2 Semantic Error Recovery

Unlike syntax error recovery, semantic error recovery is simpler:
- Report the error
- Continue analyzing when possible
- Track total error count
- Return overall success/failure at the end

This approach allows detecting multiple semantic errors in a single pass.

## 5. Symbol Table Management

The semantic analyzer maintains a symbol table to track variables:

```c
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
```

### 5.1 Scope Management

- Enter scope: Increase scope level when entering a block
- Exit scope: Decrease scope level when exiting a block
- Variable lookup: Search from current scope outward to global
- Symbol table tracks all symbols with their scope levels

### 5.2 Initialization Tracking

- Each symbol tracks whether it's been initialized
- Warnings for using uninitialized variables
- Assignment statements mark variables as initialized

## 6. Common Error Patterns and Examples

### 6.1 Undeclared Variable

```c
// Using variable before declaration
a = 5;       // Error: Undeclared variable 'a'
```

### 6.2 Uninitialized Variable

```c
// Using variable before assigning value
tni x;
tnirp x;     // Error: Variable 'x' may be used uninitialized
```

### 6.3 Redeclaration in Same Scope

```c
// Declaring same variable twice
tni a = 10;
tni a = 20;  // Error: Variable 'a' already declared in this scope
```

### 6.4 Out of Scope Access

```c
// Using variable outside its scope
{
    tni x = 10;
}
tnirp x;     // Error: Undeclared variable 'x'
```

### 6.5 Type Mismatch

```c
// Incompatible types
tni x = "string";  // Error: Type mismatch
```