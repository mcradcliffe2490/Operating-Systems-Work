/*
 * Mycale Radcliffe
 * Operating Systems Sec 001
 * 10/31/2020
 * Recreates a classical sync problem called the Reader-Writers problem
 * Where readers and writers perform operations on a buffer.
 * Multiple readers can operate at one time,
 * but when a writer thread begins work,
 * it is the only thread allowed access to the buffer
 */

#include <iostream>
#include <string>
#include <sstream>
#include <semaphore.h>
#include <unistd.h>
#include <vector>

typedef std::string String_std;

typedef struct {
  pthread_mutex_t lock;
  sem_t roomEmpty;
  sem_t turnstile;
  int readers;
  pthread_cond_t bufEmpty;
  std::vector<int> buf;
} buffer_t;

void *reader(void *arg) {
  while (true) {
    buffer_t *buffer = (buffer_t *) arg;
    while (buffer->buf.size() == 0) {
      pthread_cond_wait(&buffer->bufEmpty, &buffer->lock);
    }
    /*
     * If the reader calls wait on the turnstile, then readers will come in one after another
     * However, if the writer calls wait on the turnstile
     * all readers that come after must wait on turnstile
     */
    sem_wait(&buffer->turnstile);
    sem_post(&buffer->turnstile);

    pthread_mutex_lock(&buffer->lock);

    //increments readers and if thread is first reader "turns on the lights"
    buffer->readers += 1;
    if (buffer->readers == 1) sem_wait(&buffer->roomEmpty);
    pthread_mutex_unlock(&buffer->lock);

    //Critical Section for the readers where read work is done
    int randIndex = rand() % buffer->buf.size();

    pthread_mutex_lock(&buffer->lock);
    std::cout << "Read: " << buffer->buf[randIndex] << "\n";
    pthread_mutex_unlock(&buffer->lock);

    //decrements readers as they leave and if thread is the last reader "turn the lights off"
    pthread_mutex_lock(&buffer->lock);
    buffer->readers -= 1;
    if (buffer->readers == 0) sem_post(&buffer->roomEmpty);
    pthread_mutex_unlock(&buffer->lock);
  }
  return NULL;
}

void *writer(void *arg) {
  while (true) {
    buffer_t *buffer = (buffer_t *) arg;
    //turnstile like semaphore that helps with writer starvation
    sem_wait(&buffer->turnstile);

    sem_wait(&buffer->roomEmpty);

    // Critical Section for the writer where write work is done
    int random = rand() % 100;
    buffer->buf.push_back(random);
    std::cout << "Wrote: " << random << "\n";
    sem_post(&buffer->turnstile);
    sem_post(&buffer->roomEmpty);
  }
  return NULL;
}


int main() {
  //buffer initialization
  buffer_t buffer;
  buffer.lock = PTHREAD_MUTEX_INITIALIZER;
  buffer.readers = 0;
  buffer.bufEmpty = PTHREAD_COND_INITIALIZER;
  sem_init(&buffer.roomEmpty, 1, 1);
  sem_init(&buffer.turnstile, 1, 1);

  int numReaders = (rand() % 6) + 1;
  int numWriters = (rand() % 3) + 1;
  int totalThreads = numReaders + numWriters;
  pthread_t threads[totalThreads];

  void *status;

  //Reader and Writer thread creation
  for (int i = 0; i < numReaders; i++) {
    pthread_create(&threads[i], NULL, reader, (void *) &buffer);
  }
  for (int i = numReaders; i < totalThreads; i++) {
    pthread_create(&threads[i], NULL, writer, (void *) &buffer);
  }

  for (int i = 0; i < totalThreads; i++) {
    pthread_join(threads[i], &status);
  }

  exit(0);

}

