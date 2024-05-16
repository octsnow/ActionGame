#pragma once

#include "Object.hpp"
#include "Item.hpp"

struct Gears {
    ITEM Hat;
};

class Player : public Object {
public:
    virtual void Init(OctGame* pOctGame) override;
    virtual void Update(OctGame* pOctGame) override;
    void Attack();
    void Damage();
    void Left();
    void Right();
    void Jump();

    int GetHP();
    int GetCoin();
    ITEM PopItem();

    bool IsAttacking();

    virtual void EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
private:
    clock_t mAttackCountTime;
    bool mAttackFlag;
    int mHP;
    int mCoin;
    Gears mGears;
    std::queue<ITEM> mItemQueue;
};

std::vector<LinkedNode<Object*>**> CheckHitObject(Player player, LinkedList<Object*>& objList);
