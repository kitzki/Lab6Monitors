#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

sem_t rw_mutex, mutex;
int reader_count = 0;
int shared_data = 0;

void *reader(void *arg) {
    sem_wait(&mutex);
    reader_count++;
    if (reader_count == 1)
        sem_wait(&rw_mutex);  // First reader locks resource
    sem_post(&mutex);

    // Reading section
    printf("Reader %ld reads: %d\n", (long)arg, shared_data);
    sleep(1);

    sem_wait(&mutex);
    reader_count--;
    if (reader_count == 0)
        sem_post(&rw_mutex);  // Last reader unlocks resource
    sem_post(&mutex);
    return NULL;
}

void *writer(void *arg) {
    sem_wait(&rw_mutex);  // Writers get exclusive access
    shared_data++;
    printf("Writer %ld writes: %d\n", (long)arg, shared_data);
    sleep(1);
    sem_post(&rw_mutex);
    return NULL;
}

int main() {
    pthread_t readers[3], writers[2];
    sem_init(&rw_mutex, 0, 1);
    sem_init(&mutex, 0, 1);

    // Create reader threads
    for (long i = 0; i < 3; i++)
        pthread_create(&readers[i], NULL, reader, (void *)i);

    // Create writer threads
    for (long i = 0; i < 2; i++)
        pthread_create(&writers[i], NULL, writer, (void *)i);

    // Wait for all threads to finish
    for (int i = 0; i < 3; i++)
        pthread_join(readers[i], NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(writers[i], NULL);

    sem_destroy(&rw_mutex);
    sem_destroy(&mutex);
    return 0;
}
