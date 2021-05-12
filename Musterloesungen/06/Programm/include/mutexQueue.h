#ifndef SYNCHRONISATION_QUEUE_H
#define SYNCHRONISATION_QUEUE_H

#include "queue.h"
#include <pthread.h>

typedef struct MutexQueue
{
    pthread_mutex_t *mutex;
    pthread_cond_t *notEmpty;
    int empty;
    int running;
    Queue queue;
} MutexQueue;

MutexQueue *mutexQueueInit(void);
void Cleanup(MutexQueue *mutexQueue);

#endif