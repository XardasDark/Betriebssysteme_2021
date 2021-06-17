#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"

char pathbuff[PATH_MAX];
int aFlag = 0;
int lFlag = 0;
int gFlag = 0;
int oFlag = 0;

void printProtection(struct stat *filestat){
    printf( (S_ISDIR(filestat->st_mode)) ? "d" : "-");
    printf( (filestat->st_mode & S_IRUSR) ? "r" : "-");
    printf( (filestat->st_mode & S_IWUSR) ? "w" : "-");
    printf( (filestat->st_mode & S_IXUSR) ? "x" : "-");
    printf( (filestat->st_mode & S_IRGRP) ? "r" : "-");
    printf( (filestat->st_mode & S_IWGRP) ? "w" : "-");
    printf( (filestat->st_mode & S_IXGRP) ? "x" : "-");
    printf( (filestat->st_mode & S_IROTH) ? "r" : "-");
    printf( (filestat->st_mode & S_IWOTH) ? "w" : "-");
    printf( (filestat->st_mode & S_IXOTH) ? "x" : "-");
}


int ignoreFolder(struct dirent *file){
    if (!strncmp(file->d_name, ".", 1) || !strcmp(file->d_name, "..")){
        return 1;
    }
    return 0;
}

int printFileStat(struct dirent *file, char *folderpath){
    struct stat filestat;
    char cat[PATH_MAX];
    if(!ignoreFolder(file)){
        strcpy(cat, folderpath);
        strcat(cat, "/");
        strcat(cat, file->d_name);
    }
    char *fullpath = realpath(file->d_name, NULL);
    printf("Fullpath: %s\n", fullpath);
    if(lstat(fullpath, &filestat) == 0){
        struct tm *tm;
        char buf[200];
        tm = localtime(&filestat.st_ctime);
        strftime(buf, sizeof(buf), "%b %d %H:%M", tm);
        printProtection(&filestat);
        printf(" %ld %ld %s %s\n",filestat.st_nlink, filestat.st_size, buf, file->d_name);
        return 1;
    }else{
        ("Konnte keine Details ausgeben!");
        return 0;
    }
    puts("Debug");
}

int printDir(char *path){
    DIR *dir = opendir(path);

    if(dir){
        struct dirent *file;
        while (file = readdir(dir)){
            if(aFlag && lFlag && gFlag && oFlag){
                printFileStat(file, path);

            }else if(aFlag && lFlag){
            
            }else if(aFlag){
                
            }else if(lFlag){

            }else{
                if(!ignoreFolder(file)){
                    printf("%s\n", file->d_name);
                }
            } 
        }
        closedir(dir);


    }else if (ENOENT == errno){
        perror("Das Verzeichnis existiert nicht");
        exit(EXIT_FAILURE);
    }
    else{
        perror("Das Verzeichnis konnte nicht geöffnet werden");
        exit(EXIT_FAILURE);
    }
    
}

int main(int argc, char *argv[]){
    int opt, pathTrue;
    char *path = (char *) malloc(PATH_MAX);
    pathTrue = 0;
 

    while ((opt = getopt(argc, argv, "algo")) != -1){
        switch(opt) {
            case 'a':
                aFlag = 1;
                break;
            case 'l':
                lFlag = 1;
                break;
            case 'g':
                gFlag = 1;
                break;
            case 'o':
                oFlag = 1;
                break;
            default:
                puts("Kein gültiges Argument!");
                abort();
        }
    }

    int i;
    for(i = 1; i < argc; i++){
        if(realpath(argv[i], pathbuff)){
            pathTrue++;
            path = argv[i];
        }
    }

    if(pathTrue == 0){
        getcwd(path, PATH_MAX);
    }else if(pathTrue > 1){
        printf("Zu viele Argumente!");
        return EXIT_FAILURE;
    }
    puts(path);
    printDir(path);
}