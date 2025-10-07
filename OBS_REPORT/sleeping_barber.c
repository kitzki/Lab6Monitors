#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_CHAIRS 3
#define NUM_CUSTOMERS 5

sem_t customers;  // Counts waiting customers
sem_t barber;     // Barber ready signal
sem_t chairs;     // Available waiting chairs
pthread_mutex_t mutex;  // For protecting shared access

void *barber_thread(void *arg) {
    while (1) {
        // Wait for a customer
        sem_wait(&customers);

        // Barber gets a chair back since customer is being served
        pthread_mutex_lock(&mutex);
        sem_post(&chairs);
        pthread_mutex_unlock(&mutex);

        printf("💈 Barber is cutting hair...\n");
        sleep(2); // Simulate haircut time

        // Signal that barber is done
        sem_post(&barber);
    }

    return NULL;
}

void *customer_thread(void *arg) {
    int id = *(int *)arg;
    pthread_mutex_lock(&mutex);

    // Try to take a seat
    if (sem_trywait(&chairs) == 0) {
        printf("Customer %d is waiting.\n", id);
        sem_post(&customers);
        pthread_mutex_unlock(&mutex);

        // Wait until barber is ready
        sem_wait(&barber);
        printf("Customer %d got a haircut.\n", id);
    } else {
        pthread_mutex_unlock(&mutex);
        printf("Customer %d left (no chairs available).\n", id);
    }

    free(arg);
    return NULL;
}

int main() {
    pthread_t barber_t, customers_t[NUM_CUSTOMERS];

    sem_init(&customers, 0, 0);
    sem_init(&barber, 0, 0);
    sem_init(&chairs, 0, NUM_CHAIRS);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&barber_t, NULL, barber_thread, NULL);

    // Customers arrive at random intervals
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&customers_t[i], NULL, customer_thread, id);
        sleep(rand() % 3 + 1); // Random arrival between 1–3 sec
    }

    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customers_t[i], NULL);
    }

    // Note: Barber runs indefinitely — add termination logic if needed
    sem_destroy(&customers);
    sem_destroy(&barber);
    sem_destroy(&chairs);
    pthread_mutex_destroy(&mutex);

    return 0;
}
