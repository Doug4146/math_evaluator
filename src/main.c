#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <errno.h>

#include "errors.h"
#include "lex.h"
#include "parser.h"



int main(int argc, char *argv[]) {


    // To benchmark the performance of the executable
    LARGE_INTEGER frequency, start, end;
    double elapsedTime;
    QueryPerformanceFrequency(&frequency); // Get the high-resolution counter's frequency (ticks per second)
    QueryPerformanceCounter(&start); // Start timing

    // Check for incorrect program call
    if (argc < 2) {
        fprintf(stderr, "\nError: Incorrect usage. Correct usage: .\\math_evaluator.exe \"expression\".\n\n");
        return ERROR_INVALID_PROGRAM_USAGE;
    }
    else if (argc > 2) {
        fprintf(stderr, "\nError: Incorrect usage. Maximum of one expression allowed.\n\n");
        return ERROR_INVALID_PROGRAM_USAGE;
    }


    //-----------------------------------------------------------------------------------------------------------//
    //-----------------------------------------  MAIN LOGIC BEGINS  ---------------------------------------------//
    //-----------------------------------------------------------------------------------------------------------//


    // Begin the lexical analysis part

    // Create and initialize instance of TokenList
    TokenList tokenList;
    int initTokenList = init_tokenList(&tokenList);
    if (initTokenList == 1) {
        fprintf(stderr, "Fatal error: token list could not be created.\n\n");
        return ERROR_FATAL_FUNCTION_CALL;
    }

    // Perform lexical analysis on input string (argv[1])
    int lexerOutput = lexical_analyzer(argv[1], &tokenList);
    if (lexerOutput == 1) {
        fprintf(stderr, "Fatal error: lexical analysis could not be run.\n\n");
        free_tokenList_memory(&tokenList);
        return ERROR_FATAL_FUNCTION_CALL;
    }

    // Print the tokenList 
    int printTokenListOutput = print_tokenList(&tokenList);
    if (printTokenListOutput == 1) {
        fprintf(stderr, "Fatal error: token list could not be printed.\n\n");
        free_tokenList_memory(&tokenList);
        return ERROR_FATAL_FUNCTION_CALL;
    }


    // Begin parsing parting

    // Create and initialize an instance of StackTokenList to represent postfix form of the lexer TokenList.
    StackTokenList postfixTokenList;
    int initPostfixList = init_StackTokenList(&tokenList, &postfixTokenList);
    if (initPostfixList == 1) {
        fprintf(stderr, "Fatal error: postfix token list could not be initialized.\n\n");
        return ERROR_FATAL_FUNCTION_CALL;
    }

    // Carry out the shunting yard algorithm to parse the lexer token list into RPF
    int parser = shunting_yard_algorithm(&tokenList, &postfixTokenList);
    if (parser == 1) {
        fprintf(stderr, "Fatal error: parser could not be run.\n\n");
        free_tokenList_memory(&tokenList);
        free_stackTokenList_memory(&postfixTokenList);
        return ERROR_FATAL_FUNCTION_CALL;
    }

    // Print the tokenList 
    int printPostfixTokenList = print_stackTokenList(&postfixTokenList);
    if (printPostfixTokenList == 1) {
        fprintf(stderr, "Fatal error: token list could not be printed.\n\n");
        free_tokenList_memory(&tokenList);
        free_stackTokenList_memory(&postfixTokenList);
        return ERROR_FATAL_FUNCTION_CALL;
    }



    // Free all memory
    free_tokenList_memory(&tokenList);
    free_stackTokenList_memory(&postfixTokenList);



    //-----------------------------------------------------------------------------------------------------------//
    //-----------------------------------------  MAIN LOGIC ENDS  -----------------------------------------------//
    //-----------------------------------------------------------------------------------------------------------//



    QueryPerformanceCounter(&end); // End timing
    elapsedTime = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart; // Calculate elapsed time in seconds
    printf("Execution Time: %.8f seconds\n\n", elapsedTime); // Print execution time



    return 0;
}