#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 10


char** history[MAX_SIZE];
int CURRENT_SIZE=0;

void showAllHistory(void){
	printf("------current history of commands------\n");
	for(int i=0;i<CURRENT_SIZE;i++){
		printf("%s\n",history[i][0]);
	}
}


void save(char**argv){
	if(CURRENT_SIZE<10){
		history[CURRENT_SIZE] = argv;
		CURRENT_SIZE++;
	}
	else{
		for(int i=1;i<10;i++){
			history[i-1] = history[i];
		}
		history[9] = argv;
	}
}