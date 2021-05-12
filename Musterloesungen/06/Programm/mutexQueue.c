#include <stdio.h>
#include "mutexQueue.h"

/* Schlange muss für gesicherten Zugriff besonders initialisiert werden */

MutexQueue *mutexQueueInit(void) {
    MutexQueue *mutexQueue = (MutexQueue *) malloc(sizeof(MutexQueue));
    if (mutexQueue) {

        /* Queue */
        mutexQueue->queue = queue_create();
        if ( !(mutexQueue->queue) ) {
            printf("Fehler beim Anlegen der Queue");
            exit(EXIT_FAILURE);
        }

        /* Mutex */
        mutexQueue->mutex = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
        if (mutexQueue->mutex) {
            if (pthread_mutex_init (mutexQueue->mutex, NULL)) {
                perror("Initialisierung Mutex: ");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("Fehler beim Anlegen der Mutex");
            exit(EXIT_FAILURE);
        }

        /* Bedingungsvariablen */
        mutexQueue->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
        if (mutexQueue->notEmpty) {
            if (pthread_cond_init (mutexQueue->notEmpty, NULL)) {
                perror("Initialisierung Bedingungsvariable: ");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("Fehler beim Anlegen der Bedingungsvariablen");
            exit(EXIT_FAILURE);
        }

        mutexQueue->empty = 1;
        mutexQueue->running = 1;
    } else {
        printf("Fehler beim Anlegen der synchronisierten Queue");
        exit(EXIT_FAILURE);
    }

    return mutexQueue;
}

/* Löschen der Queue */
void Cleanup(MutexQueue *mutexQueue) {
    pthread_mutex_destroy (mutexQueue->mutex);
    free (mutexQueue->mutex);
    pthread_cond_destroy (mutexQueue->notEmpty);
    free (mutexQueue->notEmpty);
    queue_destroy(mutexQueue->queue);
    free(mutexQueue);
}