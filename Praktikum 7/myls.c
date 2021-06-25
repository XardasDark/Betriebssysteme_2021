#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"

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

int printFileStat(struct dirent *file){
    struct stat filestat;
    char *fullpath = realpath(file->d_name, NULL);
 
    if(lstat(fullpath, &filestat) == 0){
        struct tm *tm;
        char buf[200];
        tm = localtime(&filestat.st_ctime);
        strftime(buf, sizeof(buf), "%b %d %H:%M", tm);
        printProtection(&filestat);

        if(oFlag && gFlag){
            
            printf(" %2ld %6ld %s",filestat.st_nlink, filestat.st_size, buf);
        }else{
           
            struct passwd *user;
            struct group *group;
            
            user = getpwuid(filestat.st_uid);
            group = getgrgid(filestat.st_gid);
            printf(" %2ld %s %s %6ld %s",filestat.st_nlink, user->pw_name, group->gr_name , filestat.st_size, buf);
        }
        char *hasFileEnd = strrchr(file->d_name, '.');
        if(hasFileEnd && (strcmp(hasFileEnd, ".c") == 0)){
            printf(GRN " %s\n" RESET, file->d_name);

        }else if(!(S_ISDIR(filestat.st_mode)) && ((filestat.st_mode & S_IXUSR) || (filestat.st_mode & S_IXGRP) || (filestat.st_mode & S_IXOTH))){
            printf(RED " %s\n" RESET, file->d_name);

        }else{
            printf(" %s\n", file->d_name);
        }
        
        return 1;
    }else{
        ("Konnte keine Details ausgeben!");
        return 0;
    }
}

int printDir(char *path){
    struct dirent *file;
    DIR *dir = opendir(path);
    chdir(path);

    if(dir){
        while (file = readdir(dir)){
            if(file != NULL){
                if(aFlag && lFlag){
                    printFileStat(file);
                    
                }else if(aFlag){
                    printf("%s\n", file->d_name);
                
                }else if(lFlag){
                    if(!ignoreFolder(file)){
                        printFileStat(file);
                    }
                }else{
                    if(!ignoreFolder(file)){
                        printf("%s\n", file->d_name);
                    }
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
    printDir(path);
}