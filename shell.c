#include "shell.h"
#define MAX_TOKENS 512
void execute_pipeline(char **commands, int num_commands);
int tokenization(char *line, char *tokens[]);
bool run(char *line);
void chdirt(char *token) {
    if (chdir(token) == -1) {
        perror("cd");
    }
}

int main() {
    char line[512];

    while (1) {
        printf("%% ");
        memset(line,0,sizeof(char));
	fgets(line, sizeof(line)-1, stdin);
        line[strlen(line) - 1] = '\0'; 
	if(run(line)==false)
		break;
}

    return 0;
}

void execute_pipeline(char **commands, int num) {
    int pipes[num - 1][2];

    for (int i = 0; i < num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][0]);
                close(pipes[i - 1][1]);
            }

            if (i < num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            char *tokens[MAX_TOKENS];
            int count = 0;
	    char* path[3] = {"/bin/","/usr/bin/","/usr/local/bin/"};
 
            char paths[30];
	    char *token = strtok(commands[i], " ");
            while (token != NULL) {
                tokens[count++] = token;
                token = strtok(NULL, " ");
            }
            tokens[count] = NULL;
	    for(int i=0;i<3;i++)
	 {
 	memset(paths,0x00,sizeof(paths));
 	strcpy(paths,path[i]);
 	strcat(paths,tokens[0]);
	if(access(paths,F_OK)==0)
	{
	break;
	}
	}
	    execv(paths, tokens);
            exit(1);
        }
    }

    for (int i = 0; i < num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < num; i++) {
        wait(NULL);
    }
}

int tokenization(char *line, char *tokens[]) {
    int count = 0;
	if(strstr(line,"|")){
    char *token = strtok(line, "|");
    while (token != NULL) {
        tokens[count++] = token;
        token = strtok(NULL, "|");
    }
    tokens[count] = NULL;
    return count;
}
else{
char *token = strtok(line, "\t\r\n");
     while (token != NULL) {
         tokens[count++] = token;
         token = strtok(NULL, "\t\r\n");
     }
     tokens[count] = NULL;
     return count;

}
}
bool run(char *line)
{
     char *commands[MAX_TOKENS];
         int num = tokenization(line, commands);
 	
	 if (strcmp(line, "exit") == 0) {
             return false;
          }
	       
 if (strcmp(commands[0], "cd") == 0) {
        if (num > 1) {
            chdirt(commands[1]);
        } else {
            printf("디렉토리를 지정하세요.\n");
        }
        return true;
    }
     execute_pipeline(commands, num);
 memset(commands, '\0',512);
return true;
}

