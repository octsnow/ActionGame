#pragma once
#include <vector>
#include "Util.hpp"

class Rect {
public:
    bool checkHitRect(Rect target, Vector2d pos, Vector2d targetPos);

    Vector2d pos;
    int width;
    int height;
};

class Collider {
public:
    void addRect(Rect r);
    std::vector<Rect> getRects();
    bool checkHit(Collider target, Vector2d pos, Vector2d targetPos);

private:
    std::vector<Rect> m_collisionAreas;
};
