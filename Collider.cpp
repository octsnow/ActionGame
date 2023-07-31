#include "Collider.hpp"

using namespace std;

void Collider::addRect(Rect r) {
    this->m_collisionAreas.push_back(r);
}

vector<Rect> Collider::getRects() {
    return this->m_collisionAreas;
}

bool Collider::checkHit(Collider target, Vector2d pos, Vector2d targetPos)  {
    vector<Rect> targetRects = target.getRects();

    for(Rect r : this->m_collisionAreas) {
        for(Rect tr : targetRects) {
            if(r.checkHitRect(tr, pos, targetPos)) {
                return true;
            }
        }
    }

    return false;
}

bool Rect::checkHitRect(Rect target, Vector2d pos, Vector2d targetPos){
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


