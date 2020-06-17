//
// Created by Yuejia on 2020/6/11.
//

#ifndef TEST_THREADSAFE_QUEUE_1_H
#define TEST_THREADSAFE_QUEUE_1_H

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class threadsafe_queue_1 {
private:
    mutable std::mutex mut;
    std::queue<std::shared_ptr<T> > data_queue;
    std::condition_variable data_cond;
public:
    threadsafe_queue_1(){}
    threadsafe_queue_1(threadsafe_queue_1 const& other) {
        std::lock_guard<std::mutex> lock(mut);
        data_queue = other.data_queue;
    }
    threadsafe_queue_1& operator=(threadsafe_queue_1 const&) = delete;

    void push(T new_value) {
        std::shared_ptr<T> data(std::make_shared<T>(std::move(new_value)));
        std::lock_guard<std::mutex> lock(mut);
        data_queue.push(data);
        data_cond.notify_one();
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mut);
        if(data_queue.empty()) return false;
        value = std::move(*data_queue.front());
        data_queue.pop();
        return true;
    }
    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock(mut);
        if(data_queue.empty()) return std::shared_ptr<T>();
        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(mut);
        data_cond.wait(lock, [this]{return !data_queue.empty();});
        value = std::move(*data_queue.front());
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lock(mut);
        data_cond.wait(lock, [this]{return !data_queue.empty();});
        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mut);
        return data_queue.empty();
    }
};

#endif //TEST_THREADSAFE_QUEUE_1_H
