#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <string.h>
#include <unistd.h>

#include "include/queue.h"
#include "include/miniz.h"

#define COMPRESS_TREADS 2

char path[PATH_MAX];

typedef struct Job
{
    char *path;
    char *content;
} JobNode;

typedef struct MutexJobs
{
    pthread_mutex_t *mutex;
    pthread_cond_t *notEmpty;
    int isEmpty;
    int isRunning;
    Queue jobs;
} MutexJobs;

typedef struct ReaderArgs
{
    char *path;
    MutexJobs *jobs;
} ReaderArgs;

typedef struct CompressArgs
{
    int id;
    MutexJobs *jobs;
} CompressArgs;

pthread_t *compressorThreads[COMPRESS_TREADS];
CompressArgs *compressorArgs[COMPRESS_TREADS];

MutexJobs *initMutexJobs()
{
    MutexJobs *mj = (MutexJobs *)malloc(sizeof(MutexJobs));

    if (mj)
    {
        mj->jobs = queue_create();
        if (!mj->jobs)
        {
            perror("Initialisierung der Queue fehlgeschlagen!");
            exit(EXIT_FAILURE);
        }

        mj->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        if (pthread_mutex_init(mj->mutex, NULL))
        {
            perror("Mutex konnte nicht angelegt werden!");
            exit(EXIT_FAILURE);
        }

        mj->notEmpty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
        if (pthread_cond_init(mj->notEmpty, NULL))
        {
            perror("Bedingungsvariable konnte nicht angelegt werden!");
            exit(EXIT_FAILURE);
        }

        mj->isRunning = 1;
        mj->isEmpty = 1;
    }
    else
    {
        perror("Initialisierung der Queue fehlgeschlagen!");
        exit(EXIT_FAILURE);
    }
    puts("Queue Initialisierung erfolgreich");
    return mj;
}

char *getFileContent(char *filepath)
{
    FILE *file;
    char *content;
    int size;

    file = fopen(filepath, "r");
    if (file == NULL)
    {
        perror("Datei konnte nicht geöffnet!");
        return NULL;
    }

    int check = 0;
    check += fseek(file, 0, SEEK_END);
    size = ftell(file);
    check += fseek(file, 0, SEEK_SET);

    if (check || (size < 0))
    {
        perror("Datei konnte nicht gelesen werden!");
    }

    content = malloc(size);

    if (content)
    {
        fread(content, size, 1, file);
    }
    else
    {
        perror("Datei konnte nicht gelesen werden!");
    }

    fclose(file);
    sleep(1);
    return content;
}

void *readFiles(void *arg)
{
    puts("Readerthread gestartet!");
    int status = 0;
    ReaderArgs *args = (ReaderArgs *)arg;
    struct RetValues *ret = NULL;
    MutexJobs *mj = args->jobs;

    char *folderpath = args->path;
    DIR *dir;
    struct dirent *file;
    dir = opendir(folderpath);
    while (file = readdir(dir))
    {
        if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, "..") || !strcmp(strrchr(file->d_name, '.'), ".compr"))
        {
        }
        else
        {

            char cat[PATH_MAX];
            strcpy(cat, folderpath);
            strcat(cat, "/");
            strcat(cat, file->d_name);
            char *fullpath = realpath(cat, path);

            if (fullpath)
            {
                JobNode *job = (JobNode *)malloc(sizeof(JobNode));
                job->path = strdup(fullpath);
                job->content = getFileContent(fullpath);

                status = pthread_mutex_lock(mj->mutex);

                status = !queue_insert(mj->jobs, job);
                status = pthread_mutex_unlock(mj->mutex);

                //Debug
                JobNode *test = queue_head(mj->jobs);
                strcat(test->path, " Wurde zur Queue hinzugefügt");
                puts(test->path);

                if (status)
                {
                    perror("Es ist etwas beim hinzufügen des Jobs zur Queue schiefgegangen");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                perror("konnte keinen Absoluten Pfad erstellen");
            }
        }
    }

    status = pthread_mutex_lock(mj->mutex);
    mj->isRunning = 0;
    status = pthread_mutex_unlock(mj->mutex);
    status = pthread_cond_broadcast(mj->notEmpty);

    if (status)
    {
        perror("Fehler beim Senden des Mustexsignals!");
        exit(EXIT_FAILURE);
    }
}

