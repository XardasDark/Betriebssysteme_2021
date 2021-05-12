#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>

#include "queue.h"
#include "threads.h"
#include "main.h"

//const unsigned int NTHREADS = 4;
pthread_t *compressThreads[NTHREADS];
CompressArgs *compressArgsArr[NTHREADS];


int main(int argc, char *argv[]) {

    /* Anzahl der Argumente testen */
    if( argc < 2 ) {
        printf("Kein Pfad angegeben! - ./main <Pfad>\n");
        return EXIT_SUCCESS;
    }

    DIR *directory = opendir(argv[1]);

    if (directory) {
        MutexQueue *queue = mutexQueueInit();

        /* Starte Zeitmessung */
        time_t start_t, end_t;
        double diff_t;
        time(&start_t);

        /* Reader Thread */
        pthread_t reader;
        ReaderArgs readerArgs;
        readerArgs.directory = directory;
        readerArgs.basename = argv[1];
        readerArgs.queue = queue;
        if (pthread_create(&reader, NULL, readerThread, &readerArgs)) {
            perror("Reader Thread konnte nicht erzeugt werden");
        }

	for (int i = 0; i < NTHREADS; i++){
		pthread_t *compressor = (pthread_t *) malloc(sizeof(pthread_t));
		compressThreads[i] = compressor;
		CompressArgs *compressArgs = (CompressArgs *) malloc(sizeof(CompressArgs));
		compressArgsArr[i] = compressArgs;
		compressArgs->queue = queue;
		compressArgs->id = i;
		if(pthread_create(compressor, NULL, compressThread, compressArgs)){
			perror("Compress Thread konnte nicht erzeugt werden");
		}
	}

        /* Warten auf Ende des Readerthreads */
        pthread_join(reader, NULL);
	printf("MAIN: Reader Thread beendet.\n");
	for(int i = 0; i < NTHREADS; i++){
		printf("MAIN: Warte auf Thread (%d)\n", i);
		pthread_join(*compressThreads[i], NULL);
		printf("MAIN: Thread (%d) ist beendet.\n", i);
	}
        printf("Alle Threads beendet.\n");


        /* Ende Zeitmessung */
        time(&end_t);
        diff_t = difftime(end_t, start_t);
        printf("Laufzeit: %f\n", diff_t);

        /* Speicher freigeben */
        Cleanup(queue);

    } else if (ENOENT == errno) {
        perror("Das Verzeichnis existiert nicht");
        return EXIT_FAILURE;
    } else {
        perror("Verzeichnis konnte nicht geöffnet werden");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

}
