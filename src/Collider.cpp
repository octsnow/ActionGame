#include "Collider.hpp"
#include <cassert>

using namespace std;

HitBox::HitBox()
    : pos(Vector2D(0, 0))
    , width(0)
    , height(0)
    , isPhysics(false)
    , mTag("") {

    this->isActive = false;
}

HitBox::HitBox(double x, double y, int width, int height, bool isPhysics, string tag)
    : pos(Vector2D(x, y))
    , width(width)
    , height(height)
    , isPhysics(isPhysics)
    , mTag(tag) {

    this->isActive = false;
}
    
HitBox::HitBox(Vector2D pos, int width, int height, bool isPhysics, string tag)
    : pos(pos)
    , width(width)
    , height(height)
    , isPhysics(isPhysics)
    , mTag(tag) {

    this->isActive = false;
}

bool HitBox::IsHitBox(const HitBox target, const Vector2D pos, const Vector2D targetPos) const {
    CollisionDetection2D::RECTANGLE r1, r2;
    r1.position = this->pos + pos;
    r1.width = this->width;
    r1.height = this->height;
    r2.position = target.pos + targetPos;
    r2.width = target.width;
    r2.height = target.height;

    return CollisionDetection2D::CheckHitRectangleRectangle(r1, r2);

    /*
    double lx, ly, lw, lh, hx, hy;
    Vector2D worldPos, worldTargetPos;

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
    */
}

string HitBox::GetTag() const {
    return this->mTag;
}

bool HitBox::CompareTag(string tag) const {
    return this->mTag == tag;
}

void Collider::AddHitBox(double x, double y, int width, int height, bool isPhysics, string tag) {
    HitBox hitbox(x, y, width, height, isPhysics, tag);
    this->mHitBoxes.push_back(hitbox);
}

void Collider::AddHitBox(Vector2D pos, int width, int height, bool isPhysics, std::string tag) {
    HitBox hitbox(pos, width, height, isPhysics, tag);
    this->mHitBoxes.push_back(hitbox);
}

vector<HitBox> Collider::GetHitBoxes() const {
    return this->mHitBoxes;
}

bool Collider::CheckHit(const Collider target, const Vector2D pos, const Vector2D targetPos) const {
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

void Collider::ActiveHitBox(int i) {
    assert(this->mHitBoxes.size() > i);
    this->mHitBoxes[i].isActive = true;
}

void Collider::DeactiveHitBox(int i) {
    assert(this->mHitBoxes.size() > i);
    this->mHitBoxes[i].isActive = false;
}
