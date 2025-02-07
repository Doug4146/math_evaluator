#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "lex.h"
#include "parser.h"


int init_StackTokenList(TokenList* lexicalTokenList, StackTokenList* stackTokenList) {

    // Validating function parameters
    if (lexicalTokenList == NULL || lexicalTokenList->array == NULL || lexicalTokenList->position < 0 || stackTokenList == NULL) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    // Initialize postfixTokenList fields
    stackTokenList->array = malloc(lexicalTokenList->position * sizeof(Token*));
    if (stackTokenList->array == NULL) {
        return ERROR_MEMORY_ALLOCATION_FAILURE;
    }
    stackTokenList->top = -1;

    // Subroutine ran successfully
    return 0;

}


// Function for pushing a token pointer onto the `stackTokenList`. 
// Returns 0 upon successful call. 1 if errors encountered. Errors are fatal.
static int push_StackTokenList(StackTokenList* stackTokenList, Token* token) {

    // Validating function parameters
    if (stackTokenList == NULL || token == NULL) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    // Push the token pointer to the stack
    stackTokenList->top++;  
    stackTokenList->array[stackTokenList->top] = token;

    // Subroutine ran successfully
    return 0;

}


// Function for popping a token pointer from the `stackTokenList`. 
// Returns the token pointer upon successful call. 1 if errors encountered. Errors are fatal.
static Token* pop_StackTokenList(StackTokenList* stackTokenList) {

    // Validating function parameters
    if (stackTokenList == NULL || stackTokenList->top == -1) {
        return NULL;
    }

    // Pop a token pointer off the stack
    Token* poppedToken = stackTokenList->array[stackTokenList->top];
    stackTokenList->array[stackTokenList->top] = NULL;
    stackTokenList->top--;

    // Subroutine ran successfully
    return poppedToken;

}


// Returns 1 if the input `stackTokenList` is empty, 0 otherwise.
static int stack_empty(StackTokenList* stackTokenList) {
    return ((stackTokenList->top == -1) ? 1 : 0);
}


// Returns precedence of input token type (operator). Returns 0 upon fail.
static int get_precedence(TypeToken typeOperator) {
    switch (typeOperator) {
        case TOKEN_OPERATOR_PLUS: return 1;
        case TOKEN_OPERATOR_MINUS: return 1;
        case TOKEN_OPERATOR_MULTIPLY: return 2;
        case TOKEN_OPERATOR_DIVIDE: return 2;
        default: return 0;
    }
}


// FUTURE IMPLEMENTATION FOR EXPONENTIATION. CHECKS IF OPERATOR IS LEFT ASSOCIATIVE (NOT EXPONENT)
// int is_left_associative(TypeToken typeOperator) {
//     return (token->typeToken == TOKEN_OPERATOR_EXPONENT);
// }


static const char* allowedFunctions[] = {"sin", "cos", "tan", "csc", "sec", "cot",
"asin", "acos", "atan", "acsc", "asec", "acot", "ln", "log", "exp"};
static const int numFunctions = sizeof(allowedFunctions) / sizeof(allowedFunctions[0]);


