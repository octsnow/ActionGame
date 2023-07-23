#include "Object.hpp"

Object::Object() {
    this->m_animNum = 0;
    this->m_animIndex = 0;
    this->m_lastAnimNum = 0;
    this->m_lastTime = clock();
    this->m_width = 0;
    this->m_height = 0;
    this->m_position.x = 0;
    this->m_position.y = 0;
    this->m_vector.x = 0;
    this->m_vector.y = 0;
}

int Object::setImageHandle(clock_t time, std::vector<int> handles) {
    this->m_animationTimes.push_back(time);
    this->m_imageHandles.push_back(handles);

    return this->m_imageHandles.size() - 1;
}

int Object::getImageHandle() {
    int handle =  this->m_imageHandles[this->m_animNum][this->m_animIndex];

    if(clock() - this->m_lastTime >= this->m_animationTimes[this->m_animNum]) {
        this->m_animIndex = (this->m_animIndex + 1) % this->m_imageHandles[this->m_animNum].size();
        this->m_lastTime = clock();
    }

    return handle;
}

void Object::setAnimationNum(int n) {
    if(n < 0 || n >= this->m_imageHandles.size()) return;

    this->m_animNum = n;
    if(this->m_lastAnimNum != this->m_animNum) {
        this->m_animIndex = 0;
        this->m_lastTime = clock();
    }
    this->m_lastAnimNum = this->m_animNum;
}

void Object::setAnimationIndex(int i) {
    if(i >= this->m_imageHandles[this->m_animNum].size()) return;

    this->m_animIndex = i;
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

void Object::updatePosition() {
    this->m_position.x += this->m_vector.x;
    this->m_position.y += this->m_vector.y;
}

void Object::setIsGround(bool flag) {
    this->isGround = flag;
}

bool Object::getIsGround() {
    return this->isGround;
}

void Object::setGravity(double g) {
    this->m_gravity = g;
}

void Object::draw(Game* game, Vector2d cameraPos) {
    game->drawImage(
        this->getImageHandle(),
        this->m_position.x - cameraPos.x, this->m_position.y - cameraPos.y, true, this->m_vector.x < 0);
}
