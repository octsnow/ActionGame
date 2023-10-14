#pragma once
#include <vector>

class Vector2d {
public:
    double x, y;
};

template<typename T>
class LinkedNode {
public:
    LinkedNode(T v) {
        this->m_value = v;
        this->m_next = nullptr;
    }

    T* getValue() {
        return this->m_value;
    }

    T m_value;
    LinkedNode<T>* m_next;
};

template<typename T>
class LinkedList {
public:
    LinkedList() {
        this->m_head = nullptr;
    }

    ~LinkedList() {
        LinkedNode<T>* node = this->m_head;
        while(node != nullptr) {
            LinkedNode<T>* nextNode = node->m_next;
            delete node;
            node = nextNode;
        }
    }

    void append(T v) {
        LinkedNode<T>* newNode = new LinkedNode<T>(v);
        
        if(this->m_head != nullptr) {
            newNode->m_next = this->m_head;
        }
        this->m_head = newNode;
    }
    
    void remove(LinkedNode<T>** node) {
        if(node == nullptr) return;
        if(*node == nullptr) return;

        LinkedNode<T>* rmNode = *node;

        *node = (*node)->m_next;

        delete rmNode;
    }

    template<typename Func>
    void for_each(Func func) {
        LinkedNode<T>** node = &this->m_head;

        while(*node != nullptr) {
            func(node);
            node = &(*node)->m_next;
        }
    }

private:
    LinkedNode<T>* m_head;
};

template<typename T>
class Que {
public:
    Que() {
        this->m_head = nullptr;
        this->m_tail = nullptr;
    }

    ~Que() {
        LinkedNode<T>* node = this->m_head;
        while(node != nullptr) {
            LinkedNode<T>* nextNode = node->m_next;
            delete node;
            node = nextNode;
        }

        this->m_head = nullptr;
        this->m_tail = nullptr;
    }

    void append(T v) {
        LinkedNode<T>* node = new LinkedNode<T>(v);

        if(this->m_tail == nullptr) {
            this->m_head = node;
            this->m_tail = node;
        } else {
            node->m_next = this->m_tail;
            this->m_tail = node;
        }
    }

    T* getValue() {
        if(this->m_head == nullptr) {
            return nullptr;
        }

        return &this->m_head->m_value;
    }

    void pop() {
        LinkedNode<T>* node = this->m_head;

        if(node == nullptr) {
            return;
        }

        this->m_head = node->m_next;
        delete node;
    }

private:
    LinkedNode<T> *m_head, *m_tail;
};
