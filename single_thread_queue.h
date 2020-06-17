//
// Created by Yuejia on 2020/6/11.
//

#ifndef TEST_SINGLE_THREAD_QUEUE_H
#define TEST_SINGLE_THREAD_QUEUE_H

#include <iostream>
#include <memory>

template<typename T>
class queue {
private:
    struct node {
        T data;
        // 为了保证节点不需要时，它们能被删除，而无需delete
        std::unique_ptr<node> next;
        node(T data_): data(std::move(data_)) {}
    };
    std::unique_ptr<node> head;
    node* tail;
public:
    queue() {}
    queue(const queue&) =delete ;
    queue& operator=(const queue&) =delete ;

    std::shared_ptr<T> try_pop() {
        if(!head) {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(std::make_shared<T>(std::move(head->data)));
        std::unique_ptr<T> const old_head = std::move(head);
        head = std::move(old_head->next);
        return res;
    }

    void push(T new_data) {
        std::unique_ptr<node> p(new node(std::move(new_value)));
        node* const new_tail = p.get();
        if(tail) {
            tail->next = std::move(p);
        } else {
            head = std::move(p);
        }
        tail = new_tail;
    }
};

#endif //TEST_SINGLE_THREAD_QUEUE_H
