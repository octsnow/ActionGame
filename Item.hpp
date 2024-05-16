#pragma once

#include "Object.hpp"

enum ITEM {
    HAT_SLIMEHAT,
    HAT_NONE
};

class Item : public Object {
public:
    Item();
    virtual void Update(OctGame* pOctGame) override;

    bool CanPicup();
private:
    clock_t mStartTime;
};

class Coin : public Object {
public:
    virtual void Init(OctGame* pOctGame) override;
    virtual void Update(OctGame* pOctGame) override;
    virtual void EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
};

class Hat : public Item {
public:
    virtual void Init(OctGame* pOctGame) override;
    virtual void EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
};
