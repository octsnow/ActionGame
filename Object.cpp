#include "Object.hpp"
#include <cassert>

using namespace std;

Object::Object() {
    this->initParams();
    this->setTag("");
}

Object::Object(std::string tag) {
    this->initParams();
    this->setTag(tag);
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

void Object::appendCollider(Collider c) {
    this->m_colliders.push_back(c);
}

void Object::changeCollider(int i) {
    this->m_colliderIndex = i;
}

Collider* Object::getCurrentCollider() {
    return &this->m_colliders[this->m_colliderIndex];
}

Collider* Object::getCollider(int i) {
    assert(i < this->m_colliders.size());

    return &this->m_colliders[i];
}

int Object::getNumColliders() {
    return this->m_colliders.size();
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

void Object::turnLeft() {
    this->m_isLeft = true;
}

void Object::turnRight() {
    this->m_isLeft = false;
}

void Object::turnOther() {
    this->m_isLeft = !this->m_isLeft;
}

void Object::updatePosition() {
    this->m_position.x += this->m_vector.x;
    this->m_position.y += this->m_vector.y;
}

void Object::setIsGround(bool flag) {
    this->m_isGround = flag;
}

void Object::setIsWall(bool flag) {
    this->m_isWall = flag;
}

bool Object::isGround() {
    return this->m_isGround;
}

bool Object::isWall() {
    return this->m_isWall;
}

void Object::setGravity(double g) {
    this->m_gravity = g;
}

void Object::setTag(string tag) {
    this->m_tag = tag;
}

bool Object::compareTag(string tag) {
    return (tag == this->m_tag);
}

void Object::appendMessage(ObjMsg msg) {
    if(msg == ObjMsg::OBJMSG_NONE) return;

    this->m_msgQue.append(msg);
}

ObjMsg Object::getMessage() {
    ObjMsg* pMsg = this->m_msgQue.getValue();
    if(pMsg == nullptr) {
        return OBJMSG_NONE;
    }

    ObjMsg msg = *pMsg;
    this->m_msgQue.pop();

    return msg;
}

void Object::draw(Game* game, Vector2d cameraPos) {
    game->drawImage(
        this->getImageHandle(),
        this->m_position.x - cameraPos.x,
        this->m_position.y - cameraPos.y,
        true, this->m_isLeft);
}

void Object::initParams() {
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
    this->m_isLeft = false;
    this->m_colliderIndex = 0;
}
