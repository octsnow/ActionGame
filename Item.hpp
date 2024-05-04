#pragma once
#include "Object.hpp"

enum HAT {
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
    virtual void EnterObject(const Object* pObject, const HitBox* pHitbox) override;
};

class Hat : public Item {
public:
    virtual void Init(OctGame* pOctGame) override;
    virtual void EnterObject(const Object* pObject, const HitBox* pHitbox) override;
};
