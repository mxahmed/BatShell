/*
 * BatShell
 * 
 * a simple shell written as a learining experince
 * and yes the name is because of batman
 *
 * copywrites: 
 * 	<Ahmed Maher El-mitwally> github account: @mxahmed
 *	<Ahmed Maher Mohammed>
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define L_BUFSIZE 1024
#define T_BUFSIZE 64
#define LINE_DELMITERS " \t\a\r\n"

int bsh_cd(char **argv);
int bsh_help(char **argv);
int bsh_exit(char **argv);

char *built_ins[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func_pt[]) (char **) = {
  &bsh_cd,
  &bsh_help,
  &bsh_exit
};

int builtins_num(){
  return sizeof(built_ins) / sizeof(char*);
}

int bsh_cd(char **args){
  if(args[1] == NULL){
   fprintf(stderr, "bsh: expect arguments for to 'cd'\n");
  } else {
    if(chdir(args[1]) != 0) {
      perror("bsh");
    }
  }

  return 1;
}

int bsh_help(char **argv) {
  int i;
  printf("Batshell\n");
  printf("built in commands:\n");
  for (i=0; i < builtins_num(); ++i){
   printf("    %s\n", built_ins[i]);
  }
  printf("use the man command for info on other programs");
  return 1;
}

int bsh_exit(char **argv) {
  return 0;
}

int bsh_launch(char **args){
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    if(execvp(args[0], args) == -1){
      perror("bsh");
    }
    exit(-1);
  } else if (pid < 0) {
    perror("bsh");
  } else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
return 1;
}

int bsh_execute(char **args){
  int i;

  if(args[0] == NULL){
    return 1;
  }

  for (i = 0; i < builtins_num(); ++i){
    if(strcmp(args[0], built_ins[i]) == 0){
      return (*builtin_func_pt[i]) (args);
    }
  }

  return bsh_launch(args);

}

char *bsh_readline() {
  int c;
  int pos = 0;
  int bufsize = L_BUFSIZE;
  char *buffer = malloc(sizeof(char) * bufsize);

  if (!buffer) {
    fprintf(stderr, "bsh: Line buffer allocation error");
    exit(-1);
  }

  while (1) {
    c = getchar();
    if (c == EOF || c == '\n') {
      buffer[pos] = '\0';
      return buffer;
    } else {
      buffer[pos] = c;
    }
    pos++;

    if(pos >= bufsize) {
      bufsize += L_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "bsh: Line buffer reallocation error");
        exit(-1);
      }
    }
  }
}

char **bsh_tokenize(char *line) {
  int bufsize = T_BUFSIZE;
  int pos = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "bsh: tokens buffer allocation error!");
    exit(-1);
  }
  token = strtok(line, LINE_DELMITERS);
  while(token != NULL) {
     tokens[pos]=token;
     pos++;
     if ( pos >= bufsize){
       bufsize += T_BUFSIZE;
       tokens = realloc(tokens, bufsize);
       if(!tokens){
         fprintf(stderr, "bsh: tokens reallocation error!");
	 exit(-1);
       }
     }
     token = strtok(NULL, LINE_DELMITERS);
  }
  tokens[pos] = NULL;
  return tokens;
}

char *get_prompt(void) {
    int bufsize = 128; 
    char *path = malloc(bufsize * sizeof(char));
    getcwd(path, bufsize);
    return path;
}

void bsh_loop (void){
  char *prompt;
  char *line;
  char **args;
  int status;

  do {
    prompt = get_prompt();
    printf("%s: ", prompt);

    line = bsh_readline();
    args = bsh_tokenize(line);
    status = bsh_execute(args);

    free(line);
    free(args);
  } while (status);

}

int main (int argc, char **argv) {
  printf("Batshell batman's shell\n\n");
  bsh_loop();
  return 0;
}
