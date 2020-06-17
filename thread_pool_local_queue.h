//
// Created by Yuejia on 2020/6/17.
//

#ifndef TEST_THREAD_POOL_LOCAL_QUEUE_H
#define TEST_THREAD_POOL_LOCAL_QUEUE_H

#include <iostream>
#include <thread>
#include <future>
#include <queue>
#include <memory>
#include <atomic>
#include <vector>
#include "thread_pool_return_value.h"
#include "threadsafe_queue.h"
#include "join_thread.h"

class thread_pool_local_queue {
    std::atomic_bool done;
    threadsafe_queue<function_wrapper> pool_work_queue;
    typedef std::queue<function_wrapper> local_queue_type;
    static thread_local std::unique_ptr<local_queue_type> local_work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void work_thread() {
        local_work_queue.reset(new local_queue_type);
        while (!done) {
            run_pending_task();
        }
    }

public:
    thread_pool_local_queue(): done(false), joiner(threads) {
        unsigned const threads_count = std::thread::hardware_concurrency();
        try {
            for(unsigned i = 0; i < threads_count; ++i) {
                threads.emplace_back(std::thread(&thread_pool_local_queue::work_thread, this));
            }
        } catch (...) {
            done = true;
            throw ;
        }
    }
    ~thread_pool_local_queue() { done = true; }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> commit(FunctionType f) {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        if(local_work_queue) local_work_queue->push(std::move(task));
        else pool_work_queue.push(std::move(task));
        return res;
    }

    void run_pending_task() {
        function_wrapper task;
        if(local_work_queue && !local_work_queue->empty()) {
            task = std::move(local_work_queue->front());
            local_work_queue->pop();
            task();
        } else if(pool_work_queue.try_pop(task)) {
            task();
        } else {
            std::this_thread::yield();
        }
    }
};

#endif //TEST_THREAD_POOL_LOCAL_QUEUE_H
