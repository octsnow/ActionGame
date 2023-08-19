#pragma once
#include "Object.hpp"

class Player : public Object {
public:
    void init();
    void update();
    void attack();
private:
    clock_t attackCountTime;
    bool isAttacking;
};

vector<LinkedNode<Object*>**> checkHitObject(Player player, LinkedList<Object*>& objList);
