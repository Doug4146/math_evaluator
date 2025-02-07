#ifndef LEX_H
#define LEX_H


// Enumeration for valid token types
typedef enum {
    TOKEN_NUMBER,

    TOKEN_OPERATOR_PLUS, TOKEN_OPERATOR_MINUS,
    TOKEN_OPERATOR_MULTIPLY, TOKEN_OPERATOR_DIVIDE,

    TOKEN_OPEN_PARENTHESIS, TOKEN_CLOSED_PARENTHESIS,

    TOKEN_FUNCTION, 

    TOKEN_KEYWORD_PI, TOKEN_KEYWORD_E,

    TOKEN_EOF
} TypeToken;


// Struct for token. Includes token type, a pointer to the start of lexemme 
// in main string, and length of lexemme.
typedef struct Token {
    TypeToken typeToken;
    char* pLexemmeStart;
    int length;
} Token;


// Struct for a dynamic array of tokens. Includes pointer array of pointers to tokens, maximum capacity of the list
// and current position in the list. This is the main output of the lexer.
typedef struct TokenList {
    Token** array;
    int position;
    int maxCapacity;
} TokenList;


// Prints the data of the input `token` in format: "Token(type: "TYPE_TOKEN", value: "VALUE")"
// Returns 0 upon successful call, and 1 if errors encountered. Errors are fatal.
int print_token(Token* token);



// Initializes the fields for a `tokenList` struct instance
// Returns 0 if successful, 1 if errors encountered. Errors are fatal.
int init_tokenList(TokenList* tokenList);



/**
 * @brief Performs lexical analysis on the input source string, identifying tokens and storing them in a TokenList.
 * 
 * @param sourceString A null-terminated string containing the mathematical expression to be tokenized.
 * @param tokenList A pointer to a TokenList struct where the identified tokens will be stored.
 * @return int Returns 0 on success, or 1, represented by an error code 
 *         (e.g., ERROR_INVALID_FUNCTION_PARAMETERS, ERROR_FATAL_FUNCTION_CALL) on failure. Errors are fatal.
 */
int lexical_analyzer(char* sourceString, TokenList* tokenList);


// Prints every token in the input `tokenList` struct.
// Returns 0 upon successful call, and 1 if errors encountered. Errors are fatal.
int print_tokenList(TokenList* tokenList);


/*
 * - Frees the memory allocated for the TokenList array and its tokens.
 * - Ensures that no dangling pointers remain and that all allocated memory is properly freed.
 * - The original TokenList struct needs not to be freed (it was allocated onto the stack).
 * - Returns 0 upon success, 1 upon errors.
 */
int free_tokenList_memory(TokenList* tokenList);


#endif // LEX_H