#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> q;
    std::mutex mtx;
    std::condition_variable cv;
public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(std::move(value));
        cv.notify_one();
    }
    
    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]{ return !q.empty(); });
        value = std::move(q.front());
        q.pop();
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return q.empty();
    }
};

#endif