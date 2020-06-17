//
// Created by Yuejia on 2020/6/16.
//

#ifndef TEST_THREAD_POOL_RETURN_VALUE_H
#define TEST_THREAD_POOL_RETURN_VALUE_H

#include <iostream>
#include <vector>
#include <atomic>
#include <memory>
#include <future>
#include "threadsafe_queue.h"
#include "join_thread.h"

class function_wrapper{
    struct function_base {
        virtual void call() = 0;
        virtual ~function_base() {}
    };
    std::unique_ptr<function_base> impl;

    template<typename F>
    struct function_type: function_base {
        F f;
        function_type(F&& f_): f(std::move(f_)) {}
        void call() { f(); }
    };

public:
    void operator()() { impl->call(); }

    template<typename F>
    explicit function_wrapper(F&& f): impl(new function_type<F>(std::move(f))) {}
    function_wrapper() =default;
    function_wrapper(function_wrapper&& other) noexcept : impl(std::move(other.impl)) {}
    function_wrapper& operator= (function_wrapper&& other) noexcept {
        impl = std::move(other.impl);
        return *this;
    }
    function_wrapper(const function_wrapper&) = delete;
    function_wrapper(function_wrapper&) =delete;
    function_wrapper& operator=(const function_wrapper&) =delete;
};

class thread_pool {
    std::atomic_bool done;
    threadsafe_queue<function_wrapper> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread() {
        if(!done) {
            function_wrapper task;
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
    std::future<typename std::result_of<FunctionType()>::type> commit(FunctionType f) {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));
        return res;
    }

    void run_pending_task() {
        function_wrapper task;
        if(work_queue.try_pop(task)) {
            task();
        } else {
            std::this_thread::yield();
        }
    }
};

#endif //TEST_THREAD_POOL_RETURN_VALUE_H
