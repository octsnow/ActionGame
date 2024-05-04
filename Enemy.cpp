#include "Enemy.hpp"
#include "Item.hpp"
#include <iostream>

using namespace std;

#define MAX_SPEED_Y 50
#define X_SPEED 1

void Enemy::Init(OctGame* pOctGame) {
    Collider collider;
    this->SetImageHandle(0, {
        pOctGame->LoadImageFile("images/slime.bmp", true)});
    this->SetSize(50, 50);
    this->SetPosition(30, 0);

    collider.AddHitBox(1, 0, 50, 50, true, false);
    this->SetCollider(collider);
    this->SetColliderSet({0});
    this->SwitchCollider(0);

    this->SetTag("Enemy");
    this->TurnRight();
    this->mMaxHP = 100;
    this->mHP = this->mMaxHP;
}

void Enemy::Update(OctGame* pOctGame) {
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

void Enemy::Draw(OctGame* game, Vector2d cameraPos) {
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

void Enemy::Damage() {
    if(this->mHP > 0) {
        this->mHP-=10;
        if(this->mHP <= 0) {
            this->PushMessage(ObjectMessage::OBJMSG_DESTROY);
            Hat* hat = new Hat();
            hat->SetPosition(this->GetPosition().x, this->GetPosition().y);
            hat->SetGravity(this->mGravity);
            this->PushObject(hat);
        }
    }
}

void Enemy::EnterObject(const Object* obj, const HitBox* hb) {
    if(obj->CompareTag("Player") && hb->isAttack) {
        this->Damage();
    }
}
