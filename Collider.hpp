#pragma once
#include <vector>
#include "Util.hpp"

class HitBox {
public:
    HitBox(double x, double y, int width, int height, bool isPhysics, bool isAttack);
    HitBox(Vector2d pos, int width, int height, bool isPhysics, bool isAttack);
    bool IsHitBox(const HitBox target, const Vector2d pos, const Vector2d targetPos) const;

    Vector2d pos;
    int width;
    int height;
    bool isPhysics;
    bool isAttack;
    bool isActive;
};

class Collider {
public:
    void AddHitBox(double x, double y, int width, int height, bool isPhysics, bool isAttack);
    void AddHitBox(Vector2d pos, int width, int height, bool isPhysics, bool isAttack);
    std::vector<HitBox> GetHitBoxes() const;
    bool CheckHit(const Collider target, const Vector2d pos, const Vector2d targetPos) const;
    void ActiveHitBox(int i);
    void DeactiveHitBox(int i);

private:
    std::vector<HitBox> mHitBoxes;
};
