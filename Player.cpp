#include "Player.hpp"
#include <stdio.h>

#define MAX_SPEED_Y 50

void Player::update() {
    this->addVector(0, this->m_gravity);

    double vy = this->m_vector.y;
    if(vy > MAX_SPEED_Y) {
        this->m_vector.y = MAX_SPEED_Y;
    }
}
