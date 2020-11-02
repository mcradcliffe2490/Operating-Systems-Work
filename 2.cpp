#include<iostream>
#include<unistd.h>
#include "2.h"

int main() {
  pid_t pid = fork();
  char *cmdArgs[3];

  if (pid == -1) {
    perror("did not fork properly");
    exit(1);
  } else if (pid == 0) {
      cmdArgs[0] = (char *) "-w";
      cmdArgs[1] = (char *) "1.cpp";
      cmdArgs[2] = NULL;
      execvp("wc", cmdArgs);
  } else {
    std::cout << "This is the parent process" << std::endl;

    exit(0);
  }
}

