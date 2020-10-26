#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> 

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/


void execute(char** tokens, int background){
	for(int i=0;tokens[i]!=NULL;i++){
//			printf("found token %s\n", tokens[i]);
//		printf("%s\n", tokens[i+1]);
		pid_t pid;
		int ret=1;
		int status;
		pid=fork();
		if (pid==-1){
			printf("forking error\n");
			exit(EXIT_FAILURE);
		}
		else if (pid==0){
		//	char * argv_list[] = {"ls","-lart","/home",NULL};
			// The return value is -1 
	      	//printf("Your string: %s\n", tokens[0]);
		
			if (tokens[i+1]==NULL){
				if ((strcmp(tokens[0],"cd"))==0){
	
				}
				else{
					execvp(tokens[0],tokens);
					if (execvp(tokens[0],tokens)<0){
						printf("ERROR: Invalid command\n");
					}
				}
			} 
	      	exit(0); 
		}
		else{
			if (background==0){
				if (waitpid(pid,&status,0)>0){
					if (WIFEXITED(status) && !WEXITSTATUS(status)){
						//printf("program execution successfull\n");
						//printf("found token %s\n", tokens[0]);
						if ((strcmp(tokens[0],"cd"))==0){
							char path[MAX_TOKEN_SIZE];
						    strcpy(path,tokens[1]);

						    char cwd[MAX_TOKEN_SIZE];
						    if(tokens[1][0] != '/')
						    {// true for the dir in cwd
						        getcwd(cwd,sizeof(cwd));
						        strcat(cwd,"/");
						        strcat(cwd,path);
						        chdir(cwd);
						    }else{//true for dir w.r.t. /
						        chdir(tokens[1]);
						    }
						}
					} 
					else if (WIFEXITED(status) && WEXITSTATUS(status)){
						if(WEXITSTATUS(status)==127){
							printf("execv failed\n");
						}
						else{
							printf("non-zero status returned\n");
						}
					}
					else{
						printf("program didn't terminate normally\n");
					}
				}
				else{
					printf("waitpid() failed");
				}
			}
		}
	}
   
	// Freeing the allocated memory	
	for(int i=0;tokens[i]!=NULL;i++){
		free(tokens[i]);
	}
	free(tokens);

}



void sigquit() 
{ 
    printf("My DADDY has Killed me!!!\n"); 
    exit(0); 
} 

char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}


int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;           
	char **mera = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));   
	int i;
	int killall[64];
	int kill_index=0;

	for (int i=0;i<64;i++){
		killall[i]=0;
	}

	FILE* fp;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp < 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}
//	_Bool check_cd=0;
	while(1) {			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if(argc == 2) { // batch mode
			if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;	
			}
			line[strlen(line) - 1] = '\0';
		} else { // interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		printf("Command entered: %s\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
   
       //do whatever you want with the commands, here we just print them
		int background = 0;
		int size;
		int cmd=-1;
	    for(size=0; tokens[size]!=NULL; size++)
	    {
	    	if ((strcmp(tokens[size],"&&"))==0){
	    		cmd=0;
	    		break;
	    	}
	    	else if ((strcmp(tokens[size],"&&&"))==0){
	    		cmd=1;
	    		break;
	    	}
	    	else if ((strcmp(tokens[size],"&"))==0){
	    		background=1;
	    		cmd=2;
	    		tokens[size]=NULL;
	    		break;
	    	}
	    }
/*		if (cmd==2){
			background=1;
		//	printf("%s\n", "YES");
		//	printf("%s\n", tokens[size]);
			tokens[size]=NULL;
			execute(tokens,background,0);
		}*/
		if (cmd==0){
			int j=0;
			int size=0;
			while (tokens[size]!=NULL){
				mera[j]=tokens[size];
				if ((strcmp(tokens[size],"&&"))==0){
					mera[size]=NULL;
					j = -1;
					execute(mera,background);
				}
				j=j+1;
				size=size+1;
				
			}
			mera[j]=tokens[size];
			execute(mera,background);
		}
		else if (cmd==3){
/*			pid_t pid;
			int ret=1;
			int status;
			pid=fork();*/
		}
		else{
//		execute(tokens,background,0);
		


		for(i=0;tokens[i]!=NULL;i++){
//			printf("found token %s\n", tokens[i]);
			pid_t pid;
			int ret=1;
			int status;
			pid=fork();
			if (pid==-1){
				printf("forking error\n");
				exit(EXIT_FAILURE);
			}
			else if (pid==0){
			//	char * argv_list[] = {"ls","-lart","/home",NULL};
				// The return value is -1 
		      	//printf("Your string: %s\n", tokens[0]);
				if (cmd==2){
					killall[kill_index]=getpid();
				}
				if (tokens[i+1]==NULL){
					if ((strcmp(tokens[0],"cd"))==0){
		
					}
					else if ((strcmp(tokens[0],"exit"))==0){
						for (int i=0;i<kill_index;i++){
							kill(killall[i], SIGQUIT);
						}	
						kill_index=0;	
					}
					else{
						execvp(tokens[0],tokens);
						if (execvp(tokens[0],tokens)<0){
							printf("ERROR: Invalid command\n");
						}
					}
				} 
		      	exit(0); 
			}
			else{
				if (background==0){
					if (waitpid(pid,&status,0)>0){
						if (WIFEXITED(status) && !WEXITSTATUS(status)){
							//printf("program execution successfull\n");
							//printf("found token %s\n", tokens[0]);
							if ((strcmp(tokens[0],"cd"))==0){
								char path[MAX_TOKEN_SIZE];
							    strcpy(path,tokens[1]);

							    char cwd[MAX_TOKEN_SIZE];
							    if(tokens[1][0] != '/')
							    {// true for the dir in cwd
							        getcwd(cwd,sizeof(cwd));
							        strcat(cwd,"/");
							        strcat(cwd,path);
							        chdir(cwd);
							    }else{//true for dir w.r.t. /
							        chdir(tokens[1]);
							    }
							}
						} 
						else if (WIFEXITED(status) && WEXITSTATUS(status)){
							if(WEXITSTATUS(status)==127){
								printf("execv failed\n");
							}
							else{
								printf("non-zero status returned\n");
							}
						}
						else{
							printf("program didn't terminate normally\n");
						}
					}
					else{
						printf("waitpid() failed");
					}
				}
			}
		}

	
	for(int i=0;tokens[i]!=NULL;i++){
		free(tokens[i]);
		free(mera[i]);
	}
	free(tokens);
	free(mera);
}
}
	return 0;
}


