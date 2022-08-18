#ifndef EVENTY_DATA_STRUCTURES_H
#define EVENTY_DATA_STRUCTURES_H

#include "Arduino.h"

namespace eventy {

// Stack

template<class T>
class Stack {
public:
    Stack();
    ~Stack();
    void push(T);
    T pop();
    bool isEmpty() const;
    int size() const;

private:
    class Node {
    public:
        Node(T item, Node *next);
        T item() const;
        Node *next() const { return _next; }
    private:
        T _item;
        Node *_next;
    };

    Node *_head;
    int _size = 0;
};


template<class T>
Stack<T>::Stack() : _head(nullptr) {

}

template<class T>
Stack<T>::~Stack() {
    while (_head)
        pop();
}

template<class T>
void Stack<T>::push(T item) {
    _head = new Node(item, _head);
    _size++;
}

template<class T>
T Stack<T>::pop() {
    Node *next = _head->next();
    T item = _head->item();
    delete _head;
    _head = next;
    _size--;
    return item;
}

template<class T>
bool Stack<T>::isEmpty() const {
    return size() == 0;
}

template<class T>
int Stack<T>::size() const {
    return _size;
}

template<class T>
Stack<T>::Node::Node(T item, Node *next) : _item(item), _next(next) {

}

template<class T>
T Stack<T>::Node::item() const {
    return _item;
}

}

#endif // EVENTY_DATA_STRUCTURES_H
