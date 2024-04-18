#pragma once
#include "OctGame/OctGame.hpp"
#include "Object.hpp"

class Enemy : public Object {
public:
    virtual void init() override;
    virtual void update() override;
    virtual void draw(OctGame* game, Vector2d cameraPos) override;
    void damage();

    virtual void onCollision(Object obj, HitBox hb) override;

private:
    int m_HP;
    int m_maxHP;
};
