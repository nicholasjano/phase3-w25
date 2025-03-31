#include <stdio.h>
#include <stdlib.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/semantic.h"

// External processing functions
extern void proc_test_file(const char* filename);
extern void proc_semantic_file(const char* filename);

int main(int argc, char* argv[]) {
    // Default test files if no arguments provided
    const char* default_files[] = {
        "../test/input_valid.txt",
        "../test/input_invalid.txt",
        "../test/input_semantic_error.txt"
    };
    
    if (argc < 2) {
        printf("No files specified, running all test files...\n");
        
        // Process syntax analysis on valid and invalid files
        printf("\n===== PARSING & SYNTAX ANALYSIS =====\n");
        proc_test_file(default_files[0]);
        proc_test_file(default_files[1]);
        
        // Process semantic analysis on valid and semantic error files
        printf("\n===== SEMANTIC ANALYSIS =====\n");
        proc_semantic_file(default_files[0]);
        proc_semantic_file(default_files[2]);
    } else {
        // Process each file specified as arguments
        for (int i = 1; i < argc; i++) {
            // Run both syntax and semantic analysis
            proc_test_file(argv[i]);
            proc_semantic_file(argv[i]);
        }
    }
    
    return 0;
}