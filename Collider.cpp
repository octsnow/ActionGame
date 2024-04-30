#include "Collider.hpp"
#include <iostream>

using namespace std;

HitBox::HitBox(double x, double y, int width, int height, bool isPhysics, bool isAttack)
    : pos(Vector2d(0, 0))
    , width(width)
    , height(height)
    , isPhysics(isPhysics)
    , isAttack(isAttack) {
    }
    
HitBox::HitBox(Vector2d pos, int width, int height, bool isPhysics, bool isAttack)
    : pos(pos)
    , width(width)
    , height(height)
    , isPhysics(isPhysics)
    , isAttack(isAttack) {
}

bool HitBox::IsHitBox(const HitBox target, const Vector2d pos, const Vector2d targetPos) const {
    int lx, ly, lw, lh, hx, hy;
    Vector2d worldPos, worldTargetPos;

    worldPos.x = this->pos.x + pos.x;
    worldPos.y = this->pos.y + pos.y;
    worldTargetPos.x = target.pos.x + targetPos.x;
    worldTargetPos.y = target.pos.y + targetPos.y;

    if(worldPos.x < worldTargetPos.x) {
        lx = worldPos.x;
        lw = this->width;
        hx = worldTargetPos.x;
    } else {
        lx = worldTargetPos.x;
        lw = target.width;
        hx = worldPos.x;
    }

    if(worldPos.y < worldTargetPos.y ){
        ly = worldPos.y;
        lh = this->height;
        hy = worldTargetPos.y;
    }else{
        ly = worldTargetPos.y;
        lh = target.height;
        hy = worldPos.y;
    }

    return ( lx <= hx && hx < lx + lw ) && ( ly <= hy && hy < ly + lh );
}

void Collider::AddHitBox(double x, double y, int width, int height, bool isPhysics, bool isAttack) {
    HitBox hitbox(x, y, width, height, isPhysics, isAttack);
    this->mHitBoxes.push_back(hitbox);
}

void Collider::AddHitBox(Vector2d pos, int width, int height, bool isPhysics, bool isAttack) {
    HitBox hitbox(pos, width, height, isPhysics, isAttack);
    this->mHitBoxes.push_back(hitbox);
}

vector<HitBox> Collider::GetHitBoxes() const {
    return this->mHitBoxes;
}

bool Collider::CheckHit(const Collider target, const Vector2d pos, const Vector2d targetPos) const {
    vector<HitBox> targetHitBoxs = target.GetHitBoxes();

    for(HitBox r : this->mHitBoxes) {
        for(HitBox tergetHitBox : targetHitBoxs) {
            if(r.IsHitBox(tergetHitBox, pos, targetPos)) {
                return true;
            }
        }
    }

    return false;
}
