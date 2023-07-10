#include "Object.hpp"

Object::Object() {
    this->m_imageHandle = 0;
    this->m_width = 0;
    this->m_height = 0;
    this->m_position.x = 0;
    this->m_position.y = 0;
    this->m_vector.x = 0;
    this->m_vector.y = 0;
}

void Object::setImageHandle(int handle) {
    this->m_imageHandle = handle;
}

int Object::getImageHandle() {
    return this->m_imageHandle;
}

void Object::setSize(int width, int height) {
    if(width <= 0) return;
    if(height <= 0) return;

    this->m_width = width;
    this->m_height = height;
}

int Object::getWidth() {
    return this->m_width;
}

int Object::getHeight() {
    return this->m_height;
}

void Object::setPosition(double x, double y) {
    this->m_position.x = x;
    this->m_position.y = y;
}

void Object::translate(double x, double y) {
    this->m_position.x += x;
    this->m_position.y += y;
}

Vector2d Object::getPosition() {
    return this->m_position;
}

void Object::setVector(double x, double y) {
    this->m_vector.x = x;
    this->m_vector.y = y;
}

void Object::addVector(double x, double y) {
    this->m_vector.x += x;
    this->m_vector.y += y;
}

Vector2d Object::getVector() {
    return this->m_vector;
}
