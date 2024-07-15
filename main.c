#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// returns the arg list for a command
char **split_command(char command[]) {
  char **result = NULL;
  char *curr = strtok(command, " "); // pointer to current token
  int num_spaces = 0;

  while (curr) { // while curr != NULL
    // add 1 char to result
    result = realloc(result, sizeof(char *) * (++num_spaces));
    if (result == NULL)
      exit(0); // realoc failed

    result[num_spaces - 1] = curr; // set that char to the current token
    curr = strtok(NULL, " ");      // get next token
  }

  // append null-terminator
  result = realloc(result, sizeof(char *) * (num_spaces + 1));
  if (result == NULL)
    exit(0); // realloc failed
  result[num_spaces] = 0;

  return result;
}

void exec_command(char **arg_list) {
  char *path = arg_list[0];

  // handle builtin commands
  if (strcmp(path, "cd") == 0)
    chdir(arg_list[1]);
  else if (strcmp(path, "exit") == 0)
    exit(0);
  else if (strcmp(path, "help") == 0)
    puts("Yo! This is a shell. If you're still stuck, here's some advice: git gud bro.");
  else if (strcmp(path, "..") == 0)
    chdir("..");
  // execute 3rd party command, e.g. clear
  else {
    // create a clone of this process
    pid_t p = fork();

    // fork failed
    if (p < 0) {
      perror("fork failed");
      exit(0);
    }

    // parent
    else if (p > 0) {
      wait(NULL); // wait for child to finish
      return;     // continue parent process
    }

    // child
    else {
      execvp(path, arg_list);

      // this means execvp failed, as it did not change the process
      printf("unknown command\n");

      // kill child process
      exit(0);
    }
  }
}

int main() {
  while (1) {
    // print prompt
    printf("\033[0m\033[0;33m>\033[0m ");

    // get input
    char input[1024];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0'; // remove newline

    // execute
    char **arg_list = split_command(input);
    exec_command(arg_list);

    free(arg_list);
  }

  return 0;
}
