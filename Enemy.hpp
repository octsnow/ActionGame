#pragma once
#include "OctGame/OctGame.hpp"
#include "Object.hpp"

class Object;

class Enemy : public Object {
public:
    virtual void Init(OctGame* pOctGame) override;
    virtual void Update() override;
    virtual void Draw(OctGame* game, Vector2d cameraPos) override;
    void Damage();

    virtual void EnterObject(const Object* obj, const HitBox* hb) override;

private:
    int mHP;
    int mMaxHP;
};
