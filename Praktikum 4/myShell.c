#include <myShell.h>

int isRunning = 1;

char *user;
char path[PATH_MAX];
char *input;

void printInfo()
{

    if (getcwd(path, sizeof(path)) == NULL)
    {
        perror("Der Pfad existiert nicht");
    }
    user = getlogin();

    printf("%s:~%s$ ", user, path);
}

char *readInput()
{
    char *buff;
    buff = (char *)malloc(PATH_MAX * sizeof(char));
    if (buff == NULL)
    {
        EXIT_FAILURE;
    }
    fgets(buff, PATH_MAX * sizeof(char), stdin);
    /*Verhindere einen segmentation fault falls der Nutzer nur Enter drückt */
    if (buff[0] == '\n' && buff[1] == '\0')
    {
    }
    else
    {
        buff[strcspn(buff, "\n")] = 0;
    }

    return buff;
}

void cdCommand(char *argList[], int listLength)
{

    if (listLength == 0)
    {
        chdir(getenv("HOME"));
    }
    else if (listLength == 1)
    {
        chdir(argList[0]);
    }
    else
    {
        puts("Zu viele Argumente für den Befehl cd!");
    }
}

void setCommand(char *argList[], int listLength)
{
    /*Zum debuggen. Wenn 4 Parameter angegeben werden printe alle existierenden Umgebungsvariablen*/
    extern char **environ;
    if (listLength == 4)
    {
        for (char **env = environ; *env != 0; env++)
        {
            char *thisEnv = *env;
            printf("%s\n", thisEnv);
        }
    }
    else
    {
        int result = putenv("Foo=123");
        if (result == 0)
        {
            printf("Umgebungsvariable erfolgreich gesetzt\n");
            printf("Wert der gesetzten Umgebungsvariablen: %s\n", getenv("Foo"));
        }
        else
        {
            printf("Fehler beim setzen der Umgebungsvariablen\n");
        }
    }
}

void makeChild(char *command, char *argList[])
{
    pid_t p = fork();
    int status;
    /* Erzeuge Kind */
    if (p == (pid_t)0)
    {
        execvp(command, argList);
    }
    /* Elternprozess wartet auf Kinderprozess */
    else if (p > (pid_t)0)
    {
        p = waitpid(p, &status, WNOHANG);
        /*Wartet auf Error*/
        if (p == -1)
        {
            perror("wait() Error");
            isRunning = 0;
        }
        while (p == 0)
        {
            p = waitpid(p, &status, WNOHANG);
        }
    }
    else
    {
        printf("Fehler!");
        isRunning = 0;
    }
}

void parser(char *buff)
{
    char *argList[10];
    int counter = 0;
    char *seperator = " ";
    char *subString;
    subString = strtok(buff, seperator);
    char *command = subString;
    subString = strtok(NULL, seperator);
    while (subString != NULL)
    {

        if (strncmp(subString, "%", 1) == 0)
        {

            char *env = getenv(subString + 1);
            if (env)
            {
                puts(env);
                argList[counter] = env;
            }
            else
            {
                perror("Umgebungsvariable ist nicht erlaubt!\n");
            }
        }
        else if (realpath(subString, path))
        {

            char *ptr = realpath(subString, path);
            argList[counter] = ptr;
        }
        else
        {
            //Hier könnten weitere paramenter behandelt werden
        }
        subString = strtok(NULL, seperator);
        counter++;
    }
    argList[counter] = NULL;

    if (strcmp(command, "cd") == 0)
    {
        cdCommand(argList, counter);
        return;
    }
    else if (strcmp(command, "set") == 0)
    {
        setCommand(argList, counter);
        return;
    }
    else if (strcmp(command, "exit") == 0)
    {
        isRunning = 0; //Beendet die Whileschleife und damit das Programm
        return;
    }

    makeChild(command, argList);
}

int main(void)
{

    while (isRunning == 1)
    {
        printInfo();
        input = readInput();
        parser(input);
    }
}