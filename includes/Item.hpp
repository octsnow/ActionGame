#pragma once

#include "MovableObject.hpp"

enum class ITEM_ID {
    HAT_SLIME,
    WEAPON_SLIME,
    WEAPON_FIRE,
    ITEM_ID_NONE
};

class Item : public MovableObject {
public:
    Item();
    virtual void Update(OctGame* pOctGame) override;
    virtual void StayObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;

    ITEM_ID GetItemID() const;

    bool CanPickup() const;

protected:
    ITEM_ID mItemID;

private:
    clock_t mStartTime;
};

class Coin : public MovableObject {
public:
    void Init(OctGame* pOctGame) override;
    void Update(OctGame* pOctGame) override;
    void EnterObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
};

class SlimeHat : public Item {
public:
    void Init(OctGame* pOctGame) override;
};

class SlimeWeapon : public Item {
public:
    void Init(OctGame* pOctGame) override;
};

class FireWeapon : public Item {
public:
    void Init(OctGame* pOctGame) override;
};
