#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "errors.h"
#include "lex.h"
#include "parser.h"


const static double pi = 3.14159265358979;
const static double  e = 2.71828182845905;
const static double EPSILON = 1e-10;


int init_StackTokenList(TokenList* lexicalTokenList, StackTokenList* stackTokenList) {

    // Validating function parameters
    if (lexicalTokenList == NULL || lexicalTokenList->array == NULL || stackTokenList == NULL) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    // Initialize postfixTokenList fields
    stackTokenList->array = malloc((lexicalTokenList->position + 1) * sizeof(Token*));
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
    if (stackTokenList == NULL || stackTokenList->array == NULL || token == NULL) {
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
    if (stackTokenList == NULL || stackTokenList->array == NULL || stackTokenList->top < 0) {
        return NULL;
    }

    // Pop a token pointer off the stack
    Token* poppedToken = stackTokenList->array[stackTokenList->top];
    stackTokenList->array[stackTokenList->top] = NULL;
    stackTokenList->top--;

    // Subroutine ran successfully
    return poppedToken;

}


// Function for initializing the fields for an instance of a doubleStack
// Assumes the max # of elements needed for the array is the # of tokens in the postfix array
// Returns 0 upon success, and 1 upon failure/
static int init_doubleStack(StackTokenList* stackTokenList, DoubleStack* doubleStack) {

    // Validating function parameters
    if (stackTokenList == NULL || stackTokenList->array == NULL || doubleStack == NULL) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    // Initialize doubleStack fields
    doubleStack->array = malloc((stackTokenList->top + 1) * sizeof(double));
    if (doubleStack->array == NULL) {
        return ERROR_MEMORY_ALLOCATION_FAILURE;
    }
    doubleStack->top = -1;

    // Subroutine ran successfully
    return 0;
}


// Function for pushing a double val onto the `doubleStack`. 
// Returns 0 upon successful call. 1 if errors encountered. Errors are fatal.
static int push_doubleStack(DoubleStack* doubleStack, double number) {

    // Validating function parameters
    if (doubleStack == NULL || doubleStack->array == NULL) {
        return ERROR_INVALID_FUNCTION_PARAMETERS;
    }

    // Push the token pointer to the stack
    doubleStack->top++;  
    doubleStack->array[doubleStack->top] = number;

    // Subroutine ran successfully
    return 0;
}


// Function for popping an element off of the `doubleStack`. 
// Returns the value (double) of tbe popped element
static double pop_doubleStack(DoubleStack* doubleStack) {

    // Validating function parameters
    if (doubleStack == NULL || doubleStack->array == NULL || doubleStack->top < 0) {
        return 0;
    }

    // Push the token pointer to the stack
    double popped = doubleStack->array[doubleStack->top];  
    doubleStack->array[doubleStack->top] = 0;
    doubleStack->top--;

    // Subroutine ran successfully
    return popped;
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


static const char* allowedFunctions[] = {"sin", "cos", "tan", "asin", "acos", "atan", "ln", "log", "exp"};
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


// Fuction for converting a number represented by a substring to a double value.
// `start` is a pointer to some element of the string, and `length` is length of substring. 
// Returns the double value of the number in string form.
static double convert_to_double(char *start, int length) {

    // Null-terminate the substring by creating a new temporary string
    char *temp = (char *)malloc(length + 1);  // +1 for null terminator
    if (temp == NULL) {
        return 0;
    }

    // Copy the substring to temp and null-terminate it
    strncpy(temp, start, length);
    temp[length] = '\0';

    // Convert the substring to a double using strtod
    double result = atof(temp);

    // Free the allocated memory
    free(temp);

    return result;
}


// Function for evaluating the `postfixTokenList` Returns the final answer (double).
// Any errors with memory allocation and etc are fatal.
double evaluate_postfixTokenList(StackTokenList* postfixTokenList) {

    // Validate input parameters
    if (postfixTokenList == NULL || postfixTokenList->array == NULL || postfixTokenList->top < 0) {
        return 0;
    }

    // Create a doubleStack for evaluation
    DoubleStack doubleStack;
    int initDoubleStack = init_doubleStack(postfixTokenList, &doubleStack);
    if (initDoubleStack == 1) {
        return 0;
    }

    // Main loop for iterating over the tokens in postfixTokenList
    for (int i = 0; i < postfixTokenList->top + 1; i++) {

        TypeToken typeToken = postfixTokenList->array[i]->typeToken;

        // Cases when current token is of type number or keyword constant (pi or e). Push to stack.
        if (typeToken == TOKEN_NUMBER) {
            double number = convert_to_double(postfixTokenList->array[i]->pLexemmeStart, postfixTokenList->array[i]->length);
            int push = push_doubleStack(&doubleStack, number);
            if (push == 1) {
                return 0;
            }
        }
        else if (typeToken == TOKEN_KEYWORD_PI) {
            int push = push_doubleStack(&doubleStack, pi);
            if (push == 1) {
                return 0;
            }
        }
        else if (typeToken == TOKEN_KEYWORD_E) {
            int push = push_doubleStack(&doubleStack, e);
            if (push == 1) {
                return 0;
            }
        }
        // Cases when current token is an operator (+, -, *, /)
        else if (typeToken == TOKEN_OPERATOR_PLUS ||
                 typeToken == TOKEN_OPERATOR_MINUS ||
                 typeToken == TOKEN_OPERATOR_MULTIPLY ||
                 typeToken == TOKEN_OPERATOR_DIVIDE) {

            // Pop two elements from double stack (last element popped is leftmost in order)
            double pop2 = pop_doubleStack(&doubleStack);
            double pop1 = pop_doubleStack(&doubleStack);

            double result;

            switch (typeToken) {
                case TOKEN_OPERATOR_PLUS:
                    result = pop1 + pop2;
                    break;
                case TOKEN_OPERATOR_MINUS:
                    result = pop1 - pop2;
                    break;
                case TOKEN_OPERATOR_MULTIPLY:
                    result = pop1 * pop2;
                    break;
                case TOKEN_OPERATOR_DIVIDE:
                    if (pop2 == 0) {
                        fprintf(stderr, "Error: divide by zero.\n");
                        return 0;
                    }
                    result = pop1 / pop2;
                    break;
            }
            int push = push_doubleStack(&doubleStack, result);
            if (push == 1) {
                return 0;
            }

        }
        // "sin", "cos", "tan", "asin", "acos", "atan", "ln", "log", "exp". ADD SUPPORT FOR ASIN, ACOS, ATAN
        // Case when current token is a function. Pop one number and apply function and push result.
        else if (typeToken == TOKEN_FUNCTION) {

            // Determine which function it is and apply
            if (strncmp(postfixTokenList->array[i]->pLexemmeStart, "sin", postfixTokenList->array[i]->length) == 0) {
                double pop = pop_doubleStack(&doubleStack);
                int push = push_doubleStack(&doubleStack, sin(pop));
                if (push == 1) {
                    return 0;
                }
            }
            else if (strncmp(postfixTokenList->array[i]->pLexemmeStart, "cos", postfixTokenList->array[i]->length) == 0) {
                double pop = pop_doubleStack(&doubleStack);
                int push = push_doubleStack(&doubleStack, cos(pop));
                if (push == 1) {
                    return 0;
                }
            }
            else if (strncmp(postfixTokenList->array[i]->pLexemmeStart, "tan", postfixTokenList->array[i]->length) == 0) {
                double x = pop_doubleStack(&doubleStack);
                // Check for domain issues with x in tan(x)
                if (fabs(cos(x)) < EPSILON) { // tan = sin/cos => undefined when cos = 0 or cos = REALLY close to 0
                    fprintf(stderr, "Error: tan(x) is undefined for x = %.4f.\n", x);
                    return 0;
                }
                int push = push_doubleStack(&doubleStack, tan(x));
                if (push == 1) {
                    return 0;
                }
            }
            else if (strncmp(postfixTokenList->array[i]->pLexemmeStart, "ln", postfixTokenList->array[i]->length) == 0) {
                double x = pop_doubleStack(&doubleStack);
                // Check for domain issues with x in ln(x)
                if (x < 0.0 + EPSILON) {
                    fprintf(stderr, "Error: ln(x) is undefined for x <= 0.\n", x);
                    return 0;
                }
                int push = push_doubleStack(&doubleStack, log(x)); // log means log_e
                if (push == 1) {
                    return 0;
                }
            }
            else if (strncmp(postfixTokenList->array[i]->pLexemmeStart, "log", postfixTokenList->array[i]->length) == 0) {
                double x = pop_doubleStack(&doubleStack);
                // Check for domain issues with x in ln(x)
                if (x < 0.0 + EPSILON) {
                    fprintf(stderr, "Error: log(x) is undefined for x <= 0.\n", x);
                    return 0;
                }
                int push = push_doubleStack(&doubleStack, log10(x));
                if (push == 1) {
                    return 0;
                }
            }
            else if (strncmp(postfixTokenList->array[i]->pLexemmeStart, "exp", postfixTokenList->array[i]->length) == 0) {
                double x = pop_doubleStack(&doubleStack);
                int push = push_doubleStack(&doubleStack, exp(x));
                if (push == 1) {
                    return 0;
                }
            }
            else {
                fprintf(stderr, "Error: Unknown function.\n");
                return 0;
            }
        }

    }

    double finalAnswer = pop_doubleStack(&doubleStack);
    free(doubleStack.array);

    return finalAnswer;

}









