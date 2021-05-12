#ifndef MINIMINDEN_H
#define MINIMINDEN_H

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>


int countArgs(char *buf);
void parser(char *buf);



#endif