// Returns 1 if a potential function is supported. 0 if not, and -1 if error. // OPTIMIZE WITH HASHMAP
static int is_valid_function(char* pLexemmeStart, int length) {
    if (pLexemmeStart == NULL || length < 1) {
        return -ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    for (int i = 0; i < numFunctions; i++) {
        if (length == strlen(allowedFunctions[i]) && strncmp(pLexemmeStart, allowedFunctions[i], length) == 0) {
            return 1;  // Function is valid
        }
    }
    return 0;  // Function is invalid
}


int shunting_yard_algorithm(TokenList* lexicalTokenList, StackTokenList* postfixTokenList) {

    // Validating function parameters
    if (lexicalTokenList == NULL || lexicalTokenList->array == NULL || postfixTokenList == NULL) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    // Create and initialize the operator stack
    StackTokenList operatorStack;
    int initOperatorStack = init_StackTokenList(lexicalTokenList, &operatorStack);
    if (initOperatorStack == 1) {
        return ERROR_FATAL_FUNCTION_CALL;
    }

    // Iterate over all tokens in lexicalTokenList EXCEPT TOKEN_EOF
    for (int i = 0; i < lexicalTokenList->position; i++) {

        Token* currentToken = lexicalTokenList->array[i];
        int push;
        Token* popped = NULL;

        switch (currentToken->typeToken) {
            case (TOKEN_NUMBER):
            case (TOKEN_KEYWORD_E):
            case (TOKEN_KEYWORD_PI):
                push = push_StackTokenList(postfixTokenList, currentToken);
                break;
            case TOKEN_FUNCTION:
                int valid = is_valid_function(currentToken->pLexemmeStart, currentToken->length);
                if (valid == 1) {
                    push = push_StackTokenList(&operatorStack, currentToken);
                }
                else if (valid == 0) {
                    fprintf(stderr, "\nError: invalid function name at '%.*s'.\n", currentToken->length, currentToken->pLexemmeStart);
                    return ERROR_INVALID_PROGRAM_USAGE;
                }
                else {
                    return ERROR_FATAL_FUNCTION_CALL;
                }
                break;
            case TOKEN_OPEN_PARENTHESIS:
                push = push_StackTokenList(&operatorStack, currentToken);
                break;
            case TOKEN_CLOSED_PARENTHESIS:
                while (!stack_empty(&operatorStack)) {
                    if (operatorStack.array[operatorStack.top]->typeToken == TOKEN_OPEN_PARENTHESIS) {
                        break;
                    }
                    push = push_StackTokenList(postfixTokenList, pop_StackTokenList(&operatorStack));
                    if (push == 1) {
                        return ERROR_FATAL_FUNCTION_CALL;
                    }
                }
                // Check for mismatched parentheses
                if (stack_empty(&operatorStack) || operatorStack.array[operatorStack.top]->typeToken != TOKEN_OPEN_PARENTHESIS) {
                    fprintf(stderr, "\nError: mismatched parentheses.\n");
                    return ERROR_INVALID_PROGRAM_USAGE;
                }
                // Pop the matching '(' but do not push onto output
                popped = pop_StackTokenList(&operatorStack);
                // If the top of the stack is a function, pop it into output
                if (!stack_empty(&operatorStack) && operatorStack.array[operatorStack.top]->typeToken == TOKEN_FUNCTION) {
                    push_StackTokenList(postfixTokenList, pop_StackTokenList(&operatorStack));
                }
                break;
            default:
                while (!stack_empty(&operatorStack)) {
                    TypeToken topStackOperator = operatorStack.array[operatorStack.top]->typeToken;

                    if (topStackOperator == TOKEN_OPEN_PARENTHESIS) {
                        break;
                    } 

                    int topStackPrecedence = get_precedence(topStackOperator);
                    int currentTokenPrecedence = get_precedence(currentToken->typeToken);

                    if (topStackPrecedence >= currentTokenPrecedence) { // LATER ADD IMPLEMTATION FOR ASSOCIATIVITY (EXPONENT)
                        push_StackTokenList(postfixTokenList, pop_StackTokenList(&operatorStack));
                    }
                    else {
                        break;
                    }

                }
                push = push_StackTokenList(&operatorStack, currentToken);
                break;
        }

        if (push == 1) {
            return ERROR_FATAL_FUNCTION_CALL;
        }
        
    }

    // Pop remaining operators
    while (!stack_empty(&operatorStack)) {
        TypeToken topStackOperator = operatorStack.array[operatorStack.top]->typeToken;

        // If parentheses remain, mismatched error
        if (topStackOperator == TOKEN_OPEN_PARENTHESIS || topStackOperator == TOKEN_CLOSED_PARENTHESIS) {
            fprintf(stderr, "\nError: mismatched parentheses.\n");
            return ERROR_INVALID_PROGRAM_USAGE;
        }

        push_StackTokenList(postfixTokenList, pop_StackTokenList(&operatorStack));
    }

    // Subroutine ran successfully
    return 0;

}


int print_stackTokenList(StackTokenList* stackTokenList) {
    // Validate function parameters
    if (stackTokenList == NULL || stackTokenList->array == NULL || stackTokenList->top < 0) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    printf("\n");
    for (int i = 0; i < stackTokenList->top + 1; i++) {
        int printOutput = print_token(stackTokenList->array[i]); 
        if (printOutput == 1) {
            // Errors are fatal and memory is freed in main.c
            return ERROR_FATAL_FUNCTION_CALL;
        }
    }

    // Subroutine ran successfully
    return 0;
}


int free_stackTokenList_memory(StackTokenList* stackTokenList) {
    // Validate function parameters
    if (stackTokenList == NULL || stackTokenList->array == NULL || stackTokenList->top < 0) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    // Free each token in the array
    for (int i = 0; i < stackTokenList->top + 1; i++) {
        stackTokenList->array[i] = NULL; // The actual tokens will be freed by the free_tokenList_memory function
    }

    // Free the token array itself (recall its a dynamic (pointer) array of pointers to tokens)
    free(stackTokenList->array);
    stackTokenList->array = NULL;

    // Subroutine ran successfully
    return 0;

}







