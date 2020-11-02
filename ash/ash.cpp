#include<iostream>
#include "ash_helper.cpp"
#include <string>
#include <stdlib.h>
/*
 * Mycale Radcliffe & Geterry Crawford
 * Operating Systems Sec 001
 * 9/23/2020
 * Program greets you and waits for user to input command
 * upon inputting "exit" the program ends
 * and the team members names are displayed
 */
typedef std::string String_std;

//method for getting user commands
String_std prompt() {
  String_std cmd;
  std::cout << "ash " + Ash_Helper::getCurrentDir() + " >";
  std::getline(std::cin, cmd);
  return cmd;
}

int main() {
  //draws the header for the aggie shell
  std::cout << Ash_Helper::drawHeader();

  //waits for user input to execute commands and exits upon typing "exit"
  while (1) {
    String_std cmd = prompt();

    //makes sure command is not empty
    if (cmd.empty()) {
      continue;
    }

    String_std command = Ash_Helper::removeLeadWhitespace(cmd);

    if(Ash_Helper::builtInCmd(command) == 1) {
      continue;
    } else {
      std::cout << command << ": is not a command" << "\n";
      continue;
    }

  }
}

