#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void error(char *msg) {
  printf("\033[32mError: %s\033[0m", msg);
  exit(0);
}

// returns the arg list for a command
char **split_command(char command[]) {
  char **result = malloc(sizeof(char *));
  int total = 1; // total # of sub-commands
  bool is_in_str = false;

  result[0] = &command[0]; // result[0] -> address of command[0]

  // loop over the command array
  for (size_t i = 0; i < (sizeof(&command) / sizeof(char)); i++) {
    if (command[i] == ' ' && !is_in_str) {
      command[i] = 0; // set to null-terminator

      if (command[i + 1] == '"') // don't set new char * to "
        continue;

      result = realloc(result, sizeof(char *) * (++total)); // add 1 char * to result
      if (result == NULL)
        error("realloc failed");           // realloc failed
      result[total - 1] = &command[i + 1]; // set new char * to next token
    } else if (command[i] == '"') {
      is_in_str = !is_in_str;

      command[i] = 0;

      if (command[i + 1] == 0) {
        printf("asdf?");
        break;
      }

      if (is_in_str) {
        result = realloc(result, sizeof(char *) * (++total)); // add 1 char * to result
        if (result == NULL)
          error("realloc failed");           // realloc failed
        result[total - 1] = &command[i + 1]; // set new char * to next token
      }
    }
  }

  for (size_t i = 0; i < total; i++) {
    printf("'%s'\n", result[i]);
  }

  // append null-terminator
  result = realloc(result, sizeof(char *) * (total + 1));
  if (result == NULL)
    error("realloc failed"); // realloc failed
  result[total] = 0;

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
      printf("unknown command: %s\n", arg_list[0]);

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
