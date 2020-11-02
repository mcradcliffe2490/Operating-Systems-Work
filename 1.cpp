#include <iostream>
#include <unistd.h>

int main() {
    pid_t p_id = fork();

    if (p_id == 0 ) {
        std::cout << "I am the Child my id is " << getpid() << std::endl;
    } else if (p_id > 0) {
        std::cout << "I am the Parent my id is " << getppid() << std::endl;
    } else {
        std::cout << "Fork failed" << std::endl;
    }
    return 0;
}