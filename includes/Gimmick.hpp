#pragma once

#include "OctGame.hpp"
#include "MovableObject.hpp"

class Flooting : public MovableObject {
public:
    void Init(OctGame *pOctGame) override;
    void Draw(OctGame *pOctGame, Camera *pCamera) override;
};

class Goal : public Object {
public:
    void Init(OctGame *pOctGame) override;
    void Draw(OctGame *pOctGame, Camera *pCamera) override;
    void EnterObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;

    bool IsGoal() const;
private:
    bool mIsGoal;
};
