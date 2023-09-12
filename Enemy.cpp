#include "Enemy.hpp"
#include <iostream>

#define MAX_SPEED_Y 50
#define X_SPEED 1

void Enemy::init() {
    this->setTag("Enemy");
    this->turnRight();
    this->m_maxHP = 100;
    this->m_HP = this->m_maxHP;
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

void Enemy::draw(Game* game, Vector2d cameraPos) {
    Object::draw(game, cameraPos);
    int hpWidth = this->m_width * ((float)this->m_HP / this->m_maxHP) - cameraPos.x;

    game->drawBox(
            this->m_position.x - cameraPos.x,
            this->m_position.y - 20,
            this->m_position.x + hpWidth - cameraPos.x,
            this->m_position.y - 10,
            0x00FF00
    );

    game->drawBox(
            this->m_position.x - cameraPos.x + hpWidth,
            this->m_position.y - 20,
            this->m_position.x + this->m_width + cameraPos.x,
            this->m_position.y - 10,
            0xFF0000
    );
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
