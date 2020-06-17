//
// Created by Yuejia on 2020/6/15.
//

#ifndef TEST_PARALLEL_ACCUMULATE_ASYNC_H
#define TEST_PARALLEL_ACCUMULATE_ASYNC_H

#include <iostream>
#include <future>
#include <numeric>

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, last);
    unsigned long const max_chunk_size = 25;
    if(length < max_chunk_size) {
        return std::accumulate(first, last, init);
    } else {
        Iterator mid_point = first;
        std::advance(mid_point, length/2);
        std::future<T> first_half_result =
                std::async(parallel_accumulate<Iterator,T>(first, mid_point, init);
        T second_half_result = parallel_accumulate<Iterator,T>(mid_point, last, T());
        return first_half_result.get() + second_half_result;
    }
}

#endif //TEST_PARALLEL_ACCUMULATE_ASYNC_H
