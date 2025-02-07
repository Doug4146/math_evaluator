#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "lex.h"

static const int INITIAL_TOKEN_CAPACITY = 10;  // Initial capacity for the tokenArr in lexer_Output instance
static const float FACTOR_TOKEN_INCREASE = 1.5;  // Factor by which to increase token capacity


// Returns the string version of the input token type's name.
// Returns NULL upon failure (invalid token type inputted). Errors are fatal.
static char* token_type_to_string(TypeToken typeToken) {
    switch (typeToken) {
        case TOKEN_NUMBER: 
            return "TOKEN_NUMBER";
        case TOKEN_OPERATOR_PLUS: 
            return "TOKEN_OPERATOR_PLUS";
        case TOKEN_OPERATOR_MINUS: 
            return "TOKEN_OPERATOR_MINUS";
        case TOKEN_OPERATOR_MULTIPLY: 
            return "TOKEN_OPERATOR_MULTIPLY";
        case TOKEN_OPERATOR_DIVIDE: 
            return "TOKEN_OPERATOR_DIVIDE";
        case TOKEN_OPEN_PARENTHESIS: 
            return "TOKEN_OPEN_PARENTHESIS";
        case TOKEN_CLOSED_PARENTHESIS: 
            return "TOKEN_CLOSED_PARENTHESIS";
        case TOKEN_FUNCTION: 
            return "TOKEN_FUNCTION";
        case TOKEN_KEYWORD_E: 
            return "TOKEN_KEYWORD_E";
        case TOKEN_KEYWORD_PI: 
            return "TOKEN_KEYWORD_PI";
        case TOKEN_EOF: 
            return "TOKEN_EOF";
        default: 
            return NULL;
    }
}


int print_token(Token* token) {

    // Validating the fields of the input token struct
    if (token == NULL || token->pLexemmeStart == NULL || token->length < 1) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    // Convert the token type's name to string
    char* tokenTypeString = token_type_to_string(token->typeToken);
    if (tokenTypeString == NULL) {
        return ERROR_FATAL_FUNCTION_CALL; 
    }

    // Print the token data. Token value is printed on character at a time
    printf("Token(type: %s, value: '", tokenTypeString);
    printf("%.*s", token->length, token->pLexemmeStart);
    printf("')\n");

    return 0; 
}


// Allocates memory for a token struct on the heap. Takes in parameters for the new token's fields.
// Returns a pointer to the new token. Returns NULL upon errors, this error is fatal.
static Token* create_token(TypeToken typeToken, char* pLexemmeStart, int length) {

    // Validating function parameters
    if (pLexemmeStart == NULL || length < 1) {
        return NULL;
    }

    // Allocate memory on heap for the new token structure
    Token* token = malloc(sizeof(Token)); 
    if (token == NULL) {
        return NULL;
    }

    // Initialize the token's fields
    token->typeToken = typeToken;
    token->pLexemmeStart = pLexemmeStart;
    token->length = length;

    return token; 
}


int init_tokenList(TokenList* tokenList) {

    // Validating function parameters
    if (tokenList == NULL) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    tokenList->maxCapacity = INITIAL_TOKEN_CAPACITY;
    tokenList->array = malloc(tokenList->maxCapacity*sizeof(Token*));
    if (tokenList->array == NULL) {
        // fprintf(stderr, "Fatal error: memory could not be allocated.\n\n");
        return ERROR_MEMORY_ALLOCATION_FAILURE;
    }
    tokenList->position = -1;

}


