#pragma once
#include "Object.hpp"

class Player : public Object {
public:
    void init();
    void update();
    void attack();

    bool isAttacking();
private:
    clock_t attackCountTime;
    bool attackFlag;
};

vector<LinkedNode<Object*>**> checkHitObject(Player player, LinkedList<Object*>& objList);
