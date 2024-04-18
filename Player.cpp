#include "Player.hpp"
#include <stdio.h>

#define MAX_SPEED_Y 50

clock_t g_lastTime;

void Player::init() {
    Collider pCol, pAtCol;
    this->setSize(70, 100);
    pCol.addHitBox({10, 0}, 50, 100, true, false);
    pAtCol.addHitBox({10, 0}, 50, 100, true, false);
    pAtCol.addHitBox({60, 0}, 20, 100, false, true);

    this->appendCollider(pCol);
    this->appendCollider(pAtCol);

    this->setTag("Player");
}

void Player::update() {
    this->addVector(0, this->m_gravity);

    double vy = this->m_vector.y;
    if(vy > MAX_SPEED_Y) {
        this->m_vector.y = MAX_SPEED_Y;
    }

    if(this->attackFlag) {
        this->attackCountTime += clock() - g_lastTime;
        g_lastTime = clock();

        if(this->attackCountTime > 300) {
            this->changeCollider(0);
            this->attackCountTime = 0;
            this->attackFlag = false;
        }
    }
}

void Player::attack() {
    g_lastTime = clock();
    this->changeCollider(1);
    this->attackCountTime = 0;
    this->attackFlag = true;
}

bool Player::isAttacking() {
    return this->attackFlag;
}

vector<LinkedNode<Object*>**> checkHitObject(Player player, LinkedList<Object*>& objList) {
    vector<LinkedNode<Object*>**> result;
    Collider* playerCollider = player.getCurrentCollider();

    objList.for_each([&](LinkedNode<Object*>** node) {
        Object* object = (*node)->m_value;

        if(object->compareTag("Player")) {
            return;
        }

        if(playerCollider->checkHit(*object->getCurrentCollider(), player.getPosition(), object->getPosition())) {
            result.push_back(node);
        }
    });

    return result;
}
