/*
 * Mycale Radcliffe
 * Operating Systems Sec 001
 * 9/19/2020
 * Program checks to see if a fork was created,
 * identifies the parent process,
 * and then executes a command on the child process
 * and prints out the output of that command on the parent process
 * an example would be running the command ls
 */

#include<iostream>
#include<unistd.h>
#include <bits/stdc++.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

//Method for splitting a string into an array
std::vector<std::string> split(std::string inputString) {
  std::stringstream stringStream(inputString);
  std::istream_iterator<std::string> begin(stringStream);
  std::istream_iterator<std::string> end;
  std::vector<std::string> resultStrings(begin, end);
  return resultStrings;
}

//Method for converting a string to a char*
const char *convertString(std::string & str) {
  return str.c_str();
}

int main() {
  //forks the program and saves the result
  int fds[2];
  pipe(fds);
  pid_t pid = fork();
  char buffer[128];
  std::string parentResult = "";

  //The case for if the fork fails
  if (pid == -1) {
    perror("did not fork properly");
    exit(1);

    //The case for executing a program in the child
  } else if (pid == 0) {
    //Gets user input string and stores it
    std::string userInput;
    std::cout << "Input your commands and arguments" << std::endl;
    std::cin >> userInput;

    std::vector<std::string> userInputVect = split(userInput);
    std::vector<char*> userInputChar;

    for( auto const& c : userInputVect) {
      userInputChar.emplace_back(const_cast<char*>(c.c_str()));
    }

    //Push Null to terminate in execvp
    userInputChar.push_back(NULL);

    //dups the standard output for the child process and runs command
    close(fds[0]);
    dup2(1, fds[1]);
    close(fds[1]);

    execvp(userInputChar[0], userInputChar.data() );
    exit(-1);

    //Parent Proccess code
  } else {
    std::cout << "This is the parent process" << std::endl;

    //We are only reading from the pipe no input needed
    close(fds[1]);
    //Reads from the pipe and outputs on the screen
    FILE* pipe = fdopen(fds[0], "r ");

    while(!feof(pipe)) {
      if(fgets(buffer, 128, pipe) != NULL) {
        parentResult += buffer;
      }
    }
    pclose(pipe);
    std::cout << parentResult;
    exit(0);
  }
}

