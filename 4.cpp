/*
 * Mycale Radcliffe
 * Operating Systems Sec 001
 * 10/3/2020
 * Program creates two separate threads and uses them both to count from 0 to 10
 * and then exits the program
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <pthread.h>

//lock for critical section
std::mutex printMutex;

//Counts from 0 to 10 and prints out the result with the thread number
void * countUp(void *tNum) {

  for (int i = 0; i <= 10; ++i) {
    printMutex.lock();
    std::cout << "t" << tNum << "num: " << i << std::endl;
    printMutex.unlock();
  }
  return NULL;
}


int main() {
  pthread_t threads[2];
  void *status;
  //creates two separate threads and waits for them to complete their task
  //std::thread t1(&countUp, 1);
  pthread_create(&threads[0], NULL, countUp, (void *)(1));

  pthread_create(&threads[1], NULL, countUp, (void *)(2));

  //std::thread t2(&countUp, 2);

  pthread_join(threads[0], &status);
  pthread_join(threads[1], &status);

  std::cout << "exiting program";
  exit(0);


}
