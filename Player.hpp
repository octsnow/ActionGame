#pragma once
#include "Object.hpp"

class Player : public Object {
public:
    void update();
};

vector<LinkedNode<Object>**> checkHitObject(Player player, LinkedList<Object>& objList);
