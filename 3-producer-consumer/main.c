#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_CAPACITY 4

//region buffer variables

int buffer[BUFFER_CAPACITY];
unsigned int buffer_size = 0, front = 0, rear = BUFFER_CAPACITY - 1;

//endregion

//region lab variables

sem_t buffer_semaphore, counter_semaphore, full_buffer_semaphore, empty_buffer_semaphore;
int counter = 0;

int total_messages_num = 10;
int monitor_interval = 2;
int collector_interval = 1;

int collector_counter = 0;

//endregion

//region buffer functions

void enqueue(int data) {
    rear = (rear + 1) % BUFFER_CAPACITY;
    buffer_size++;
    buffer[rear] = data;
    printf("\nMonitor thread: writing to buffer at position %d", rear);
}

int dequeue() {
    int data = 0;

    data = buffer[front];
    printf("\nCollector thread: reading from buffer at position %d and value = %d", front, data);

    front = (front + 1) % BUFFER_CAPACITY;

    buffer_size--;

    return data;
}

//endregion

//region lab functions

void *message(void *arg) {
    int message_num = *((int *) arg);

    sleep(rand() % 7);
    printf("\nCounter thread %d: received a message", message_num);
    printf("\nCounter thread %d: waiting to write", message_num);
    sem_wait(&counter_semaphore);
    counter++;
    printf("\nCounter thread %d: now adding to counter, counter value = %d", message_num, counter);
    sem_post(&counter_semaphore);

    return NULL;
}

void *monitor(void *arg) {
    while (1) {
        sleep(monitor_interval);
        printf("\nMonitor thread: waiting to read counter");

        // 1) take snapshot from counter and reset
        sem_wait(&counter_semaphore);

        int temp = counter;
        counter = 0;

        sem_post(&counter_semaphore);

        // 2) check buffer is full or not

        int sem_value;
        sem_getvalue(&full_buffer_semaphore,&sem_value);

        if(sem_value <= 0){
            printf("\nMonitor thread: Buffer full!!");
        }

        sem_wait(&full_buffer_semaphore);

        // 3) enter counter value in buffer

        sem_wait(&buffer_semaphore);

        printf("\nMonitor thread: reading a count value of %d", temp);

        enqueue(temp);

        sem_post(&buffer_semaphore);
        sem_post(&empty_buffer_semaphore);
    }
}

void *collector(void *arg) {
    while (1) {

        sleep(collector_interval);
        // 1) check buffer is empty or not

        int sem_value;
        sem_getvalue(&full_buffer_semaphore,&sem_value);

        if(sem_value == BUFFER_CAPACITY){
            printf("\nCollector thread: buffer is empty!!");
        }

        sem_wait(&empty_buffer_semaphore);

        // 2) dequeue the buffer

        sem_wait(&buffer_semaphore);

        int value = dequeue();
        collector_counter += value;

        if (collector_counter == total_messages_num) {
            return NULL;
        }

        sem_post(&buffer_semaphore);
        sem_post(&full_buffer_semaphore);
    }
}

//endregion

int main() {
    sem_init(&buffer_semaphore, 0, 1);
    sem_init(&counter_semaphore, 0, 1);
    sem_init(&full_buffer_semaphore, 0,BUFFER_CAPACITY);
    sem_init(&empty_buffer_semaphore, 0, 0);

    pthread_t monitor_thread, collector_thread;
    pthread_t messages[total_messages_num];

    pthread_create(&monitor_thread, NULL, monitor, NULL);
    pthread_create(&collector_thread, NULL, collector, NULL);

    for (int i = 0; i < total_messages_num; ++i) {
        int message_num = i;

        pthread_create(&messages[i], NULL, message, &message_num);
    }

    pthread_join(collector_thread, NULL);

    sem_destroy(&buffer_semaphore);
    sem_destroy(&counter_semaphore);
    sem_destroy(&full_buffer_semaphore);
    sem_destroy(&empty_buffer_semaphore);
}