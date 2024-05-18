#pragma once

#include "OctGame/OctGame.hpp"
#include "Object.hpp"

enum class ENEMY_ID {
    ENEMY_SLIME,
    ENEMY_FIRE,
};

class Enemy : public Object {
public:
    virtual void Init(OctGame* pOctGame) override;
    virtual void Draw(OctGame* pOctGame, Camera* pCamera) override;
    ENEMY_ID GetEnemyID() const;
protected:
    int mHP;
    int mMaxHP;
    ENEMY_ID mEnemyID;
};

class Slime : public Enemy {
public:
    virtual void Init(OctGame* pOctGame) override;
    virtual void Update(OctGame* pOctGame) override;
    void Damage();

    virtual void EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
};

class Fire : public Enemy {
    virtual void Init(OctGame* pOctGame) override;
    virtual void Update(OctGame* pOctGame) override;
    virtual void Draw(OctGame* pOctGame, Camera* pCamera) override;
    void Damage();

    virtual void EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
};
