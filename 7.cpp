/*
 * Mycale Radcliffe
 * Operating Systems Sec 001
 * 10/23/2020
 * Makes a dynamic amount of producer and consumer threads that will use the same buffer
 * to produce and consume randomly generated numbers and limits the size of the buffer
 */
#include <iostream>
#include <string>
#include <sstream>
#include <condition_variable>
#include <semaphore.h>
#include <queue>
#include <unistd.h>

class Widget {
public:

  int getData() {
    return data;
  }

  void setData(int data) {
    this->data = data;
  }

private:
  int data;
};

/*
 * struct that will hold the shared data needed for each buffer
 * This includes a queue which  is the buffer, a mutex, and a conditional to make sure it's not empty
 */
typedef struct {
  std::queue<Widget> buf; // the buffer
  size_t len; // The max number of items in buffer
  pthread_mutex_t mutex; // needed to add/remove data from the buffer
  pthread_cond_t can_produce; // signaled when items are removed
  sem_t items; // semaphore to keep track of whether a thread can move forward based on items in buffer
} buffer_t;

// produce random numbers
void *producer(void *arg) {
  buffer_t *buffer = (buffer_t *) arg;

  while (true) {
    pthread_mutex_lock(&buffer->mutex);

    while (buffer->buf.size() == buffer->len) {
      pthread_cond_wait(&buffer->can_produce, &buffer->mutex);
    }

    //generates a random number, places it inside of widget object, and inserts into buffer
    int data = rand() % 100;
    Widget widget;
    widget.setData(data);
    printf("Produced: %d\n", widget.getData());

    buffer->buf.push(widget);

    pthread_mutex_unlock(&buffer->mutex);
    //post that an item has been added by updating the semaphore
    sem_post(&buffer->items);
  }
  return NULL;
}

// consume random numbers
void *consumer(void *arg) {
  buffer_t *buffer = (buffer_t *) arg;

  while (true) {
    //waits until there are items in buffer
    sem_wait(&buffer->items);
    pthread_mutex_lock(&buffer->mutex);

    //print the to be consumed data and then remove it from the queue
    printf("Consumed: %d\n", buffer->buf.front().getData());
    buffer->buf.pop();

    pthread_mutex_unlock(&buffer->mutex);
    //signal to the conditional that there is space in the queue to produce
    pthread_cond_signal(&buffer->can_produce);
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  int producerT = std::stoi(argv[1]);
  int consumerT = std::stoi(argv[2]);
  int threadSize = producerT + consumerT;
  size_t bufferLen = std::stoi(argv[3]);

  buffer_t buffer;
  buffer.len = bufferLen;
  buffer.mutex = PTHREAD_MUTEX_INITIALIZER;
  buffer.can_produce = PTHREAD_COND_INITIALIZER;

  sem_init(&buffer.items, threadSize, 0);
  void *status;

  pthread_t threads[threadSize];

  for (int i = 0; i < producerT; i++) {
    pthread_create(&threads[i], NULL, producer, (void *) &buffer);
  }
  for (int i = producerT; i < threadSize; i++) {
    pthread_create(&threads[i], NULL, consumer, (void *) &buffer);
  }

  for (int i = 0; i < threadSize; i++) {
    pthread_join(threads[i], &status);
  }

  exit(0);

}