#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "threads.h"
#include "miniz.h"

const char ext[] = "compr";

void *readerThread(void *arguments)
{

    ReaderArgs *args = (ReaderArgs *)arguments;
    MutexQueue *queue = args->queue;

    printf("Reader Thread gestartet\n");

    struct dirent *dir;
    while ((dir = readdir(args->directory)) != NULL)
    {

        struct stat path_stat;
        char *fullPath = getFullPath(args->basename, dir->d_name);
        stat(fullPath, &path_stat);
        /* Nur für reguläre und unkomprimierte Dateien */
        if ((S_ISREG(path_stat.st_mode) != 0) && (strcmp(get_filename_ext(fullPath), ext) != 0))
        {

            printf("READER: Bearbeite %s\n", dir->d_name);

            /* Job erstellen */
            Job *job = malloc(sizeof(Job));
            if (!job)
            {
                printf("readerThread: Fehler beim Speicherallozieren\n");
                exit(EXIT_FAILURE);
            }
            job->path = strdup(fullPath);
            job->content = get_file_content(fullPath);

            /* Sperre holen */
            if (pthread_mutex_lock(queue->mutex))
            {
                perror("Queue konnte nicht gesperrt werden");
                return NULL;
            }

            /* Job kann hinzugefügt werden */
            if (!(queue_insert(queue->queue, job)))
            {
                printf("readerThread: Konnte Job nicht zu Queue hinzufügen\n");
            }
            queue->empty = 0;


            /* Aufheben der Sperre*/
            if (pthread_mutex_unlock(queue->mutex))
            {
                perror("Fehler beim Entsperren der Queue");
                return NULL;
            }

	    printf("READER: Sende notEmpty Signal\n");
            /* Senden des Signals */
            if (pthread_cond_signal(queue->notEmpty))
            {
                perror("Fehler beim Senden eines Signals: ");
            }
        }

        free(fullPath);

        /* Nur zum Testen Anfang */
        /*while (!queue_empty(queue->queue))
        {
            //Job *test = queue_head(queue->queue);
            //printf("%s\n", test->path);
            //queue_delete(queue->queue);
	    getFile(arguments);
        }*/
        /* Nur zum Testen Ende */
    }

    sleep(1);

    /* Sperre holen */
    if (pthread_mutex_lock(queue->mutex))
    {
       perror("Queue konnte nicht gesperrt werden");
       return NULL;
    }

    queue->running = 0;
    printf("READER: Sende running=0 Signal\n");


    /* Aufheben der Sperre*/
    if (pthread_mutex_unlock(queue->mutex))
    {
       perror("Fehler beim Entsperren der Queue");
       return NULL;
    }


    if(pthread_cond_broadcast(queue->notEmpty)){
	perror("Fehler beim Senden des running=0 Signals");
    }
}



void *compressThread(void *arguments)
{

	CompressArgs *args = (CompressArgs *)arguments;

	printf("COMPRESS(%d): Started compress Thread\n", args->id);
	//sleep(2);
	//printf("Still in compress Thread (%d)\n", args->id);
	getFile(arguments);

}




    void deleteJob(Job * job)
    {
        free(job->path);
        free(job->content);
        free(job);
    }


    // TODO: Diese beiden Methoden könnte man eventuell auslagern...
    char *getFullPath(const char *base, const char *name)
    {
        char *realBase = realpath(base, NULL);
        char *fullPath = malloc(strlen(realBase) + strlen(name) + 2);
        if (!fullPath)
        {
            printf("Fehler beim Holen des Speichers\n");
            exit(EXIT_FAILURE);
        }

        strcpy(fullPath, realBase);
        strcat(fullPath, "/");
        strcat(fullPath, name);
        free(realBase);
	return fullPath;
    }

