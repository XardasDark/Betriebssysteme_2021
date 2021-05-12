#include "include/miniMinden.h"
char *input;
char cwd[PATH_MAX];
int running = 1;

void printInfo()
{
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd() error");
    }
    char *name;
    name = getlogin();
    printf("\n %s:~%s$ ", name, cwd);
}

char *readInput()
{
    char* buff;
    buff = (char *)malloc(PATH_MAX * sizeof(char));
    scanf("%[^\n]", buff);
    while(getchar() != '\n');
    return buff;
}

void makeABaby(char *command, char *argList[])
{
	pid_t p = fork();
	int status;
	/* Child */
	if (p == (pid_t)0){
		execvp(command,argList);
	}
	/* Parent wait for child */
	else if (p > (pid_t)0){
		p = waitpid(p, &status, WNOHANG);
		/*wait error*/
		if(p == -1){
			perror("wait() error");
			running = 0;
		}
		while(p == 0){
			p = waitpid(p, &status, WNOHANG);
		}
	}
	else
	{
        	printf("Fehler!");
		running = 0;
	}
}

bool startsWith(const char *pre, const char *str){
	size_t lenpre = strlen(pre);
	size_t lenstr = strlen(str);
	return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

void parseCDCommand(char* argList[], int length){
	if(length == 1){
		chdir(getenv("HOME"));
	}else if(length == 2){
		chdir(argList[1]);
	}else{
		printf("cd: too many arguments.");
	}
}

void parseSetCommand(char* argList[], int length){
	extern char** environ;
	if(length == 1){
		for(char **env = environ; *env != 0; env++){
			char *thisEnv = *env;
			printf("%s\n", thisEnv);
		}
	}else{
		for(int i = 1; i < length; i++){
			char *buffer = argList[i];
			char *varName = strtok(buffer, "=");
			char *varValue = strtok(NULL, "=");
			if(varName == NULL || varValue == NULL){
				printf("Invalid format to set environment variable. Needed: KEY=VALUE");
			}else{
				setenv(varName, varValue, 0);
				printf("Added environment variable: %s = %s\n", varName, varValue);
			}
		}
	}
}

void parser(char* buf){
//	printf("getenv():%s\n",getenv("SHELL"));
//	printf("realpath:%s\n",realpath("ls"));
	char *argList[10];
	int counter = 0;
	char *delimiters = " ";
	char *ptr = strtok(buf, delimiters);
	char *command = ptr;


	while(ptr != NULL){
		char asPath[PATH_MAX];
		//The String is an environment variable
		if (startsWith("%", ptr)){
			char* asEnv = getenv(ptr+1);
			if(asEnv){
				argList[counter] = asEnv;
			}else{
				perror("Invalid environment variable entered, skipping it...");
			}
		}else if(realpath(ptr, &asPath)){
			argList[counter] = &asPath;
		}else{
			argList[counter] = ptr;
		}
		ptr = strtok(NULL, delimiters);
		counter++;
	}
	argList[counter] = NULL;
//	for(int i = 0; i < counter;i++){
//		printf("%s\n",argList[i]);
//	}
	if(strcmp(command, "exit") == 0){
		running = 0;
		return;
	}else if(strcmp(command, "set") == 0){
		parseSetCommand(argList, counter);
		return;
	}else if(strcmp(command, "cd") == 0){
		parseCDCommand(argList, counter);
		return;
	}

	makeABaby(command, argList);
}

int main()
{
	while(running == 1){
		printInfo();
		input = readInput();
		parser(input);
	}
	return 0;
}
