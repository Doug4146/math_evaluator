#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <errno.h>
#include "errors.h"
#include "lex.h"


static const int INITIAL_TOKEN_CAPACITY = 10;  // Initial capacity for the tokenArr in lexer_Output instance


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

    tokenList.maxCapacity = INITIAL_TOKEN_CAPACITY;
    tokenList.position = -1;
    tokenList.array = malloc(tokenList.maxCapacity*sizeof(Token*));
    if (tokenList.array == NULL) {
        fprintf(stderr, "Fatal error: memory could not be allocated.\n\n");
        return ERROR_MEMORY_ALLOCATION_FAILURE;
    }

    // Perform lexical analysis on input string (argv[1])
    int lexerOutput = lexical_analyzer(argv[1], &tokenList);
    if (lexerOutput == 1) {
        fprintf(stderr, "Fatal error: lexical analysis could not be ran.\n\n");
        int freeTokenListResult = free_tokenList_memory(&tokenList);
        return ERROR_FATAL_FUNCTION_CALL;
    }

    // Print the tokenList 
    int printTokenListOutput = print_tokenList(&tokenList);
    if (printTokenListOutput == 1) {
        fprintf(stderr, "Fatal error: token list could not be printed.\n\n");
        int freeTokenListResult = free_tokenList_memory(&tokenList);
        return ERROR_FATAL_FUNCTION_CALL;
    }


    // Free all memory
    int freeTokenListResult = free_tokenList_memory(&tokenList);



    //-----------------------------------------------------------------------------------------------------------//
    //-----------------------------------------  MAIN LOGIC ENDS  -----------------------------------------------//
    //-----------------------------------------------------------------------------------------------------------//



    QueryPerformanceCounter(&end); // End timing
    elapsedTime = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart; // Calculate elapsed time in seconds
    printf("Execution Time: %.8f seconds\n\n", elapsedTime); // Print execution time



    return 0;
}