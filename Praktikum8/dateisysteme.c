#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h> 
#include <unistd.h>
#include <fcntl.h>

#include "dateisysteme.h"

void printSecondHalfPrintFirstHalf(const int filedescriptor)
{
    char buffer[BUFFER_SIZE];
    long int count;
    long int size = getFileSize(filedescriptor);

    /* Prüfe Größe */
    if (size < 0)
    {
        char *error = "Dateigröße konnte nicht ermittelt werden\n";
        write(IOERR, error, strlen(error));
        return;
    }

    /* Gebe zweite Hälfte aus */
    if (lseek(filedescriptor, size / 2, SEEK_CUR) < (off_t)0)
    {
        char *error = "Fehler beim Setzen des Dateizeigers\n";
        write(IOERR, error, strlen(error));
        return;
    }
    count = size - size / 2;
    while (count > 0)
    {
        ssize_t nRead = read(filedescriptor, buffer, (size_t)MIN(BUFFER_SIZE, count));
        if (nRead < 0)
        {
            char *error = "Fehler beim Lesen\n";
            write(IOERR, error, strlen(error));
            return;
        }
        if (writeFromBuffer(IOOUT, buffer, (size_t)nRead) < 0)
        {
            char *error = "Fehler bei der Ausgabe\n";
            write(IOERR, error, strlen(error));
            return;
        }
        count -= nRead;
    }

    /* Gebe erste Hälfte aus */
    if (lseek(filedescriptor, 0, SEEK_SET) < (off_t)0)
    {
        char *error = "Fehler beim Setzen des Dateizeigers\n";
        write(IOERR, error, strlen(error));
        return;
    }
    count = size / 2;
    while (count > 0)
    {
        ssize_t nRead = read(filedescriptor, buffer, (size_t)MIN(BUFFER_SIZE, count));
        if (nRead < 0)
        {
            char *error = "Fehler beim Setzen des Dateizeigers\n";
            write(IOERR, error, strlen(error));
            return;
        }
        if (writeFromBuffer(IOOUT, buffer, (size_t)nRead) < 0)
        {
            char *error = "Fehler bei der Ausgabe\n";
            write(IOERR, error, strlen(error));
            return;
        }
        count -= nRead;
    }

    if (write(IOOUT, "\n", 1) < 0)
    {
        char *error = "Fehler bei der Ausgabe\n";
        write(IOERR, error, strlen(error));
        return;
    }

    /* Rewind */
    if (lseek(filedescriptor, 0, SEEK_SET) < (off_t)0)
    {
        char *error = "Fehler beim Zurücksetzen des Dateizeigers\n";
        write(IOERR, error, strlen(error));
        return;
    }
}

/* Gibt die Größe eine Datei in Bytes zurück */
off_t getFileSize(const int filedescriptor)
{
    struct stat fileStat;
    if (fstat(filedescriptor, &fileStat) < 0)
    {
        return (off_t)-1;
    }
    else
    {
        return fileStat.st_size;
    }
}

/* Schreibt Bytes von einem Buffer */
int writeFromBuffer(const int filedescriptor, const char* buffer, size_t nBytes) {
    while(nBytes > 0) {
        ssize_t written = write(filedescriptor, buffer, nBytes);
        if (written < 0) {
            char *error = "Fehler beim Schreiben\n";
            write(IOERR, error, strlen(error));
            return -1;
        }
        nBytes -= written;
    }
    return 0;
}

void writeFile(const int input, const int output){
	int size = getFileSize(input);
	char buff[size];
	ssize_t nBytes = read(input, buff, (size_t)size);
	
	if(nBytes < 0){
		char *error = "Fehler beim lesen\n";
		write(IOERR, error, strlen(error));
	}
	
	writeFromBuffer(output, buff, nBytes);
		
}

