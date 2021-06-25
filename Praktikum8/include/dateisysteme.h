#ifndef DATEISYSTEME_H
#define DATEISYSTEME_H

#include <sys/types.h>

#define IOIN 0
#define IOOUT 1
#define IOERR 2

#define SQUEEZE_LEAD 11
#define SQUEEZE_END 10

#define BUFFER_SIZE 20

#define _POSIX_C_SOURCE 200112L

void printSecondHalfPrintFirstHalf (const int);
off_t getFileSize(const int);
int writeFromBuffer(const int, const char*, size_t);
void writeFile(const int input, const int output);
void squeezeFile(const int input, const int output);

#endif //DATEISYSTEME_H