// Adds a pre-created token pointer to the dynamic pointer array of token pointers in the TokenList struct.
// Returns 0 upon successful call, and 1 if errors encountered. Errors are fatal.
static int add_token_to_list(TokenList* tokenList, Token* token) {

    // Validating function parameters
    if (tokenList == NULL || tokenList->array == NULL || token == NULL || token->pLexemmeStart == NULL || token->length < 1) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    // Increment position along array and check if resizing required
    tokenList->position++;
    if (tokenList->position >= (tokenList->maxCapacity-1)) {

        tokenList->maxCapacity *= FACTOR_TOKEN_INCREASE; 

        // Resize the memory and store returned pointer in temporary variable
        Token** tempArray = realloc(tokenList->array, tokenList->maxCapacity * sizeof(Token*));
        if (tempArray == NULL) {
            return ERROR_MEMORY_ALLOCATION_FAILURE;
        }

        // Assign the resized array to the original array pointer
        tokenList->array = tempArray;
    }

    // Add token to the array
    tokenList->array[tokenList->position] = token;

    return 0; 

}


// Checks if input character is a digit (0-9). 1 if yes, else 0.
static int is_digit(char value) {
    return ('0' <= value && value <= '9');
}


// Checks if input character is an operator or paranthesis. 1 if yes, else 0.
static int is_operator_or_paren(char value) {
    return (value == '+' || value == '-' || value == '*' || value == '/' || 
            value == '(' || value == ')');
}


// Checks if input character is a (LOWERCASE) alphabetical letter (a-z). 1 if yes, else 0.
static int is_alpha(char value) {
    return ('a' <= value && value <= 'z');
}


// Checks if input character is a plus '+' or '-'. 1 if yes, else 0.
static int is_plus_or_minus(char value) {
    return (value == '+' || value == '-');
}


/*
- Is called when lexer identifies a number. Traverses string section pointed to by `lexemmeStart` and checks if number.
- Supports integers, floating-point numbers (with '.'), and scientific notation ('E+' or 'E-'). Valid numbers must be
  immediately followed by whitespace, an operator, a parenthesis, or the null terminator.
- Upon detection of invaid syntax, an error message is printed, and NULL is returned.
- If valid number found, creates a new token and `returns` its pointer. All errors are fatal.
*
*/
static Token* scan_number(char* lexemmeStart) {

    // Validating function parameters
    if (lexemmeStart == NULL) {
        return NULL;
    }

    // For number length and string traversal
    int counter = 0;
    char* traverser = lexemmeStart;

    // Scan for integer part of potential number until non-digit character encountered
    while (is_digit(*traverser)) {
        counter++; 
        traverser++; 
    }

    // Check for potential float (must have digits after '.')
    if (*traverser == '.') { 
        if (is_digit(*(traverser+1))) {
            //consume the dot
            counter++; 
            traverser++;

            // Scan fractional part of potential number until non-digit character encountered
            while (is_digit(*traverser)) {
                counter++;
                traverser++;
            }
        }
        else {
            fprintf(stderr, "\nError: invalid floating-point number at '%.*s'.\n", counter+2, lexemmeStart);
            return NULL; // Report invalid syntax
        }
    }

    // Check for potential scientific notation (must have 'E+' or 'E-' AND a following digit)
    if (*traverser == 'E') {
        if (is_plus_or_minus(*(traverser+1))) {
            if (is_digit(*(traverser+2))) {
                //consume the 'E+' or 'E-'
                counter += 2;
                traverser += 2;

                // Scan the scientific notation part of potential number
                while (is_digit(*traverser)) {
                    counter++;
                    traverser++;
                }
            }
            else {
                fprintf(stderr, "\nError: invalid scientific notation at '%.*s'.\n", counter+3, lexemmeStart);
                return NULL; // Report invalid syntax
            }
        }
        else {
            fprintf(stderr, "\nError: invalid scientific notation at '%.*s'.\n", counter+2, lexemmeStart);
            return NULL; // Report invalid syntax
        }
    }

    // Check if the number is not followed by a valid character
    if (*traverser != ' ' && *traverser != '\0' && !is_operator_or_paren(*traverser)) { 
        // Report the invalid number syntax and terminate the program
        fprintf(stderr, "\nError: invalid character after number at '%.*s'.\n", counter+1, lexemmeStart);
        return NULL;
    }

    // Create and return valid number token. // If NULL, lexer_analyzer will flag it.
    return create_token(TOKEN_NUMBER, lexemmeStart, counter);

}  


