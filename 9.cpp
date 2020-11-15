/*
 * Mycale Radcliffe
 * Operating Systems Sec 001
 * 11/14/2020
 * Recreates the Dinning Philosiphers problem but replaces philosophers
 * with gamers in a living playing a game
 * Threads should take turns using the controller resource,
 * should not block any threads while they are not using resources,
 * prevent deadlock
 * and prevent starvation
 */
#include <iostream>
#include <semaphore.h>
#include <unistd.h>

typedef std::string String_std;

typedef struct {
  sem_t controllers[5];
  sem_t numGamers;
  pthread_mutex_t lock;

  int left(int i) {
    return i;
  }

  int right(int i) {
    return (i + 1) % 5;
  }

  void getControllers(int i) {
    sem_wait(&numGamers); // Semaphore to make sure only 4 Gamers in living room at a time
    sem_wait(&controllers[right(i)]); //Gamer pick up right controller
    sem_wait(&controllers[left(i)]); //Gamer pick up left controller
  }

  void putDownControllers(int i) {
    sem_post(&controllers[right(i)]); //Gamer put down right controller
    sem_post(&controllers[left(i)]); //Gamer put down left controller
    sem_post(&numGamers); //Gamer exits the living room to let another Gamer in
  }

} gamers_t;

void *gamingThread0(void *args) {
  gamers_t *gamers = (gamers_t *) args;
  while (true) {
    pthread_mutex_lock(&gamers->lock);
    srand(1);
    int randomTime = (rand() % 10) + 1;
    pthread_mutex_unlock(&gamers->lock);

    sleep(randomTime); //represents Gamer spectating (or philosopher thinking)

    gamers->getControllers(0); //pick up controllers to play or wait for a controller
    pthread_mutex_lock(&gamers->lock); //mutual exclusion for cout
    std::cout
        << "Thread 0 is gaming with controllers "
        << gamers->left(0)
        << " and "
        << gamers->right(0)
        << "\n";
    pthread_mutex_unlock(&gamers->lock); //mutual exclusion for cout
    gamers->putDownControllers(0); // put down the controllers for another gamer to pick up
  }

}

void *gamingThread1(void *args) {
  gamers_t *gamers = (gamers_t *) args;
  while (true) {
    pthread_mutex_lock(&gamers->lock);
    srand(2);
    int randomTime = (rand() % 10) + 1;
    pthread_mutex_unlock(&gamers->lock);

    sleep(randomTime); //represents Gamer spectating (or philosopher thinking)

    gamers->getControllers(1); //pick up controllers to play or wait for a controller
    pthread_mutex_lock(&gamers->lock); //mutual exclusion for cout
    std::cout
        << "Thread 1 is gaming with controllers "
        << gamers->left(1)
        << " and "
        << gamers->right(1)
        << "\n";
    pthread_mutex_unlock(&gamers->lock); //mutual exclusion for cout
    gamers->putDownControllers(1); // put down the controllers for another gamer to pick up
  }
}

void *gamingThread2(void *args) {
  gamers_t *gamers = (gamers_t *) args;
  while (true) {
    pthread_mutex_lock(&gamers->lock);
    srand(3);
    int randomTime = (rand() % 10) + 1;
    pthread_mutex_unlock(&gamers->lock);

    sleep(randomTime); //represents Gamer spectating (or philosopher thinking)

    gamers->getControllers(2); //pick up controllers to play or wait for a controller
    pthread_mutex_lock(&gamers->lock); //mutual exclusion for cout
    std::cout
        << "Thread 2 is gaming with controllers "
        << gamers->left(2)
        << " and "
        << gamers->right(2)
        << "\n";
    pthread_mutex_unlock(&gamers->lock); //mutual exclusion for cout
    gamers->putDownControllers(2); // put down the controllers for another gamer to pick up
  }
}

void *gamingThread3(void *args) {
  gamers_t *gamers = (gamers_t *) args;
  while (true) {
    pthread_mutex_lock(&gamers->lock);
    srand(4);
    int randomTime = (rand() % 10) + 1;
    pthread_mutex_unlock(&gamers->lock);

    sleep(randomTime); //represents Gamer spectating (or philosopher thinking)

    gamers->getControllers(3); //pick up controllers to play or wait for a controller
    pthread_mutex_lock(&gamers->lock); //mutual exclusion for cout
    std::cout
        << "Thread 3 is gaming with controllers "
        << gamers->left(3)
        << " and "
        << gamers->right(3)
        << "\n";
    pthread_mutex_unlock(&gamers->lock); //mutual exclusion for cout
    gamers->putDownControllers(3); // put down the controllers for another gamer to pick up
  }
}

void *gamingThread4(void *args) {
  gamers_t *gamers = (gamers_t *) args;
  while (true) {
    pthread_mutex_lock(&gamers->lock);
    srand(5);
    int randomTime = (rand() % 10) + 1;
    pthread_mutex_unlock(&gamers->lock);

    sleep(randomTime); //represents Gamer spectating (or philosopher thinking)

    gamers->getControllers(4); //pick up controllers to play or wait for a controller
    pthread_mutex_lock(&gamers->lock); //mutual exclusion for cout
    std::cout
        << "Thread 4 is gaming with controllers "
        << gamers->left(4)
        << " and "
        << gamers->right(4)
        << "\n";
    pthread_mutex_unlock(&gamers->lock); //mutual exclusion for cout
    gamers->putDownControllers(4); // put down the controllers for another gamer to pick up
  }
}

int main() {
  pthread_t threads[5];
  void *status;
  gamers_t gamers;
  //Initialize the semaphores
  sem_init(&gamers.numGamers, 1, 4); //no more than 4 gamers in living room
  gamers.lock = PTHREAD_MUTEX_INITIALIZER;

  for (int i = 0; i < 5; i++) {
    sem_init(&gamers.controllers[i], 1, 1);
  }

  pthread_create(&threads[0], NULL, gamingThread0, (void *) &gamers);
  pthread_create(&threads[1], NULL, gamingThread1, (void *) &gamers);
  pthread_create(&threads[2], NULL, gamingThread2, (void *) &gamers);
  pthread_create(&threads[3], NULL, gamingThread3, (void *) &gamers);
  pthread_create(&threads[4], NULL, gamingThread4, (void *) &gamers);

  for (int i = 0; i < 5; i++) pthread_join(threads[i], &status);

  exit(0);

}

