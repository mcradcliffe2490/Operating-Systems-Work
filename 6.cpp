/*
 * Mycale Radcliffe
 * Operating Systems Sec 001
 * 10/17/2020
 * Makes a dynamic amount of producer and consumer threads that will use the same buffer
 * to produce and consume randomly generated numbers
 */
#include <iostream>
#include <string>
#include <sstream>
#include <condition_variable>
#include <queue>

//Widget class that will hold our data
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
  std::queue<Widget> b; // the buffer
  pthread_mutex_t mutex; // needed to add/remove data from the buffer
  pthread_cond_t can_consume; // signaled when items are added
} buffer_t;

// produce random numbers
void *producer(void *arg) {
  buffer_t *buffer = (buffer_t *) arg;

  while (true) {

    pthread_mutex_lock(&buffer->mutex);

    //generates a random number, places it inside of widget object, and inserts into buffer
    int data = rand() % 100;
    Widget widget;
    widget.setData(data);
    printf("Produced: %d\n", widget.getData());

    buffer->b.push(widget);

    pthread_mutex_unlock(&buffer->mutex);
    pthread_cond_signal(&buffer->can_consume);
  }
  return NULL;
}

// consume random numbers
void *consumer(void *arg) {
  buffer_t *buffer = (buffer_t *) arg;

  while (true) {

    pthread_mutex_lock(&buffer->mutex);

    //makes consumer wait if the buffer is empty
    while (buffer->b.size() == 0) {
      pthread_cond_wait(&buffer->can_consume, &buffer->mutex);
    }

    //prints the current data to be consumed and pops it from buffer
    printf("Consumed: %d\n", buffer->b.front().getData());
    buffer->b.pop();

    pthread_mutex_unlock(&buffer->mutex);
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  buffer_t buffer;
  buffer.mutex = PTHREAD_MUTEX_INITIALIZER;
  buffer.can_consume = PTHREAD_COND_INITIALIZER;

  void *status;

  int producerT = std::stoi(argv[1]);
  int consumerT = std::stoi(argv[2]);
  int threadSize = producerT + consumerT;
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
