#pragma once
#ifndef MY_MACROS_H
#define MY_MACROS_H

#include <stdio.h>
#include <stdlib.h>

// 1. Check for NULL and return 0
#define CHECK_RETURN_0(ptr) \
    do { \
        if ((ptr) == NULL) { \
            return 0; \
        } \
    } while (0)

// 2. Check for NULL, print a message, and return 0
#define CHECK_MSG_RETURN_0(ptr, msg) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "%s\n", (msg)); \
            return 0; \
        } \
    } while (0)

// 3. Free memory, close file, and return 0
#define FREE_CLOSE_FILE_RETURN_0(ptr, file) \
    do { \
        if ((ptr) != NULL) { \
            free((ptr)); \
        } \
        if ((file) != NULL) { \
            fclose((file)); \
        } \
        return 0; \
    } while (0)

// 4. Close file and return 0
#define CLOSE_RETURN_0(file) \
    do { \
        if ((file) != NULL) { \
            fclose((file)); \
        } \
        return 0; \
    } while (0)

#endif // MY_MACROS_H