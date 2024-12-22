#pragma once

#include "Object.hpp"

class MovableObject : public Object {
public:
    MovableObject();
    virtual void StayObject(OctGame *pOctGame, HitBox hitbox, const Object *pTargetObject, const HitBox *pTargetHitbox) override;
protected:
    void SetMoveSpeed(float move_speed);
private:
    float m_move_speed;
};
