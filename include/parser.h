#ifndef PARSER_H
#define PARSER_H


// LEX module convereted argv string -> list of token structs. For parsing will now convert list of token structs into
// RPN using shunting yard, and then evaulate with stacks. Much faster (lack of recursion) and simpler

// Parser errors (invalid function names, invalid parenthesis, and unexpected token list (e.g. two operators in a row)) happen during
// Conversion to RPN

// Other errors (divide by 0, more invalid stuff, happen during evalution of rpn)


// Structure for a fixed-length stack version of an array of token pointers.
// Used for operator stack AND output token list in shunting yard algorithm 
typedef struct StackTokenList {
    Token** array;
    int top;
} StackTokenList;


// Function for initializing `stackTokenList` based on the `lexicalTokenList` produced by the lexer module
// Note that any instance of StackTokenList can ONLY store up to the maximum number of tokens produced by the lexer or less
// Returns 0 upon successful call. 1 if errors encountered. Errors are fatal.
int init_StackTokenList(TokenList* lexicalTokenList, StackTokenList* stackTokenList);


// Function implementation of the shunting yard algorithm.
// Takes input `lexicalTokenList` created from lex module and an initialized `postfixTokenList`
// Performs shunting yard algorithm and fills out the postfixTokenList
// Returns 0 upon success. 1 if errors encountered. Errors are fatal.
int shunting_yard_algorithm(TokenList* lexicalTokenList, StackTokenList* postfixTokenList);


// Prints every token in the input `stackTokenList` struct.
// Returns 0 upon successful call, and 1 if errors encountered. Errors are fatal.
int print_stackTokenList(StackTokenList* stackTokenList);


/*
 * - Frees the memory allocated for the StackTokenList array (tokens themselves are freed seperately)
 * - Ensures that no dangling pointers remain and that all allocated memory is properly freed.
 * - The original TokenList struct needs not to be freed (it was allocated onto the stack).
 * - Returns 0 upon success, 1 upon errors.
 */
int free_stackTokenList_memory(StackTokenList* stackTokenList);










#endif // PARSER_H


