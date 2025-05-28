#include <stdio.h>
#include <stdarg.h>
#include "Utils.h"

void printMessage(const char* str, ...) {
    va_list args;
    va_start(args, str);

    const char* current = str;

    // Loop through all arguments until NULL is encountered
    while (current != NULL) {
        printf("%s ", current);
        current = va_arg(args, const char*);
    }

    va_end(args);

    // Print a newline after the message
    printf("\n");
}
