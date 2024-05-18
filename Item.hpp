#pragma once

#include "Object.hpp"

enum class ITEM_ID {
    HAT_SLIME,
    WEAPON_SLIME,
    WEAPON_FIRE,
    ITEM_ID_NONE
};

class Item : public Object {
public:
    Item();
    virtual void Update(OctGame* pOctGame) override;
    virtual void StayObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;

    ITEM_ID GetItemID() const;

    bool CanPickup() const;

protected:
    ITEM_ID mItemID;

private:
    clock_t mStartTime;
};

class Coin : public Object {
public:
    virtual void Init(OctGame* pOctGame) override;
    virtual void Update(OctGame* pOctGame) override;
    virtual void EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) override;
};

class SlimeHat : public Item {
public:
    virtual void Init(OctGame* pOctGame) override;
};

class SlimeWeapon : public Item {
public:
    virtual void Init(OctGame* pOctGame) override;
};

class FireWeapon : public Item {
public:
    virtual void Init(OctGame* pOctGame) override;
};
