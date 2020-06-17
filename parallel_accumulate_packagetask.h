//
// Created by Yuejia on 2020/6/15.
//

#ifndef TEST_PARALLEL_ACCUMULATE_PACKAGETASK_H
#define TEST_PARALLEL_ACCUMULATE_PACKAGETASK_H

// 方式1：accumulate简单并行版本
#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <numeric>
#include <future>
#include "join_thread.h"

template<typename Iterator, typename T>
struct accumulate_block{
    // change
    T operator() (Iterator first, Iterator last) {
        return std::accumulate(first, last, T());
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, end);
    if(!length) return init;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length-1) / min_per_thread + 1;
    unsigned long const hardware_threads = std::thread::hardware_concurrency();
    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector<std::future<T> > futures(num_threads-1);
    std::vector<std::thread> threads(num_threads-1);
    // add
    join_threads joiner(threads);

    Iterator block_start = first;
    for(unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        std::packaged_task<T(Iterator, Iterator)> task(accumulate_block<Iterator, T>());
        futures[i] = task.get_future();
        threads[i] = std::thread(std::move(task), block_start, block_end);
        block_start = block_end;
    }
    T last_result = accumulate_block<Iterator,T>()(block_start, last);
    T result = init;
    for(unsigned long i = 0; i < num_threads - 1; ++i) {
        result += futures[i].get();
    }
    result += last_result;
    return result;
}

#endif //TEST_PARALLEL_ACCUMULATE_PACKAGETASK_H
