#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&cond_producer, &mutex);
        }
        int item = rand() % 100; 
        buffer[count] = item;
        printf("Produced: %d\n", item);
        count++;
        pthread_cond_signal(&cond_consumer);
        pthread_mutex_unlock(&mutex);
    }
}

void *consumer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == 0) {
            pthread_cond_wait(&cond_consumer, &mutex);
        }
        int item = buffer[count - 1];
        printf("Consumed: %d\n", item);
        count--;
        pthread_cond_signal(&cond_producer);
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    pthread_t prod_thread, cons_thread;

    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    return 0;
}

