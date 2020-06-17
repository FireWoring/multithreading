//
// Created by Yuejia on 2020/6/17.
//

#ifndef TEST_INTERRUPTABLE_THREAD_H
#define TEST_INTERRUPTABLE_THREAD_H

#include <iostream>
#include <thread>
#include <future>
#include <atomic>
#include <boost/thread/thread.hpp>

class interrupt_flag {
    std::atomic_bool flag;
public:
    void set() { flag = true; }
    bool is_set() const { return flag; }
};
thread_local interrupt_flag this_thread_interrupt_flag;

class interruptable_thread {
    std::thread internal_thread;
    interrupt_flag* flag;
public:
    template<typename FunctionType>
    explicit interruptable_thread(FunctionType f) {
        std::promise<interrupt_flag*> p;
        internal_thread = std::thread([f, &p]{
           p.set_value(&this_thread_interrupt_flag);
           f();
        });
        flag = p.get_future().get();
    }
    void interrupt() {
        if(flag) {
            flag->set();
        }
    }
};

void interrupt_point() {
    if(this_thread_interrupt_flag.is_set()) {
        throw boost::thread_interrupted();
    }
}

#endif //TEST_INTERRUPTABLE_THREAD_H