/*
- Is called when lexer identifies a letter. Traverses string section pointed to by `lexemmeStart` and checks if potential function
- Functions may ONLY include letter characters (a-z) and must be immediately followed by a left parenthesis
- Upon detection of invaid syntax, an error message is printed, and NULL is returned.
- If potentially valid function found, creates a new token and `returns` its pointer. All errors are fatal.
*
*/
static Token* scan_function(char* lexemmeStart) {

    // Validating function parameters
    if (lexemmeStart == NULL) {
        return NULL;
    }

    // For number length and string traversal
    int counter = 0;
    char* traverser = lexemmeStart;

    // Scan for the entire function.
    while (is_alpha(*traverser)) {
        counter++; 
        traverser++; 
    }

    // Check if the function name is a known keyword constant (e or pi)

    if (counter == 1 && (strncmp(lexemmeStart, "e", 1) == 0)) {
        // Check if the keyword is not followed by a valid character
        if (!is_operator_or_paren(*traverser) && *traverser != ' ' && *traverser != '\0') { 
            // Report the invalid keyword syntax and terminate the program
            fprintf(stderr, "\nError: invalid character after keyword at '%.*s'.\n", counter+1, lexemmeStart);
            return NULL;
        }
        // Create and return valid number token. If NULL, lexer_analyzer will flag it.
        return create_token(TOKEN_KEYWORD_E, lexemmeStart, counter);
    }
    if (counter == 2 && (strncmp(lexemmeStart, "pi", 2) == 0)) {
        // Check if the keyword is not followed by a valid character
        if (!is_operator_or_paren(*traverser) && *traverser != ' ' && *traverser != '\0') { 
            // Report the invalid keyword syntax and terminate the program
            fprintf(stderr, "\nError: invalid character after keyword at '%.*s'.\n", counter+1, lexemmeStart);
            return NULL;
        }
        // Create and return valid number token. If NULL, lexer_analyzer will flag it.
        return create_token(TOKEN_KEYWORD_PI, lexemmeStart, counter);
    }


    // Check if the function is not followed by a valid character
    if (*traverser != '(') { 
        // Report the invalid function syntax and terminate the program
        fprintf(stderr, "\nError: invalid character after function at '%.*s'.\n", counter+1, lexemmeStart);
        return NULL;
    }

    // Create and return valid function token. If NULL, lexer_analyzer will flag it.
    return create_token(TOKEN_FUNCTION, lexemmeStart, counter);

}


