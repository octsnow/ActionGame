#pragma once

#include "OctGame.hpp"
#include "MovableObject.hpp"

enum class ENEMY_ID {
    ENEMY_SLIME,
    ENEMY_FIRE,
};

struct EnemyInitData {
    ENEMY_ID id;
    Vector2d pos;
};

class Enemy : public MovableObject {
public:
    virtual void Init(OctGame* pOctGame) override;
    virtual void Draw(OctGame* pOctGame, Camera* pCamera) override;
    virtual void Term(OctGame* pOctGame);
    virtual void Drop(OctGame* pOctGame) {};
    virtual void Damage(OctGame* pOctGame);
    ENEMY_ID GetEnemyID() const;

protected:
    int mHP;
    int mMaxHP;
    ENEMY_ID mEnemyID;
};

class Slime : public Enemy {
public:
    void Init(OctGame* pOctGame) override;
    void Update(OctGame* pOctGame) override;
    void Drop(OctGame* pOctGame) override;
    void Damage(OctGame* pOctGame) override;

    void EnterObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
};

class Fire : public Enemy {
    void Init(OctGame* pOctGame) override;
    void Update(OctGame* pOctGame) override;
    void Draw(OctGame* pOctGame, Camera* pCamera) override;
    void Damage(OctGame* pOctGame) override;

    void EnterObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
};
