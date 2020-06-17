//
// Created by Yuejia on 2020/6/16.
//

#ifndef TEST_QUICK_SORT_BASED_THREAD_POOL_H
#define TEST_QUICK_SORT_BASED_THREAD_POOL_H

#include <iostream>
#include <list>
#include <vector>
#include <future>
#include <algorithm>
#include <chrono>
#include "thread_pool_return_value.h"

template<typename T>
struct sorter {
    thread_pool pool;

    std::list<T> do_sort(std::list<T>& chunk_data) {
        if(chunk_data.empty()) return chunk_data;

        std::list<T> result;
        result.splice(result.begin(),chunk_data,chunk_data.begin());
        T const& partition_val = *result.begin();
        typename std::list<T>::iterator divide_pointer =
                std::partition(result.begin(), result.end(), [&](T const& val){return val < partition_val;});

        std::list<T> new_low_chunk;
        new_low_chunk.splice(new_low_chunk.end(),chunk_data,chunk_data.begin(),divide_pointer);
        std::future<std::list<T> > new_lower = pool.commit(std::bind(&sorter::do_sort, this, std::move(new_low_chunk)));

        std::list<T> new_high(do_sort(chunk_data));
        result.splice(result.end(),new_high);
        while(new_lower.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
            pool.run_pending_task();
        }
        result.splice(result.begin(),new_lower.get());
        return result;
    }
};

template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input) {
    if(input.empty()) return input;
    sorter<T> s;
    return s.do_sort(input);
}

#endif //TEST_QUICK_SORT_BASED_THREAD_POOL_H
