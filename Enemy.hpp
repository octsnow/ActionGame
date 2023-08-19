#pragma once
#include "Object.hpp"

class Enemy : public Object {
public:
    void init();
    void update();
    void damage();

    void onCollision(Object obj, HitBox hb);

private:
    int m_HP;
};
