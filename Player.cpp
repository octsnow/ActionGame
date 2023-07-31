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

vector<LinkedNode<Object>**> checkHitObject(Player player, LinkedList<Object>& objList) {
    vector<LinkedNode<Object>**> result;
    Collider* playerCollider = player.getCollider();

    objList.for_each([&](LinkedNode<Object>** node) {
        Object* object = &(*node)->m_value;

        if(playerCollider->checkHit(*object->getCollider(), player.getPosition(), object->getPosition())) {
            result.push_back(node);
        }
    });

    return result;
}
