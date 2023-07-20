#include "Enemy.hpp"
#include <iostream>

#define MAX_SPEED_Y 50

void Enemy::init() {

}

void Enemy::update() {
    if(this->getIsGround()) {
        this->addVector(1, 0);
    }

    this->addVector(0, this->m_gravity);

    double vy = this->m_vector.y;
    if(vy > MAX_SPEED_Y) {
        this->m_vector.y = MAX_SPEED_Y;
    }
}
