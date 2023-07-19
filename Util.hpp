#pragma once
#include <vector>
#include "Object.hpp"
#include "Player.hpp"

class CollisionBox {
public:
    Vector2d pos;
    int width;
    int height;
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
        LinkedNode<Object>* node = this->m_head;
        while(node != nullptr) {
            LinkedNode<Object>* nextNode = node->m_next;
            delete node;
            node = nextNode;
        }
    }

    void append(Object v) {
        LinkedNode<Object>* newNode = new LinkedNode<Object>(v);
        
        if(this->m_head != nullptr) {
            newNode->m_next = this->m_head->m_next;
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
    LinkedNode<Object>* m_head;
};

class Stage {
public:
    void setStage(int* stage, int width, int height, int blockSize);
    void setScreenSize(int width, int height);
    int getHitVertical(Object obj);
    int getHitHorizontal(Object obj);
    bool checkHitBlock(Object* obj);
private:
    int* m_stage;
    int m_stageWidth;
    int m_stageHeight;
    int m_screenWidth;
    int m_screenHeight;
    int m_blockSize;
};

bool checkHitBox(CollisionBox c1, CollisionBox c2);
vector<LinkedNode<Object>**> checkHitObject(Player player, LinkedList<Object>& objList);
