//
// Created by Yuejia on 2020/6/11.
//

#ifndef TEST_HEAD_LINKED_QUEUE_H
#define TEST_HEAD_LINKED_QUEUE_H

#include <iostream>
#include <memory>

template<typename T>
class queue {
private:
    struct node {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };
    std::unique_ptr<node> head;
    node* tail;
public:
    queue(): head(new node), tail(head.get()) {}
    queue(const queue&) =delete ;
    queue& operator=(const queue&) =delete ;

    std::shared_ptr<T> try_pop() {
        if(head.get() == tail) {
            return std::make_shared<T>();
        }
        std::shared_ptr<T> const res(head->data);
        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return res;
    }

    void push(T value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_data)));
        std::unique_ptr<node> p(new node);
        node* const new_tail = p.get();
        tail->data = new_data;
        tail->next = new_tail;
        tail = new_tail;
    }
};

#endif //TEST_HEAD_LINKED_QUEUE_H
