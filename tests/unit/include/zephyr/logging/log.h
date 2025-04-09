#pragma once

#include <stdio.h>

#define LOG_MODULE_REGISTER(X)
#define LOG_ERR(...)         \
    do {                     \
        printf("INF: ");     \
        printf(__VA_ARGS__); \
        puts("");            \
    } while (0)
#define LOG_INF(...)         \
    do {                     \
        printf("INF: ");     \
        printf(__VA_ARGS__); \
        puts("");            \
    } while (0)
