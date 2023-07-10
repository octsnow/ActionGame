#pragma once

template<typename T>
class LinkedNode {
public:
    LinkedNode();

    T* getValue();

    T* m_value;
    LinkedNode* m_next;
};
template<typename T>
class LinkedList {
public:
    LinkedList();
private:
    T* m_head;
};
