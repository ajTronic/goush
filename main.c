#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// returns the arg list for a command
char** parse_command(char command[]) {
  char** result = NULL;
  char* curr = strtok(command, " "); // pointer to current token
  int num_spaces = 0;

  while (curr) { // while curr != NULL
    // add 1 char to result
    result = realloc(result, sizeof(char*) * (++num_spaces)); 
    if (result == NULL) exit(0); // realoc failed

    result[num_spaces-1] = curr; // set that char to the current token
    curr = strtok(NULL, " "); // get next token
  }

  // append null-terminator
  result = realloc(result, sizeof(char*) * (num_spaces+1)); 
  if (result == NULL) exit(0); // realloc failed
  result[num_spaces] = 0;

  return result;
}

int main() {
  while (1) {
    // print prompt
    printf("\033[0m\033[0;33m>\033[0m ");

    // get input
    char input[1024];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0'; // remove newline

    // execute command
    pid_t p = fork();
    if (p < 0)
      perror("fork failed");
    else if (p == 0) {
      // child
      char **arg_list = parse_command(input);
      execvp(arg_list[0], arg_list);

      printf("unknown command\n");
      exit(0);
    } else {
      // parent
      wait(NULL); // wait for command to finish
    }
  }

  return 0;
}

