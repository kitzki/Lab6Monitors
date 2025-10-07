#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <cstdlib> // for rand()

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

std::queue<int> buffer;
std::mutex mtx;
std::condition_variable not_full, not_empty;

void producer() {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait until buffer has space
        not_full.wait(lock, [] { return buffer.size() < BUFFER_SIZE; });

        int item = rand() % 100;
        buffer.push(item);
        std::cout << "Produced: " << item << "\n";

        lock.unlock();
        not_empty.notify_one(); // Signal consumer

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void consumer() {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait until buffer has items
        not_empty.wait(lock, [] { return !buffer.empty(); });

        int item = buffer.front();
        buffer.pop();
        std::cout << "Consumed: " << item << "\n";

        lock.unlock();
        not_full.notify_one(); // Signal producer

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

int main() {
    std::thread prod(producer);
    std::thread cons(consumer);

    prod.join();
    cons.join();

    return 0;
}
