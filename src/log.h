#ifndef CHIP8_LOG_H_INCLUDED
#define CHIP8_LOG_H_INCLUDED 1

#include <stdio.h>

#define LOG_I(fmt, ...) printf("INFO: " fmt "\n", ##__VA_ARGS__);
#define LOG_W(fmt, ...) printf("WARN: " fmt "\n", ##__VA_ARGS__);
#define LOG_E(fmt, ...) printf("ERROR: " fmt "\n", ##__VA_ARGS__);

#endif // CHIP8_LOG_H_INCLUDED


