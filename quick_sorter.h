//
// Created by Yuejia on 2020/6/13.
//

#ifndef TEST_QUICK_SORTER_H
#define TEST_QUICK_SORTER_H

#include <iostream>
#include <list>
#include <future>
#include <vector>
#include <thread>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <boost/shared_ptr.hpp>
#include "threadsafe_stack.h"

template<typename T>
class sorter {
    struct chunk_to_sort {
        std::list<T> data;
        std::promise<std::list<T> > promise;
    };
    threadsafe_stack<chunk_to_sort> chunks;
    std::vector<std::thread> threads;
    unsigned const max_thread_count;
    std::atomic<bool> end_of_data;

    sorter() : max_thread_count(std::thread::hardware_concurrency()), end_of_data(false) {}
    ~sorter() {
        end_of_data.store(true);
        for(unsigned i = 0; i < threads.size(); ++i) threads[i].join();
    }

    void try_sort_thunk() {
        boost::shared_ptr<chunk_to_sort> chunk = chunks.pop();
        if(chunk) sort_trunk(chunk);
    }

    std::list<T> do_sort(std::list<T>& chunk_data) {
        if(chunk_data.empty()) return chunk_data;
        std::list<T> result;
        // 将pivot元素取出
        result.splice(result.begin(), chunk_data, chunk_data.begin());
        T const& partition_val = *result.begin();

        // 对数据进行划分
        typename std::list<T>::iterator divide_point =
                std::partition(chunk_data.begin(),chunk_data.end(), [&](T const& val){ return val < partition_val;});

        // 取出小于partition_val的元素，并获取future
        chunk_to_sort new_low_chunk;
        new_low_chunk.data.splice(new_low_chunk.data.end(), chunk_data, chunk_data.begin(), divide_point);
        std::future<std::list<T> > new_lower = new_low_chunk.promise.get_future();

        // 将上述数据块入栈
        chunks.push(std::move(new_low_chunk));
        // 判断当前线程数是否小于最大线程数
        if(threads.size() < max_thread_count) {
            threads.push_back(std::thread(&sorter<T>::sort_thread, this));
        }

        // 本线程中对大于partition_val的元素块进行排序
        std::list<T> new_higher(do_sort(chunk_data));

        // 合并结果
        result.splice(result.end(), new_higher);
        // 当前半部分数据还未排序完时，可以启动其他线程继续排序
        while(new_lower.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
            try_sort_thunk();
        }
        // 待前半部分数据排序完，合并到result中
        result.splice(result.begin(), new_lower.get());
    }

    void sort_chunk(boost::shared_ptr<chunk_to_sort> const& chunk) {
        chunk->promise.set_value(do_sort(chunk->data));
    }
    void sort_thread() {
        while(!end_of_data) {
            try_sort_thunk();
            std::this_thread::yield();
        }
    }
};

template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input) {
    if(input.empty()) return input;
    sorter<T> s;
    return s.do_sort(input);
}

#endif //TEST_QUICK_SORTER_H
