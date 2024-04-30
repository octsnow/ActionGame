#pragma once
#include "Object.hpp"

class Player : public Object {
public:
    virtual void Init(OctGame* pOctGame);
    virtual void Update();
    void Attack();
    void Damage();

    int GetHP();
    int GetCoin();

    bool IsAttacking();

    virtual void HitObject(const Object* pObject, const HitBox* pHitbox);
private:
    clock_t mAttackCountTime;
    bool mAttackFlag;
    int mHP;
    int mCoin;
};

std::vector<LinkedNode<Object*>**> CheckHitObject(Player player, LinkedList<Object*>& objList);
