#include "Util.hpp"

template<typename T> LinkedNode<T>::LinkedNode() {
    this->m_value = nullptr;
    this->m_next = nullptr;
}

template<typename T> T* LinkedNode<T>::getValue() {
    return this->m_value;
}

template<typename T> LinkedList<T>::LinkedList() {
    this->m_head = nullptr;
}
