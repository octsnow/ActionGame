#pragma once

#include <queue>
#include <iostream>

enum class RANDOM_METHOD {
    RM_MIDDLE_SQUARE,
    RM_LINEAR_CONGRUENTIAL,
    RM_LFSR,
};

class Vector2D {
public:
    Vector2D() : x(0), y(0) {}
    Vector2D(double x, double y): x(x), y(y) {}
    double Length();
    double x, y;

    Vector2D& operator*=(const double& scalar);

    static double CrossProduct(Vector2D v1, Vector2D v2);
};

Vector2D operator+(const Vector2D& vector1, const Vector2D& vector2);
Vector2D operator-(const Vector2D& vector1, const Vector2D& vector2);
Vector2D operator*(const Vector2D& vector, const double& scalar);
Vector2D operator*(const double& scalar, const Vector2D& vector);

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

class Random {
public:
    Random(RANDOM_METHOD method, unsigned int seed);
    unsigned int GetRandom();

private:
    unsigned int mLastValue;
    RANDOM_METHOD mMethod;
};

namespace CollisionDetection2D {
typedef struct point_tag {
    Vector2D position;
} POINT;

typedef struct line_tag {
    POINT p1;
    POINT p2;
} LINE;

typedef struct rectangle_tag {
    Vector2D position;
    double width;
    double height;
} RECTANGLE;

// p1 -- p2
//  |    |
// p4 -- p3
typedef struct quadrangle_tag {
    POINT p1;
    POINT p2;
    POINT p3;
    POINT p4;
} QUADRANGLE;

bool CheckHitRectanglePoint(RECTANGLE r, POINT p);
bool CheckHitRectangleRectangle(RECTANGLE r1, RECTANGLE r2);
bool CheckHitLineLine(LINE l1, LINE l2);
bool CheckHitQuadranglePoint(QUADRANGLE q, POINT p);
};
