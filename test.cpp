#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>
#include <vector>

int thread_cnt = 0;
std::mutex mtx;  // Mutex for matrix synchronization
std::condition_variable cv; 
int print_count = 0;

void hello() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    {
        std::unique_lock<std::mutex> lock(mtx);
        thread_cnt++;

        if (thread_cnt < 5) {
            cv.wait(lock, [&] { return thread_cnt == 5; });
        } else {
            cv.notify_all();
        }
    }

    std::cout << "Hello from thread " << std::this_thread::get_id() << std::endl;
}

int main() {
    std::vector<std::thread> threads;
    

    for (int i = 0; i < 5; ++i) {
        threads.push_back(std::thread(hello));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
