#pragma once

#include <queue>
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
        this->mForeachDoing = false;
    }

    ~LinkedList() {
        LinkedNode<T>* node = this->mHead;
        while(node != nullptr) {
            LinkedNode<T>* nextNode = node->mNext;
            delete node;
            node = nextNode;
        }
    }

    /*
    // copy constructors
    LinkedList(const LinkedList<T>& other) {
        if(this->mHead != nullptr) {
            std::cout << "not null" << std::endl;
        }
        this->Clear();
        LinkedNode<T>* node = other.mHead;
        while(node != nullptr) {
            this->Append(*node->GetValue());
            node = node->mNext;
        }
    }

    void operator=(const LinkedList<T>& other) {
        this->Clear();
        LinkedNode<T>* node = other.mHead;
        while(node != nullptr) {
            this->Append(*node->GetValue());
            node = node->mNext;
        }
    }
*/
    void Append(T v) {
        if(this->mForeachDoing) {
            this->mAppendQueue.push(v);
            return;
        }
        LinkedNode<T>* newNode = new LinkedNode<T>(v);
        
        if(this->mHead == nullptr) {
            this->mHead = newNode;
            this->mTail = newNode;
        } else {
            assert(this->mTail != nullptr);
            newNode->mPrev = this->mTail;
            this->mTail->mNext = newNode;
            this->mTail = newNode;
        }
    }
    
    void Remove(LinkedNode<T>* node) {
        if(this->mForeachDoing) {
            this->mRemoveQueue.push(node);
            return;
        }
        if(node == nullptr) return;
        if(node == this->mHead) {
            this->mHead = node->mNext;
        }
        if(node == this->mTail) {
            this->mTail = this->mTail->mPrev;
        }
        if(node->mPrev != nullptr) {
            node->mPrev->mNext = node->mNext;
        }
        if(node->mNext != nullptr) {
            node->mNext->mPrev = node->mPrev;
        }

        delete node;
    }

    bool Empty() {
        return this->mHead == nullptr;
    }

    LinkedNode<T>* Search(T v) const {
        std::cout << "Search" << std::endl;
        LinkedNode<T>* node = this->mHead;
        while(node != nullptr) {
            if(*node->GetValue() == v) {
                return node;
            }
            node = node->mNext;
        }

        return nullptr;
    }

    void Clear() {
        this->for_each([&](LinkedNode<T>* node) {
            this->Remove(node);
        });
        /*
        LinkedNode<T>* node = this->mHead;
        std::cout << "B" << std::endl;
        while(node != nullptr) {
            LinkedNode<T>* nextNode = node->mNext;
            std::cout << "BA" << std::endl;
            delete node;
            std::cout << "BB" << std::endl;
            node = nextNode;
        }
        std::cout << "C" << std::endl;

        this->mHead = nullptr;
        this->mTail = nullptr;
        */
        this->mHead = nullptr;
        this->mTail = nullptr;
    }

    LinkedNode<T>* GetHead() {
        return this->mHead;
    }

    LinkedNode<T>* GetTail() {
        return this->mTail;
    }

    template<typename Func>
    void for_each(Func func) {
        this->mForeachDoing = true;
        LinkedNode<T>* node = this->mHead;

        while(node != nullptr) {
            func(node);
            node = node->mNext;
        }
        this->mForeachDoing = false;

        while(!this->mAppendQueue.empty()) {
            this->Append(this->mAppendQueue.front());
            this->mAppendQueue.pop();
        }
        while(!this->mRemoveQueue.empty()) {
            this->Remove(this->mRemoveQueue.front());
            this->mRemoveQueue.pop();
        }
    }

private:
    LinkedNode<T>* mHead;
    LinkedNode<T>* mTail;
    bool mForeachDoing;
    std::queue<T> mAppendQueue;
    std::queue<LinkedNode<T>*> mRemoveQueue;
};
