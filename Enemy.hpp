#pragma once

#include "OctGame/OctGame.hpp"
#include "Object.hpp"

class Enemy : public Object {
public:
    virtual void Draw(OctGame* pOctGame, Vector2d cameraPos) override;
protected:
    int mHP;
    int mMaxHP;
};

class Slime : public Enemy {
public:
    virtual void Init(OctGame* pOctGame) override;
    virtual void Update(OctGame* pOctGame) override;
    virtual void Draw(OctGame* pOctGame, Vector2d cameraPos) override;
    void Damage();

    virtual void EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
};

class Fire : public Enemy {
    virtual void Init(OctGame* pOctGame) override;
    virtual void Update(OctGame* pOctGame) override;
    virtual void Draw(OctGame* pOctGame, Vector2d cameraPos) override;
    void Damage();

    virtual void EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
};
