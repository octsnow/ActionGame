#include "Enemy.hpp"
#include "Item.hpp"
#include "Util.hpp"
#include <iostream>
#include <time.h>

using namespace std;

#define MAX_SPEED_Y 50
#define X_SPEED 1
#define HP_OFFSET_Y 20
#define HP_H 10
#define MAX_RANDOM_RATE 100
#define SLIME_LOST_RATE 30

namespace {
    Random random(RANDOM_METHOD::RM_LFSR, time(NULL));

    bool isHappend(int rate) {
        unsigned int r = random.GetRandom() % MAX_RANDOM_RATE;

        cout << r << endl;

        return r < rate;
    }
}

void Enemy::Init(OctGame* pOctGame) {
    this->SetTag("Enemy");
    this->TurnRight();
}

void Enemy::Draw(OctGame* pOctGame, Camera* pCamera) {
    Object::Draw(pOctGame, pCamera);

    int hpWidth = this->mWidth * ((float)this->mHP / this->mMaxHP);
    Vector2d viewPos = pCamera->CalcViewPosition(this->mPosition.x, this->mPosition.y - HP_OFFSET_Y);

    pOctGame->DrawBox(
            viewPos.x, viewPos.y,
            viewPos.x + hpWidth, viewPos.y + HP_H,
            0x00FF00, true
    );

    pOctGame->DrawBox(
            viewPos.x + hpWidth, viewPos.y,
            viewPos.x + this->mWidth, viewPos.y + HP_H,
            0xFF0000, true
    );
}

ENEMY_ID Enemy::GetEnemyID() const {
    return this->mEnemyID;
}

void Slime::Init(OctGame* pOctGame) {
    Enemy::Init(pOctGame);
    Collider collider;
    this->SetImageHandle(0, {
        pOctGame->LoadImageFile("images/slime.bmp", true)});
    this->SetSize(50, 50);
    this->SetPosition(30, 0);

    collider.AddHitBox(1, 0, 50, 50, true);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);

    this->mMaxHP = 100;
    this->mHP = this->mMaxHP;
    this->mEnemyID = ENEMY_ID::ENEMY_SLIME;
}

void Slime::Update(OctGame* pOctGame) {
    if(this->IsWall()) {
        this->TurnOther();
    }

    if(this->IsGround()) {
        if(this->mIsLeft) {
            this->SetVector(-1, 0);
        } else {
            this->SetVector(1, 0);
        }
    }

    this->AddVector(0, this->mGravity);

    if(this->mVector.y > MAX_SPEED_Y) {
        this->mVector.y = MAX_SPEED_Y;
    }
}

void Slime::Drop() {
    SlimeHat* hat = new SlimeHat();
    hat->SetPosition(this->GetPosition().x, this->GetPosition().y);
    hat->SetGravity(this->mGravity);
    this->PushObject(hat);
}

void Slime::Damage() {
    if(this->mHP > 0) {
        this->mHP-=10;
        if(this->mHP <= 0) {
            this->PushMessage(ObjectMessage::OBJMSG_DESTROY);
            if(isHappend(SLIME_LOST_RATE)) {
                this->Drop();
            }
        }
    }
}

void Slime::EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(pTargetObject->CompareTag("Player") && pTargetHitbox->CompareTag("attack")) {
        this->Damage();
    }
}

void Fire::Init(OctGame* pOctGame) {
    Enemy::Init(pOctGame);
    Collider collider;
    this->SetImageHandle(0, {
        pOctGame->LoadImageFile("images/slime.bmp", true)});
    this->SetSize(50, 50);
    this->SetPosition(70, 0);

    collider.AddHitBox(1, 0, 50, 50, true);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);

    this->mMaxHP = 100;
    this->mHP = this->mMaxHP;
    this->mEnemyID = ENEMY_ID::ENEMY_FIRE;
}

void Fire::Update(OctGame* pOctGame) {
    if(this->IsWall()) {
        this->TurnOther();
    }

    if(this->IsGround()) {
        if(this->mIsLeft) {
            this->SetVector(-1, 0);
        } else {
            this->SetVector(1, 0);
        }
    }

    this->AddVector(0, this->mGravity);

    if(this->mVector.y > MAX_SPEED_Y) {
        this->mVector.y = MAX_SPEED_Y;
    }
}

void Fire::Draw(OctGame* pOctGame, Camera* pCamera) {
    Vector2d viewPos = pCamera->CalcViewPosition(this->GetPosition());
    pOctGame->DrawBox(viewPos.x, viewPos.y, viewPos.x + this->GetWidth(), viewPos.y + this->GetWidth(), 0xFF3300, true);
    Enemy::Draw(pOctGame, pCamera);
}

void Fire::Damage() {
    if(this->mHP > 0) {
        this->mHP-=10;
        if(this->mHP <= 0) {
            this->PushMessage(ObjectMessage::OBJMSG_DESTROY);
            FireWeapon* hat = new FireWeapon();
            hat->SetPosition(this->GetPosition().x, this->GetPosition().y);
            hat->SetGravity(this->mGravity);
            this->PushObject(hat);
        }
    }
}

void Fire::EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
}
