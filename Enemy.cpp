#include "Enemy.hpp"
#include "Item.hpp"
#include <iostream>

using namespace std;

#define MAX_SPEED_Y 50
#define X_SPEED 1

void Enemy::Draw(OctGame* pOctGame, Vector2d cameraPos) {
    Object::Draw(pOctGame, cameraPos);

    int hpWidth = this->mWidth * ((float)this->mHP / this->mMaxHP);

    pOctGame->DrawBox(
            this->mPosition.x - cameraPos.x,
            this->mPosition.y - 20,
            this->mPosition.x + hpWidth - cameraPos.x,
            this->mPosition.y - 10,
            0x00FF00, true
    );

    pOctGame->DrawBox(
            this->mPosition.x - cameraPos.x + hpWidth,
            this->mPosition.y - 20,
            this->mPosition.x + this->mWidth - cameraPos.x,
            this->mPosition.y - 10,
            0xFF0000, true
    );
}

void Slime::Init(OctGame* pOctGame) {
    Collider collider;
    this->SetImageHandle(0, {
        pOctGame->LoadImageFile("images/slime.bmp", true)});
    this->SetSize(50, 50);
    this->SetPosition(30, 0);

    collider.AddHitBox(1, 0, 50, 50, true);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);

    this->SetTag("Slime");
    this->TurnRight();
    this->mMaxHP = 100;
    this->mHP = this->mMaxHP;
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

void Slime::Draw(OctGame* game, Vector2d cameraPos) {
    Object::Draw(game, cameraPos);
    int hpWidth = this->mWidth * ((float)this->mHP / this->mMaxHP);

    game->DrawBox(
            this->mPosition.x - cameraPos.x,
            this->mPosition.y - 20,
            this->mPosition.x + hpWidth - cameraPos.x,
            this->mPosition.y - 10,
            0x00FF00, true
    );

    game->DrawBox(
            this->mPosition.x - cameraPos.x + hpWidth,
            this->mPosition.y - 20,
            this->mPosition.x + this->mWidth - cameraPos.x,
            this->mPosition.y - 10,
            0xFF0000, true
    );
}

void Slime::Damage() {
    if(this->mHP > 0) {
        this->mHP-=10;
        if(this->mHP <= 0) {
            this->PushMessage(ObjectMessage::OBJMSG_DESTROY);
            SlimeHat* hat = new SlimeHat();
            hat->SetPosition(this->GetPosition().x, this->GetPosition().y);
            hat->SetGravity(this->mGravity);
            this->PushObject(hat);
        }
    }
}

void Slime::EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(pTargetObject->CompareTag("Player") && pTargetHitbox->CompareTag("attack")) {
        this->Damage();
    }
}

void Fire::Init(OctGame* pOctGame) {
    Collider collider;
    this->SetImageHandle(0, {
        pOctGame->LoadImageFile("images/slime.bmp", true)});
    this->SetSize(50, 50);
    this->SetPosition(70, 0);

    collider.AddHitBox(1, 0, 50, 50, true);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);

    this->SetTag("Slime");
    this->TurnRight();
    this->mMaxHP = 100;
    this->mHP = this->mMaxHP;
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

void Fire::Draw(OctGame* pOctGame, Vector2d cameraPos) {
    pOctGame->DrawBox(this->mPosition.x, this->mPosition.y, this->mPosition.x + this->mWidth, this->mPosition.y + this->mHeight, 0xFF3300, true);
    Enemy::Draw(pOctGame, cameraPos);
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
    if(pTargetObject->CompareTag("Player") && pTargetHitbox->CompareTag("attack")) {
        this->Damage();
    }
}
