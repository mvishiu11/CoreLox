#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ANSI color codes
#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"

// Fun random facts and/or quotes
const char* funFacts[] = {
    "Programming tip: There is no code faster than no code.",
    "Quote: \"In the middle of difficulty lies opportunity.\" - Albert Einstein",
    "Quote: \"The only limit to our realization of tomorrow is our doubts of today.\" - FDR",
    "Did you know? A group of flamingos is called a 'flamboyance'.",
    "Fun fact: Honey never spoils!",
};

// Function to print a random fun fact
void printFunFact() {
    srand(time(NULL));
    int randomIndex = rand() % (sizeof(funFacts) / sizeof(funFacts[0]));
    printf(COLOR_MAGENTA "%s\n" COLOR_RESET, funFacts[randomIndex]);
}

// Greeting message
void greetUser() {
    printf(COLOR_GREEN "Welcome to the Carbonlox REPL! 🎉\n" COLOR_RESET);
    printFunFact();
    printf(COLOR_YELLOW "Type ':help' for help, or ':exit' to quit.\n" COLOR_RESET);
}

// Fun goodbye message
void goodbyeMessage() {
    printf(COLOR_CYAN "Thanks for using Carbonlox! 🚀 See you next time!\n" COLOR_RESET);
    printFunFact();
}

// Handle special REPL commands like ":help" or ":exit"
int handleReplCommand(const char* line) {
    if (strcmp(line, ":exit") == 0) {
        return 1;
    } else if (strcmp(line, ":help") == 0) {
        printf(COLOR_YELLOW "Carbonlox REPL Commands:\n" COLOR_RESET);
        printf(COLOR_GREEN "  :help   " COLOR_RESET " - Show this help message\n");
        printf(COLOR_GREEN "  :exit   " COLOR_RESET " - Exit the REPL\n");
        return 0;
    } else {
        return 0;
    }
}

// REPL (Read-Eval-Print Loop)
void repl() {
    greetUser();

    char line[1024];
    for (;;) {
        printf(COLOR_BLUE "carbonlox> " COLOR_RESET);

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        if (line[0] == '\n') {
            continue;
        }

        line[strcspn(line, "\n")] = 0;

        if (handleReplCommand(line)) {
            break;
        }

        line[strcspn(line, "\n")] = 0;

        printf(COLOR_MAGENTA "You typed: \"%s\"\n" COLOR_RESET, line);
    }

    goodbyeMessage();
}

// Run a file
void runFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, COLOR_RED "Could not open file \"%s\".\n" COLOR_RESET, path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, COLOR_RED "Not enough memory to read \"%s\".\n" COLOR_RESET, path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, COLOR_RED "Could not read file \"%s\".\n" COLOR_RESET, path);
        exit(74);
    }

    buffer[bytesRead] = '\0';
    fclose(file);

    // Interpret the file here (you can replace this with actual interpreter logic)
    printf(COLOR_MAGENTA "Running file: \"%s\"\n" COLOR_RESET, path);
    printf(COLOR_CYAN "File contents:\n%s\n" COLOR_RESET, buffer);

    free(buffer);
}

int main(int argc, const char* argv[]) {
    initVM();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        fprintf(stderr, COLOR_RED "Usage: carbonlox [path]\n" COLOR_RESET);
        exit(64);
    }

    freeVM();
    return 0;
}