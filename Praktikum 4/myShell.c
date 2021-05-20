#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <linux/limits.h>
#include <dirent.h>


int isRunning = 1;

char *user;
char path[PATH_MAX];
char *input;

void printInfo(){

    if(getcwd(path, sizeof(path)) == NULL){
        perror("Der Pfad existiert nicht");
    }
    user = getlogin();
    

    printf("%s:~%s$ ",user, path);
}

char *readInput(){
    char *buff;
    buff = (char *)malloc(PATH_MAX * sizeof(char));
    if(buff == NULL){
        EXIT_FAILURE;
    }
    fgets(buff, PATH_MAX * sizeof(char), stdin);
    buff[strcspn(buff, "\n")] = 0;

    return buff;
}

void cdCommand(char* argList[], int listLength){

    if(listLength == 0){
        chdir(getenv("HOME"));
    }else if(listLength == 1){
        chdir(argList[0]);
    }else{
        puts("Zu viele Argumente für den Befehl cd!");
    }
}

void parser(char* buff){
    char *argList[15];
    int counter = 0;
    char *seperator = " ";
    char *subString;
    subString = strtok(buff, seperator);
    char *command = subString;
    subString = strtok(NULL, seperator);
    while(subString != NULL) {
        
        if(strncmp(subString, "%", 1) == 0){
            
            char *env = getenv(subString+1);
            if(env){
                puts(env);
                argList[counter] = env;

            }else{
                perror("Umgebungsvariabel ist nicht erlaubt!");
            }
            
            
        }else if(realpath(subString, path)){
            
            char *ptr = realpath(subString, path);
            argList[counter] = ptr;
            
        }else{
            //Hier könnten weitere paramenter behandelt werden
        }
        subString = strtok(NULL, seperator);
        counter ++;
    }
    argList[counter] = NULL;

    if(strcmp(command, "cd") == 0){
        cdCommand(argList, counter);
        return;
    }else if(strcmp(command, "set") == 0){
        //setCommand(argList, counter);
        return;
    }else if(strcmp(command, "exit") == 0){
        isRunning = 0;
        return;
    }

    //makeABaby(command, counter);

}

int main(void){

    while(isRunning == 1){
        printInfo();
        input = readInput();
        parser(input);
    }
    


}