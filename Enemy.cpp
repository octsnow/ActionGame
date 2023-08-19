#include "Enemy.hpp"
#include <iostream>

#define MAX_SPEED_Y 50
#define X_SPEED 1

void Enemy::init() {
    this->setTag("Enemy");
    this->turnRight();
    this->m_HP = 100;
}

void Enemy::update() {
    if(this->isWall()) {
        this->turnOther();
    }

    if(this->isGround()) {
        if(this->m_isLeft) {
            this->setVector(-1, 0);
        } else {
            this->setVector(1, 0);
        }
    }

    this->addVector(0, this->m_gravity);

    if(this->m_vector.y > MAX_SPEED_Y) {
        this->m_vector.y = MAX_SPEED_Y;
    }
}

void Enemy::damage() {
    this->m_HP--;
    cout << "HP: " << this->m_HP << endl;
}

void Enemy::onCollision(Object obj, HitBox hb) {
    if(obj.compareTag("Player") && hb.isAttack) {
        this->damage();
    }
}
