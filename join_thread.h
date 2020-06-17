//
// Created by Yuejia on 2020/6/16.
//

#ifndef TEST_JOIN_THREAD_H
#define TEST_JOIN_THREAD_H

#include <iostream>
#include <thread>
#include <vector>

// 使用RAII思想，在析构函数中清除多余的线程
class join_threads {
    std::vector<std::thread>& threads;
public:
    explicit join_threads(std::vector<std::thread>& threads_): threads(threads_) {}
    ~join_threads() {
        for(unsigned long i = 0 ; i < threads.size(); ++i) {
            if(threads[i].joinable()) threads[i].join();
        }
    }
};

#endif //TEST_JOIN_THREAD_H
