#pragma once

#include <vector>
#include <string>
#include "Util.hpp"

class HitBox {
public:
    HitBox();
    HitBox(double x, double y, int width, int height, bool isPhysics, std::string tag);
    HitBox(Vector2d pos, int width, int height, bool isPhysics, std::string tag);
    bool IsHitBox(const HitBox target, const Vector2d pos, const Vector2d targetPos) const;
    std::string GetTag() const;
    bool CompareTag(std::string tag) const;

    Vector2d pos;
    int width;
    int height;
    bool isPhysics;
    bool isActive;

private:
    std::string mTag;
};

class Collider {
public:
    void AddHitBox(double x, double y, int width, int height, bool isPhysics, std::string tag = "");
    void AddHitBox(Vector2d pos, int width, int height, bool isPhysics, std::string tag = "");
    std::vector<HitBox> GetHitBoxes() const;
    bool CheckHit(const Collider target, const Vector2d pos, const Vector2d targetPos) const;
    void ActiveHitBox(int i);
    void DeactiveHitBox(int i);

private:
    std::vector<HitBox> mHitBoxes;
};