void squeezeFile(const int input, const int output){
	/* Buffer für die ersten 11 und die letzten 10 Characters + EOF */
	char buffLead[SQUEEZE_LEAD];
	char buffEnd[SQUEEZE_END + 1];
	
	/* Liest die ersten 11 Characters */
	ssize_t nBytes = read(input, buffLead, (size_t)SQUEEZE_LEAD);
	if(nBytes < 0){
		char *error = "Fehler beim Lesen der Datei\n";
		write(IOERR, error, strlen(error));
		return;
	}
	
	
	/* Gehe an das Ende der Datei */
	if (lseek(input, -SQUEEZE_END - 1, SEEK_END) < (off_t)0)
	{
		char *error = "Fehler beim Setzen des Dateizeigers\n";
		write(IOERR, error, strlen(error));
		return;
	}
	
	/* Liest die letzten 10 Characters und das Ende der Datei*/
	nBytes = read(input, buffEnd, (size_t)SQUEEZE_END+1);
	if(nBytes < 0){
		char *error = "Fehler beim Lesen der Datei\n";
		write(IOERR, error, strlen(error));
		return;
	}
	
	/* Schreibt die ersten 11 Characters in die output Datei*/
	int ret = writeFromBuffer(output, buffLead, strlen(buffLead));
	if(ret < 0){
		return;
	}
	
	/* Schreibt die letzten 10 Characters + EOF in die output Datei*/
	ret = writeFromBuffer(output, buffEnd, strlen(buffEnd));
	if(ret < 0){
		return;
	}
}

int main(int argc, char *argv[])
{

    /* Anzahl der Argumente testen */
    if (argc < 3)
    {
        char *usage = "Nutzung: ./main <Quelle> <Ziel>\n";
        if (write(IOOUT, usage, strlen(usage)) != strlen(usage))
        {
            char *error = "Fehler bei der Ausgabe\n";
            write(IOERR, error, strlen(error));
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    /* Öffnen der Eingabe-Datei */
    char *src = argv[1];
    int filedescriptor_in = open(src, O_RDONLY);
    if (filedescriptor_in < 0)
    {
        char *error = "Datei konnte nicht geöffnet werden\n";
        write(IOERR, error, strlen(error));
        return EXIT_FAILURE;
    }

    char *message = "Zweite Haelfte / Erste Haelfte:\n";
    write(IOOUT, message, strlen(message));

    printSecondHalfPrintFirstHalf(filedescriptor_in);

    /* Öffnen der Ausgabe-Datei */
    char *dest = argv[2];
    int filedescriptor_out = open(dest, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (filedescriptor_out < 0)
    {
        char *error = "Datei konnte nicht geöffnet werden\n";
        write(IOERR, error, strlen(error));
        return EXIT_FAILURE;
    }

    //writeFile(filedescriptor_in, filedescriptor_out);
    

    message = "Inhalt der Ausgabedatei nach dem Kopieren:\n";
    write(IOOUT, message, strlen(message));

    squeezeFile(filedescriptor_in, filedescriptor_out);

    /* Schließen der Eingabe-Datei */
    if (close(filedescriptor_in))
    {
        char *error = "Eingabe-Datei konnte nicht geschlossen werden\n";
        write(IOERR, error, strlen(error));
        return EXIT_FAILURE;
    }
    /*  Schließen der Ausgabe-Datei */
    if (close(filedescriptor_out))
    {
        char *error = "Ausgabe-Datei konnte nicht geschlossen werden\n";
        write(IOERR, error, strlen(error));
        return EXIT_FAILURE;
    }

    /* Öffnen der Ausgabe-Datei */
    char *srcP = argv[2];
    filedescriptor_in = open(srcP, O_RDONLY);
    if (filedescriptor_in < 0)
    {
        char *error = "Datei konnte nicht geöffnet werden\n";
        write(IOERR, error, strlen(error));
        return EXIT_FAILURE;
    }

    writeFile(filedescriptor_in, IOOUT);

    /*  Schließen der Ausgabe-Datei */
    if (close(filedescriptor_in))
    {
        char *error = "Ausgabe-Datei konnte nicht geschlossen werden\n";
        write(IOERR, error, strlen(error));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

