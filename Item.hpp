#pragma once
#include "Object.hpp"

class Coin : public Object {
public:
    virtual void Init(OctGame* pOctGame);
    virtual void Update();

    virtual void HitObject(const Object* pObject, const HitBox* pHitbox);
};

class Hat : public Object {
public:
    virtual void Init(OctGame* pOctGame);
};
