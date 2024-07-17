#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static char **split_command(char command[]) {
  char **result = malloc(sizeof(char *) * 256);
  int total = 1; // total # of sub-commands
  bool is_in_str = false;
  size_t command_len = strlen(command);

  result[0] = &command[0]; // result[0] -> address of command[0] until null terminator

  // loop over the command array
  for (size_t i = 0; i < command_len; i++) {
    if (command[i] == ' ' && !is_in_str) {
      command[i] = 0; // set to null-terminator

      if (command[i + 1] == '"') // don't set new char * to "
        continue;

      result[++total - 1] = &command[i + 1]; // set new char * to next token
    } else if (command[i] == '"') {
      is_in_str = !is_in_str;

      command[i] = 0; // replace with null terminator

      if (is_in_str) 
        result[++total - 1] = &command[i + 1]; // set new char * to next token
    }
  }

  result[total] = 0; // append null-terminator

  return result;
}

static void exec_command(char **arg_list) {
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

int main(void) {
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
}
