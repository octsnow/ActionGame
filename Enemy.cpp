#include "Enemy.hpp"
#include <iostream>

#define MAX_SPEED_Y 50
#define X_SPEED 1

void Enemy::init() {

}

void Enemy::update() {
    if(this->getIsGround()) {
        this->setVector(1, 0);
        this->turnRight();
    }

    this->addVector(0, this->m_gravity);

    double vy = this->m_vector.y;
    if(vy > MAX_SPEED_Y) {
        this->m_vector.y = MAX_SPEED_Y;
    }
}
