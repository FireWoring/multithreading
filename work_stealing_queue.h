//
// Created by Yuejia on 2020/6/17.
//

#ifndef TEST_WORK_STEALING_QUEUE_H
#define TEST_WORK_STEALING_QUEUE_H

#include <iostream>
#include <deque>
#include <mutex>
#include "thread_pool_return_value.h"

class work_stealing_queue {
private:
    typedef function_wrapper data_type;
    std::deque<data_type> the_queue;
    mutable std::mutex the_mutex;
public:
    work_stealing_queue() =default;
    work_stealing_queue(work_stealing_queue const&) =delete;
    work_stealing_queue& operator=(work_stealing_queue const&) =delete;

    void push(data_type data) {
        std::lock_guard<std::mutex> lock(the_mutex);
        the_queue.push_front(std::move(data));
    }
    bool empty() {
        std::lock_guard<std::mutex> lock(the_mutex);
        return the_queue.empty();
    }
    bool try_pop(data_type& res) {
        std::lock_guard<std::mutex> lock(the_mutex);
        if(the_queue.empty()) return false;
        res = std::move(the_queue.front());
        the_queue.pop_front();
        return true;
    }
    bool try_steal(data_type& res) {
        std::lock_guard<std::mutex> lock(the_mutex);
        if(the_queue.empty()) return false;
        res = std::move(the_queue.back());
        the_queue.pop_back();
        return true;
    }
};

#endif //TEST_WORK_STEALING_QUEUE_H
