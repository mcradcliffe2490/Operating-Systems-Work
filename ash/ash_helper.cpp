#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <cstring>
#include <algorithm>
#include <fcntl.h>

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#define GetCurrentDir getcwd
#endif

typedef std::string String_std;

class Ash_Helper {
public:
  static String_std drawHeader() {
    String_std asciiHeader = readAscii("aggieHeader.txt");
    String_std asciiImage = readAscii("ncatLogo.txt");

    return asciiHeader + "\n" + asciiImage + "\n" + "\n";
  }

  //get rid of leading whitespace
  static String_std removeLeadWhitespace(String_std cmd) {
    String_std result;
    try {
      result = cmd.substr(
          cmd.find_first_not_of(" "),
          cmd.size());
    } catch (const std::out_of_range &e) {
      result = " ";
    }
    return result;
  }

  //get the current directory
  static String_std getCurrentDir() {
    char buf[FILENAME_MAX];

    GetCurrentDir(buf, FILENAME_MAX);
    String_std currWorkingDir(buf);
    return currWorkingDir;

  }

  //Changes current directory based on cmd
  static void changeDir(String_std arg) {
    String_std directory = "";

    //removes possible quotations from directory
    for (int i = 0; i < arg.size(); i++) {
      if (arg[i] != '\"') directory += arg[i];
    }

    int completedSuccesfully = chdir(directory.c_str());

    if (completedSuccesfully == -1) {
      std::cout << "Error: that is not a valid directory" << " \n";
    }
  }

  //returns 1 if built in command was executed and returns 0 otherwise
  static int builtInCmd(String_std command) {
    std::vector<String_std> cmdAndArgs = splitCmd(command);

    //checks for exit command and related errors
    if (cmdAndArgs[0] == "exit" && cmdAndArgs.size() == 1) {
      std::cout << "Mycale Radcliffe & Geterry Crawford" << std::endl;
      exit(0);
    } else if (cmdAndArgs[0] == "exit") {
      std::cout << "Error: exit should not have any arguments" << "\n";
      return 1;
    }

    //checks for cd command and related errors
    if (cmdAndArgs[0] == "cd" && cmdAndArgs.size() == 2) {
      changeDir(cmdAndArgs[1]);
      return 1;
    } else if (cmdAndArgs[0].find("cd") != String_std::npos) {
      std::cout << "Error: cd should always have 1 argument only" << "\n";
      return 1;
    }

    if (cmdAndArgs[0] == "path" && cmdAndArgs.size() == 1) {
      std::cout << "PATH=" << getenv("PATH") << "\n";
      return 1;
    } else if (cmdAndArgs[0] == "path") {
      String_std path = "";
      for (int i = 1; i < cmdAndArgs.size(); i++) {
        path += cmdAndArgs[i] + " ";
      }
      setenv("PATH", path.c_str(), 1);
      return 1;
    }

    return 0;
  }

  static int shellCommand(String_std lineCommand) {
    std::vector<String_std> cmdAndArgsVect = splitCmd(lineCommand);
    const char **cmdAndArgs = new const char *[cmdAndArgsVect.size() + 1];
    bool redirectOut = false;
    int redirectIndex;

    const char *command = cmdAndArgsVect[0].c_str();
    //changes the paths into const char *
    String_std usrPath = "/usr/bin/";
    usrPath += command;
    const char *charUsrPath = usrPath.c_str();

    String_std binPath = "/bin/";
    binPath += command;
    const char *charBinPath = binPath.c_str();

    String_std localPath = "/usr/local/bin/";
    localPath += command;
    const char *charLocalPath = localPath.c_str();

    //sets up argument array for execv
    if (cmdAndArgsVect.size() + 1 == 2) {
      //No arguments, but array still needs to be null terminating
      cmdAndArgs[1] = NULL;
    } else {
      for (int i = 1; i < cmdAndArgsVect.size(); ++i) {
        //in case of redirect operator
        if (cmdAndArgsVect[i].compare(">") == 0) {
          cmdAndArgs[i] = NULL;
          redirectOut = true;
          redirectIndex = i;
          break;
        }
        cmdAndArgs[i] = cmdAndArgsVect[i].c_str();
      }
      //end of arguments, array needs to be null terminating unless redirect operator exist
      if (!redirectOut) cmdAndArgs[cmdAndArgsVect.size()] = NULL;
    }

    //section to check if redirect was done correctly
    if (redirectOut) {
      if (cmdAndArgsVect.size() != redirectIndex + 2) {
        std::cout << "that is not the correct format for a redirect command" << "\n";
        return 1;
      }
    }


    pid_t pid = fork();
    int status;

    if (pid == -1) {
      perror("did not fork properly");
      //2 means that the fork failed
      return 2;
    } else if (pid == 0) {

      if (redirectOut) {
        const char *file = cmdAndArgsVect[redirectIndex + 1].c_str();
        int fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

        dup2(fd, 1);   // make stdout go to file
        dup2(fd, 2);   // makes stderr go to the file

        close(fd);
      }

      cmdAndArgs[0] = charBinPath;
      if (execv(charBinPath, (char **) cmdAndArgs) != -1) exit(0);

      cmdAndArgs[0] = charUsrPath;
      if (execv(charUsrPath, (char **) cmdAndArgs) != -1) exit(0);

      cmdAndArgs[0] = charLocalPath;
      if (execv(charLocalPath, (char **) cmdAndArgs) != -1) exit(0);

      std::cout << command << ": is not a valid command" << "\n";
      exit(-1);
    }

    waitpid(pid, &status, 0);

    return 1;
  }

  static void batchProgram(String_std lineCommand) {
    std::vector<String_std> cmdAndArgsVect = splitCmd(lineCommand);

    if(cmdAndArgsVect.size() != 2) {
      std::cout << "That is not the correct amount of arguments for this command" << "\n";
      return;
    }

    String_std fileName = cmdAndArgsVect[1];
    std::ifstream file(fileName);
    String_std line;

    while(std::getline(file,line)) {

      if (line.empty()) {
        continue;
      }

      String_std command = removeLeadWhitespace(line);

      if(builtInCmd(command) == 1) {
        continue;
      } else if (shellCommand(command) == 1) {
        continue;
      } else if (shellCommand(command) == 2) {
        std::cout << "fork failed" << "\n";
        continue;
      } else if (shellCommand(command) == 3) {
        std::cout << "waitpid failed" << "\n";
        continue;
      }
    }
  }

private:

  static std::string readAscii(std::string fileName) {
    std::ifstream file(fileName);
    std::string result;
    if (file) {
      std::ostringstream ss;
      ss << file.rdbuf();
      result = ss.str();
    }
    file.close();
    return result;
  }

  /*
   * splits command into vector where first elem is always the command
   * and following elements are always args
   */
  static std::vector<String_std> splitCmd(String_std cmd) {
    std::istringstream s(cmd);
    std::vector<String_std> cmdAndArgs;

    while (s) {
      String_std word;
      s >> word;
      //if word starts with a quotation, keeps whole quoted section together
      if (word.find_first_of("\"") == 0) {
        String_std fullWord = "";
        while (s) {
          if (word.back() == '\"') break;
          fullWord += word + " ";
          s >> word;
        }
        cmdAndArgs.push_back(fullWord + word);
        //otherwise adds any word from the stream that is not whitespace
      } else if (word.find_first_not_of(' ') != String_std::npos) cmdAndArgs.push_back(word);
    };

    return cmdAndArgs;
  }
};




