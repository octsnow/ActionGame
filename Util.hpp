#pragma once
#include <vector>
#include <iostream>

class Vector2d {
public:
    Vector2d() : x(0), y(0) {}
    Vector2d(double x, double y): x(x), y(y) {}
    double x, y;
};

template<typename T>
class LinkedNode {
public:
    LinkedNode(T v) {
        this->mValue = v;
        this->mNext = nullptr;
        this->mPrev = nullptr;
    }

    T* GetValue() {
        return &this->mValue;
    }

    T mValue;
    LinkedNode<T>* mPrev;
    LinkedNode<T>* mNext;
};

template<typename T>
class LinkedList {
public:
    LinkedList() {
        this->mHead = nullptr;
        this->mTail = nullptr;
    }

    ~LinkedList() {
        LinkedNode<T>* node = this->mHead;
        while(node != nullptr) {
            LinkedNode<T>* nextNode = node->mNext;
            delete node;
            node = nextNode;
        }
    }

    void Append(T v) {
        LinkedNode<T>* newNode = new LinkedNode<T>(v);
        
        if(this->mHead == nullptr) {
            this->mHead = newNode;
            this->mTail = newNode;
        } else {
            newNode->mPrev = this->mTail;
            this->mTail->mNext = newNode;
            this->mTail = newNode;
        }
    }
    
    void Remove(LinkedNode<T>* node) {
        if(node == nullptr) return;
        if(node == this->mHead) {
            this->mHead = node->mNext;
            std::cout << "head null" << std::endl;
        }
        if(node == this->mTail) {
            this->mTail = this->mTail->mPrev;
            std::cout << "tail null" << std::endl;
        }
        if(node->mPrev != nullptr) {
            node->mPrev->mNext = node->mNext;
            std::cout << "prev not null" << std::endl;
        }
        if(node->mNext != nullptr) {
            node->mNext->mPrev = node->mPrev;
            std::cout << "next not null" << std::endl;
        }

        std::cout << "delete node" << std::endl;
        delete node;
        std::cout << "deleted node" << std::endl;
    }

    LinkedNode<T>* GetHead() {
        return this->mHead;
    }

    LinkedNode<T>* GetTail() {
        return this->mTail;
    }

    template<typename Func>
    void for_each(Func func) {
        LinkedNode<T>* node = this->mHead;

        while(node != nullptr) {
            func(node);
            node = node->mNext;
        }
    }

private:
    LinkedNode<T>* mHead;
    LinkedNode<T>* mTail;
};
