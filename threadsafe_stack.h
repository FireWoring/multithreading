//
// Created by Yuejia on 2020/6/3.
//

#ifndef TEST_THREADSAFE_STACK_H
#define TEST_THREADSAFE_STACK_H

#include <exception>
#include <memory>
#include <stack>
#include <mutex>

class some_big_object{};
void swap(some_big_object& lhs, some_big_object& rhs);
class X {
private:
    some_big_object some_detail;
    std::mutex m;
public:
    X(some_big_object const& sd): some_detail(sd) {}
    friend void swap(X& lhs, X& rhs) {
        if(&lhs == &rhs) return;
//        std::unique_lock<std::mutex> lock_a(lhs.m, std::defer_lock);
//        std::unique_lock<std::mutex> lock_b(rhs.m, std::defer_lock);
//        std::lock(lock_a, lock_b);

        std::lock(lhs.m, rhs.m);
        std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock);
        std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock);
        swap(lhs.some_detail, rhs.some_detail);
    }
};

struct empty_stack: public std::exception {
    const char* what() const throw() {
        return "empty stack";
    }
};

template<typename T>
class threadsafe_stack {
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    threadsafe_stack(){}
    threadsafe_stack(const threadsafe_stack& other) {
        std::lock_guard<std::mutex> lock(m);
        data = other.data;
    }
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;

    void push(T new_data) {
        std::lock_guard<std::mutex> lock(m);
        data.push(new_data);
    }
    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_stack();
        std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
        data.pop();
        return res;
    }
    void pop(T& value) {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_stack();
        value = data.top();
        data.pop();
    }
    bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};


#endif //TEST_THREADSAFE_STACK_H
