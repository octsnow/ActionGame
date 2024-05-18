#pragma once

#include "Object.hpp"
#include "Item.hpp"

struct Gears {
    ITEM_ID Hat;
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
    ITEM_ID PopItem();

    void SetGears(Gears gears);

    bool IsAttacking();

    virtual void EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
    virtual void StayObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
private:
    clock_t mAttackCountTime;
    bool mAttackFlag;
    int mHP;
    int mCoin;
    Gears mGears;
    std::queue<ITEM_ID> mItemQueue;
};

std::vector<LinkedNode<Object*>**> CheckHitObject(Player player, LinkedList<Object*>& objList);
