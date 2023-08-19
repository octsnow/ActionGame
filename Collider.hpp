#pragma once
#include <vector>
#include "Util.hpp"

class HitBox {
public:
    HitBox(Vector2d pos, int width, int height, bool isPhysics, bool isAttack);
    bool isHitBox(HitBox target, Vector2d pos, Vector2d targetPos);

    Vector2d pos;
    int width;
    int height;
    const bool isPhysics;
    const bool isAttack;
};

class Collider {
public:
    void addHitBox(Vector2d pos, int width, int height, bool isPhysics, bool isAttack);
    std::vector<HitBox> getHitBoxes();
    bool checkHit(Collider target, Vector2d pos, Vector2d targetPos);

private:
    std::vector<HitBox> m_collisionAreas;
};
