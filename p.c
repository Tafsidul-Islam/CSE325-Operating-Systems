#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THINKING 0
#define HUNGRY 1
#define EATING 2

typedef sem_t semaphore;

int *state;
semaphore mutex;
semaphore *s;
int *e_count;
int N;
volatile int termination_flag = 0;


void think(int i);
void eat(int i);
void *philosopher(void *num);
void take_forks(int i);
void put_forks(int i);
void test(int i);

void think(int i) {
    printf("Philosopher %d is thinking.\n", i);
    sleep(rand() % 3 + 1);
}

void eat(int i) {
    printf("Philosopher %d is eating.\n", i);
    int eating_time = rand() % 5 + 1;
    for (int j = 1; j <= eating_time; j++) {
        sleep(1);
        int percent_eaten = (j * 100) / eating_time;
        printf("Philosopher %d has eaten %d%%.\n", i, percent_eaten);
        if (percent_eaten < 50) {
            printf("Philosopher %d will not release the fork, wants to eat more.\n", i);
        }
        if (percent_eaten >= 50 && rand() % 100 < 50) {
            printf("Philosopher %d decides to release the forks voluntarily.\n", i);
            put_forks(i);
            return;
        }
    }
    e_count[i]++;
    if (e_count[i] >= 3) {
        int ate = 1;
        for (int k = 0; k < N; k++) {
            if (e_count[k] < 3) {
                ate = 0;
                break;
            }
        }
        if (ate) {
            termination_flag = 1;
        }
    }
    printf("Philosopher %d has finished eating.\n", i);
}

void *philosopher(void *num) {
    int i = *((int *)num);
    int max_iterations = 5; 
    int iterations = 0;
    while (!termination_flag && iterations < max_iterations) {
        think(i);
        take_forks(i);
        eat(i);
        put_forks(i);
        iterations++;
    }
    pthread_exit(NULL);
}

void take_forks(int i) {
    sem_wait(&mutex);
    state[i] = HUNGRY;
    printf("Philosopher %d is hungry.\n", i);
    test(i);
    sem_post(&mutex);
    sem_wait(&s[i]);
}

void put_forks(int i) {
    sem_wait(&mutex);
    state[i] = THINKING;
    printf("Philosopher %d is releasing forks and now thinking.\n", i);
    test((i + N - 1) % N);
    test((i + 1) % N);
    sem_post(&mutex);
}

void test(int i) {
    if (state[i] == HUNGRY && state[(i + N - 1) % N] != EATING && state[(i + 1) % N] != EATING) {
        state[i] = EATING;
        sem_post(&s[i]);
    }
}

int main() {
    srand(time(NULL));
    do {
        printf("Enter an odd number of philosophers: ");
        scanf("%d", &N);
        if (N % 2 == 0) {
            printf("Number of philosophers must be odd. Please try again.\n");
        }
    } while (N % 2 == 0);
    state = malloc(N * sizeof(int));
    s = malloc(N * sizeof(semaphore));
    e_count = malloc(N * sizeof(int));
    pthread_t *thread_id = malloc(N * sizeof(pthread_t));
    sem_init(&mutex, 0, 1);
    for (int i = 0; i < N; i++) {
        sem_init(&s[i], 0, 0);
        state[i] = THINKING;
        e_count[i] = 0;
    }
    for (int i = 0; i < N; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&thread_id[i], NULL, philosopher, arg);
    }
    for (int i = 0; i < N; i++) {
        pthread_join(thread_id[i], NULL);
    }
    printf("Program terminates.\n");
    free(state);
    free(s);
    free(e_count);
    free(thread_id);
    return 0;
}