void *compressFile(JobNode *job, int compressThreadId)
{

    //Komprimiere den Inhalt der Datei
    Result *result;
    /*puts("Debug: Starte Komprimierung");*/
    if (job->content != NULL)
    {
        result = compress_string(job->content);
    }
    else
    {
        printf("COMPRESS(%d): Datei hat keinen Inhalt\n", compressThreadId);
        exit(EXIT_FAILURE);
    }
    if (result == NULL)
    {
        perror("Fehler bei der Komprimierung");
        exit(EXIT_FAILURE);
    }
    printf("COMPRESS(%d): Die Datei %s mit einer Groesse von %d Bytes wurde komprimiert und hat jetzt eine Groesse von %d Bytes\n", compressThreadId, job->path, strlen(job->content), result->length);

    //Erstelle eine Datei mit der endung .compr
    char *fileEnd = ".compr";
    char *fileName = malloc(strlen(job->path) + strlen(fileEnd));
    printf("COMPRESS(%d): path: %s Endung: %s\n", compressThreadId, job->path, fileEnd);
    sprintf(fileName, "%s%s", job->path, fileEnd);

    strcpy(fileName, job->path);
    strcpy(fileName, fileEnd);

    printf("COMPRESS(%d): File:%s\n", compressThreadId, fileName);
    FILE *fp = fopen(fileName, "w");

    //Schreibe komprimierten Inhalt in Datei
    fwrite(result->data, sizeof(char), sizeof(char) * (result->length), fp);
    free(result->data);
    free(result);
    fclose(fp);
}

int main(int argc, char *argv[])
{

    if (argc <= 1)
    {
        puts("Kein Pfad angegeben!");
        return EXIT_SUCCESS;
    }

    DIR *dir = opendir(argv[1]);

    if (dir)
    {

        MutexJobs *mj = initMutexJobs();

        // Beginn Zeitmessung //
        time_t start, end;
        double diff;
        time(&start);

        pthread_t *reader = (pthread_t *)malloc(sizeof(pthread_t));
        ReaderArgs *rargs = (ReaderArgs *)malloc(sizeof(ReaderArgs));
        rargs->path = argv[1];
        rargs->jobs = mj;

        if (pthread_create(&reader, NULL, readFiles(rargs), &rargs))
        {
            perror("Readerthread konnte nicht erstellt werden!");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < COMPRESS_TREADS; i++)
        {
            pthread_t *compressor = (pthread_t *)malloc(sizeof(pthread_t));
            compressorThreads[i] = compressor;
            CompressArgs *cargs = (CompressArgs *)malloc(sizeof(CompressArgs));
            compressorArgs[i] = cargs;
            cargs->id = i;
            cargs->jobs = mj;
                
            /*puts("Debug");*/
            if (pthread_create(compressor, NULL, compressFile(cargs->jobs, cargs->id), NULL))
            {
                perror("Compressorthread konnte nicht erstellt werden!");
                exit(EXIT_FAILURE);
            }
            sleep(3);
        }

        pthread_join(reader, NULL);
        puts("Readerthread beendet");
        for (int i = 0; i < COMPRESS_TREADS; i++)
        {
            printf("Warte auf Thread (%d)\n", i);
            pthread_join(*compressorThreads[i], NULL);
            printf("Compressorthread %d beendet\n", i);
        }
        puts("Alle Threads beendet");

        // Ende Zeitmessung //
        time(&end);
        diff = difftime(end, start);
        printf("Laufzeit: %fsec\n", diff);
    }
    else if (ENOENT == errno)
    {
        perror("Das Verzeichnis existiert nicht");
        exit(EXIT_FAILURE);
    }
    else
    {
        perror("Das Verzeichnis konnte nicht geöffnet werden");
        exit(EXIT_FAILURE);
    }
}