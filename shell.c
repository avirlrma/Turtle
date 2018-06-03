#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "history_stack.c"

#define BUFFER_LENGTH   80
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"


char *readLine(){
	/* used this method earlier because buffer size had to be extended everytime
	char line[BUFFER_LENGTH];
	fgets(line,BUFFER_LENGTH,stdin)*/
	char *line = NULL;
	ssize_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	return line;
}


/* Code for parse taken from https://brennan.io/2015/01/16/write-a-shell-in-c/ */
char** parse(char* line){
   int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n"); //when printing to non-standard output (stdout)
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;

}

int launch(char** argv){
	int cmd_length=0;
	int bg = 0;
	for(int i=0;argv[i]!=NULL;i++){
		//printf("%s\n",argv[i]);
		cmd_length++;
	}

	if(strcmp(argv[cmd_length-1],"$")==0){
		bg=1;
		argv[cmd_length-1] =NULL;
	}

	int pid = fork();

	if(pid<0){
		fprintf(stderr,"Failed creation of child\n");
		return 0;
	}

	else if(pid==0){
		execvp(argv[0],argv);
	}

	else{
		if(bg==0)
			wait(0);
	}

	return 1;
}

int execute(char** argv){
	if(argv[0]==NULL)
		return 0;
	//execute command from history
	int cmd_length=0;
	for(int i=0;argv[i]!=NULL;i++){
		cmd_length++;
	}

	if (strcmp(argv[0],"!")==0){
		int cmd_number=0;

		if(strcmp(argv[1],"!")){
			cmd_number=1;
		}

		else{
			cmd_number = *argv[1];

		}
		char **command = history[CURRENT_SIZE-cmd_number];
		save(command);
		return launch(command);
	}


	/*here code of commands that are built-in in the shell
	 to be invoked should be present*/
	// NO BUILT_IN FUNCTIONS AS OF NOW.
	save(argv);
	return launch(argv);
}


void loopTillCMD(){
	char line[BUFFER_LENGTH];
	int status;
	do{
		// this is a clear and simple vulnerability (if an attacker may control the input)
		printf(">");
		char *cmd = readLine();
		char** argv = parse(cmd);
		status = execute(argv);
	}while(status);
	return;
}

int main(){
	loopTillCMD();
	return 0;
}