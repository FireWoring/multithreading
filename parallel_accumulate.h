//
// Created by Yuejia on 2020/6/13.
//

#ifndef TEST_PARALLEL_ACCUMULATE_H
#define TEST_PARALLEL_ACCUMULATE_H

#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <numeric>

template<typename Iterator, typename T>
struct accumulate_block{
    void operator() (Iterator first, Iterator last, T& result) {
        result = std::accumulate(first, last, result);
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, end);
    if(!length) return init;

    // 定义单线程处理的最小数据个数
    unsigned long const min_per_thread = 25;
    // 计算最大线程个数
    unsigned long const max_threads = (length-1) / min_per_thread + 1;
    // 获取硬件支持的最大线程个数
    unsigned long const hardware_threads = std::thread::hardware_concurrency();
    // 计算最终的线程数
    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    // 计算每个数据块的大小
    unsigned long const block_size = length / num_threads;

    std::vector<T> results(num_threads);
    // 主线程使用了一个线程
    std::vector<std::thread> threads(num_threads-1);

    Iterator block_start = first;
    for(unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i] = std::thread(accumulate_block<Iterator, T>(), block_start, block_end, std::ref(results[i]));
        block_start = block_end;
    }
    accumulate_block<Iterator, T>()(block_start, last, results[num_threads-1]);
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    return std::accumulate(results.begin(), results.end(), init);
}

#endif //TEST_PARALLEL_ACCUMULATE_H
