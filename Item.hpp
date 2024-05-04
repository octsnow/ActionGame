#pragma once
#include "Object.hpp"

enum HAT {
    HAT_SLIMEHAT,
    HAT_NONE
};

class Item : public Object {
public:
    Item();
    virtual void Update();

    bool CanPicup();
private:
    clock_t mStartTime;
};

class Coin : public Object {
public:
    virtual void Init(OctGame* pOctGame);
    virtual void Update();

    virtual void EnterObject(const Object* pObject, const HitBox* pHitbox);
};

class Hat : public Item {
public:
    virtual void Init(OctGame* pOctGame);
    virtual void EnterObject(const Object* pObject, const HitBox* pHitbox);
};