int lexical_analyzer(char* sourceString, TokenList* tokenList) {

    // Validating function parameters
    if (sourceString == NULL || tokenList == NULL || tokenList->array == NULL) {
        return ERROR_INVALID_FUNCTION_PARAMETERS; 
    }

    // Pointer to traverse the source string
    char* pTraverse = sourceString;

    // Main scanning loop for the lexer.
    while (*pTraverse != '\0') {

        // Check if token found in current loop iteration and declare potential token struct
        int tokenFound = 0;
        Token* newToken = NULL; 

        // Switch case to determine the token type of the current character
        switch (*pTraverse) {
            case ' ': 
                pTraverse++; break; //current element is whitespace, ignore it and move on
            case '(': 
                newToken = create_token(TOKEN_OPEN_PARENTHESIS, pTraverse, 1); tokenFound = 1;
                pTraverse++; break;
            case ')':
                newToken = create_token(TOKEN_CLOSED_PARENTHESIS, pTraverse, 1); tokenFound = 1;
                pTraverse++; break;
            case '*':
                newToken = create_token(TOKEN_OPERATOR_MULTIPLY, pTraverse, 1); tokenFound = 1;
                pTraverse++; break;
            case '/':
                newToken = create_token(TOKEN_OPERATOR_DIVIDE, pTraverse, 1); tokenFound = 1;
                pTraverse++; break;
            case '+':
                newToken = create_token(TOKEN_OPERATOR_PLUS, pTraverse, 1); tokenFound = 1;
                pTraverse++; break;
            case '-':
                newToken = create_token(TOKEN_OPERATOR_MINUS, pTraverse, 1); tokenFound = 1;
                pTraverse++; break;
            default:
                // When digit is encountered, run following logic to determine if number
                if (is_digit(*pTraverse)) {
                    newToken = scan_number(pTraverse);
                    if (newToken == NULL) {
                        // NULL return from this function should terminate the program
                        return ERROR_FATAL_FUNCTION_CALL;
                    }
                    tokenFound = 1;
                    pTraverse += newToken->length;
                    break;
                }
                // When letter is encountered, run following logic to determine if function
                else if (is_alpha(*pTraverse)) {
                    newToken = scan_function(pTraverse);
                    if (newToken == NULL) {
                        return ERROR_FATAL_FUNCTION_CALL;
                    }
                    tokenFound = 1;
                    pTraverse += newToken->length;
                    break;
                }
                // Current character is unrecognizeable. Report and terminate the program.
                fprintf(stderr, "\nError: invalid character at '%.*s'.\n", 1, pTraverse);
                return ERROR_INVALID_PROGRAM_USAGE;
        }

        if (tokenFound) {
        
            // Checks for fatal create_token errors for non-default cases in switch-case
            if (newToken == NULL) {
                return ERROR_FATAL_FUNCTION_CALL;
            }

            // Add token to the TokenList struct. Any errors here should terminate the program
            int addToken = add_token_to_list(tokenList, newToken);
            if (addToken == 1) {
                free(newToken);
                newToken = NULL;
                return ERROR_FATAL_FUNCTION_CALL;
            }
        }
    
    }

    // Create an EOF token after main scanning loop has ran. 
    // Upon error, terminate program and free TokenList struct and related memory in main.c.
    Token* lastToken = create_token(TOKEN_EOF, "\0", 1);
    if (lastToken == NULL) {
        return ERROR_FATAL_FUNCTION_CALL; 
    }

    // Add EOF token to TokenList struct. 
    // Upon error, terminate program and free TokenList struct and related memory in main.c.
    int addToken = add_token_to_list(tokenList, lastToken);
    if (addToken == 1) {
        free(lastToken);
        lastToken = NULL; 
        return ERROR_FATAL_FUNCTION_CALL;
    }

    // Subroutine ran successfully
    return 0;
}


int print_tokenList(TokenList* tokenList) {

    // Validate function parameters
    if (tokenList == NULL || tokenList->array == NULL || tokenList->position < 0) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    printf("\n");
    for (int i = 0; i < tokenList->position + 1; i++) {
        int printOutput = print_token(tokenList->array[i]); 
        if (printOutput == 1) {
            // Errors are fatal and memory is freed in main.c
            return ERROR_FATAL_FUNCTION_CALL;
        }
    }

    // Subroutine ran successfully
    return 0;
}


int free_tokenList_memory(TokenList* tokenList) {

    // Validating function parameters. Errors are fatal and should terminate program.
    if (tokenList == NULL || tokenList->array == NULL || tokenList->position < 0) {
        return ERROR_INVALID_FUNCTION_PARAMETERS; 
    }

    // Free each token in the array
    for (int i = 0; i < tokenList->position + 1; i++) {
        if (tokenList->array[i] != NULL) { // Check if token is valid before freeing
            tokenList->array[i]->pLexemmeStart = NULL; 
            free(tokenList->array[i]); 
            tokenList->array[i] = NULL; 
        }
    }

    // Free the token array itself (recall its a dynamic (pointer) array of pointers to tokens)
    free(tokenList->array);
    tokenList->array = NULL;

    // Subroutine ran successfully
    return 0;

}




