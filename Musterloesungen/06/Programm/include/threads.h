#ifndef MULTITHREADS_H
#define MULTITHREADS_H

#include <dirent.h>
#include "mutexQueue.h"

typedef struct Job {
    char *path;
    char *content;
} Job;

typedef struct ReaderArgs {
    DIR *directory;
    char *basename;
    MutexQueue *queue;
} ReaderArgs;

typedef struct CompressArgs {
    MutexQueue *queue;
    int id;
} CompressArgs;

void *readerThread(void *arguments);
void *compressThread(void *arguments);
void deleteJob(Job *job);

char *getFullPath(const char *base, const char *name);
char *get_file_content(const char *filename);
void getFile(void *arguments);
void cmprFile(Job *job, int);
char *get_filename_ext(const char *filename);

#endif

