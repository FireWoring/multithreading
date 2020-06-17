//
// Created by Yuejia on 2020/6/16.
//

#ifndef TEST_THREAD_POOL_SIMPLE_H
#define TEST_THREAD_POOL_SIMPLE_H

#include <iostream>
#include <thread>
#include <atomic>
#include "threadsafe_queue.h"
#include "join_thread.h"

class thread_pool {
    std::atomic_bool done;
    threadsafe_queue<std::function<void()> > work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread() {
        while(!done) {
            std::function<void()> task;
            if(work_queue.try_pop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool(): done(false), joiner(threads) {
        unsigned const thread_count = std::thread::hardware_concurrency();
        try{
            for(unsigned i = 0; i < thread_count; ++i) {
                threads.emplace_back(std::thread(&thread_pool::worker_thread, this));
            }
        } catch (...) {
            done = true;
            throw;
        }
    }

    ~thread_pool() {
        done = true;
    }

    template<typename FunctionType>
    void commit(FunctionType f) {
        work_queue.push(std::function<void()>(f));
    }
};

#endif //TEST_THREAD_POOL_SIMPLE_H
