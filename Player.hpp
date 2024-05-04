#pragma once
#include "Object.hpp"
#include "Item.hpp"

struct Gears {
    HAT Hat;
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

    bool IsAttacking();

    virtual void EnterObject(const Object* pObject, const HitBox* pHitbox) override;
private:
    clock_t mAttackCountTime;
    bool mAttackFlag;
    int mHP;
    int mCoin;
    Gears mGears;
};

std::vector<LinkedNode<Object*>**> CheckHitObject(Player player, LinkedList<Object*>& objList);
