#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

#define NUM_PHILOSOPHERS 5

std::mutex forks[NUM_PHILOSOPHERS];

void philosopher(int id) {
    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    // To prevent deadlock: last philosopher picks up forks in reverse order
    if (id == NUM_PHILOSOPHERS - 1) {
        std::swap(left, right);
    }

    for (int i = 0; i < 3; ++i) {
        // Thinking
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Pick up forks
        forks[left].lock();
        forks[right].lock();

        // Eating
        std::cout << "Philosopher " << id << " is eating.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Put down forks
        forks[right].unlock();
        forks[left].unlock();

        // Thinking again
        std::cout << "Philosopher " << id << " is thinking.\n";
    }
}

int main() {
    std::vector<std::thread> philosophers;

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.push_back(std::thread(philosopher, i));
    }

    // Wait for all threads to finish
    for (auto &t : philosophers) {
        t.join();
    }

    return 0;
}