char *get_file_content(const char *filename)
{

    char *buffer = "";

    /* Öffne Datei zum Lesen */
    FILE *infile = fopen(filename, "r");

    /* Abbruch, falls Datei nicht existiert */
    if (infile == NULL)
    {
        perror("Fehler beim Lesen der Datei: ");
        return buffer;
    }

    /* Suche Dateiender */
    if (fseek(infile, 0, SEEK_END))
    {
        perror("Fehler beim Lesen der Datei: ");
    }

    /* Länge der Datei*/
    long length = ftell(infile);
    if (length < 0)
    {
        perror("Fehler beim Lesen der Datei: ");
    }

    /* Zurück zum Anfang */
    if (fseek(infile, 0, SEEK_SET))
    {
        perror("Fehler beim Lesen der Datei: ");
    }

    /* Datei einlesen */
    buffer = malloc(length);
    if (buffer)
    {
        fread(buffer, 1, length, infile);
    }
    else
    {
        perror("Fehler beim Lesen der Datei: ");
    }

    /* Datei schließen */
    fclose(infile);

    /* Um langsame Festplatte zu simulieren */
    sleep(1);

    /* Rückgabe */
    return buffer;
}

void getFile(void *arguments){
	//printf("in der Funktion getFile\n");
	CompressArgs *args = (CompressArgs *) arguments;
	MutexQueue *queue = args->queue;

	int haveJob = 0;
	Job *job;
	//Solange bis ein Job aus der Que geholt wurde
	while(queue->running || queue->empty == 0){

		//Que sperren
		if(pthread_mutex_lock(queue->mutex)){
			perror("Queue konnten icht gesperrt werden");
			return;
		}

		while(queue->running && queue->empty){
			printf("COMPRESS(%d): Warte auf Einlesen einer Datei...\n", args->id);
			int ret = pthread_cond_wait(queue->notEmpty, queue->mutex);
			printf("COMPRESS(%d): Signal erhalten.\n", args->id);
		}
		printf("COMPRESS(%d): Hole naechste Datei...\n", args->id);

		if(queue->running == 0 && queue->empty){ //Thread beenden, wenn reader vorbei und alle Dateien komprimiert sind
			//Sperre Aufheben
			if(pthread_mutex_unlock(queue->mutex)){
				perror("Fehler beim Entsperren der Queue");
				return;
			}
			printf("COMPRESS(%d): Beende Thread.\n", args->id);
			pthread_exit(NULL);
		}

		//Wenn die Que nicht leer ist
		job = (Job*) queue_head(queue->queue);

		haveJob = 1;
		if(job == 0){
			haveJob = 0;
		}else{
			if(!queue_delete(queue->queue)){
				perror("Element konnte nicht aus Queue entfernt werden");
			}
		}

		if(queue_empty(queue->queue)){
			queue->empty = 1;
		}

		//Sperre Aufheben
		if(pthread_mutex_unlock(queue->mutex)){
			perror("Fehler beim Entsperren der Queue");
			return;
		}

		cmprFile(job, args->id);
		sleep(3);
		printf("COMPRESS(%d): Datei komprimieren fertig. Running? -> %s, Empty -> %s\n", args->id, queue->running ? "True" : "False", queue->empty ? "True" : "False");
	}
	printf("COMPRESS(%d): Beende Thread.\n", args->id);
	pthread_exit(NULL);
}

void cmprFile(Job *job, int compressThreadId){

	//Komprimiere den inhalt der datei
	Result *result;
	if(job->content != NULL){
		result = compress_string(job->content);
	}else{
		printf("COMPRESS(%d): Datei hat keinen inhalt\n", compressThreadId);
		return;
	}

	if(result == NULL){
		perror("Fehler bei der Komprimierung");
		return;
	}
	printf("COMPRESS(%d): Die Date %s mit einer groesse von %d Bytes wurde komprimiert und hat jetzt eine groesse von %d Bytes\n", compressThreadId, job->path, strlen(job->content), result->length );

	//Erstelle eine Datei mit der endung .compr
	char *fileEnd = ".compr";
	char *fileName = malloc(strlen(job->path) + strlen(fileEnd));
	printf("COMPRESS(%d): path: %s endung: %s\n", compressThreadId, job->path, fileEnd);
	sprintf(fileName, "%s%s", job->path, fileEnd);

	//strcpy(fileName, job->path);
	//strcpy(fileName, fileEnd);

	printf("COMPRESS(%d): File:%s\n", compressThreadId, fileName);
	FILE *fp = fopen(fileName,"w");

	//Schreibe die kompremierung in die datei
	fwrite(result->data, sizeof(char), sizeof(char)*(result->length), fp);
	free(result->data);
	free(result);
	fclose(fp);
}

char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}
