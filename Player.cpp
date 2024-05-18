#include "Player.hpp"
#include "Enemy.hpp"
#include <stdio.h>

#define MAX_SPEED_X 4
#define MAX_SPEED_Y 50
#define JUNP_SPEED 20

using namespace std;



namespace {
    clock_t g_lastTime;
}

void Player::Init(OctGame* pOctGame) {
    Collider collider, attackCollider;
    this->SetImageHandle(0, {
        pOctGame->LoadImageFile("images/player.bmp", true)});
    this->SetImageHandle(80, {
        pOctGame->LoadImageFile("images/player_walk0.bmp", true),
        pOctGame->LoadImageFile("images/player_walk1.bmp", true),
        pOctGame->LoadImageFile("images/player_walk2.bmp", true),
        pOctGame->LoadImageFile("images/player_walk3.bmp", true)});
    this->SetImageHandle(150, {
        pOctGame->LoadImageFile("images/player_punch0.bmp", true),
        pOctGame->LoadImageFile("images/player_punch1.bmp", true)});

    attackCollider.AddHitBox(20, 0, 60, 100, true, "body");
    attackCollider.AddHitBox(60, 0, 40, 100, false, "attack");
    this->SetCollider(attackCollider);
    this->SetColliderSet({0});
    this->SetColliderSet({0, 1});
    this->SwitchCollider(0);

    this->SetSize(100, 100);
    this->SetTag("Player");
    this->mHP = 100;
    this->mCoin = 0;
    this->mGears.Hat = ITEM_ID::ITEM_ID_NONE;
}

void Player::Update(OctGame* pOctGame) {
    bool isMoved = false;
    if(pOctGame->IsPressed('a') && !this->IsAttacking()) {
        this->Left();
        isMoved = true;
    }
    if(pOctGame->IsPressed('d') && !this->IsAttacking()) {
        this->Right();
        isMoved = true;
    }
    if(pOctGame->IsPressed(' ')) {
        this->Jump();
    }

    if(pOctGame->IsDown('j')) {
        this->Attack();
    }
    Vector2d vec = this->GetVector();
    if(!isMoved) {
        if(!this->IsAttacking()) {
            this->SetAnimationNum(0);
        }
        if(vec.x > 0) {
            this->AddVector(-1, 0);
        } else if(vec.x < 0) {
            this->AddVector(1, 0);
        }
    }

    double vy = this->mVector.y;
    if(vy > MAX_SPEED_Y) {
        this->mVector.y = MAX_SPEED_Y;
    }

    if(this->mAttackFlag) {
        this->mAttackCountTime += clock() - g_lastTime;
        g_lastTime = clock();

        if(this->mAttackCountTime > 300) {
            this->SwitchCollider(0);
            this->mAttackCountTime = 0;
            this->mAttackFlag = false;
        }
    }
}

void Player::Attack() {
    g_lastTime = clock();
    this->SwitchCollider(1);
    this->mAttackCountTime = 0;
    this->mAttackFlag = true;
    this->SetAnimationNum(2);
}

void Player::Damage() {
    this->mHP -= 10;
}

void Player::Left() {
    this->AddVector(-1, 0);
    this->TurnLeft();
    if(this->GetVector().x < -MAX_SPEED_X) {
        this->SetVector(-MAX_SPEED_X, this->GetVector().y);
    }
    this->SetAnimationNum(1);
}

void Player::Right() {
    this->AddVector(1, 0);
    this->TurnRight();
    if(this->GetVector().x > MAX_SPEED_X) {
        this->SetVector(MAX_SPEED_X, this->GetVector().y);
    }
    this->SetAnimationNum(1);
}

void Player::Jump() {
    if(!this->IsGround()) {
        return;
    }

    this->SetVector(this->GetVector().x, -JUNP_SPEED * (this->mGears.Hat == ITEM_ID::HAT_SLIME ? 1.5 : 1));
}

int Player::GetHP() {
    return this->mHP;
}

int Player::GetCoin() {
    return this->mCoin;
}

ITEM_ID Player::PopItem() {
    if(this->mItemQueue.empty()) {
        return ITEM_ID::ITEM_ID_NONE;
    }

    ITEM_ID item = this->mItemQueue.front();
    this->mItemQueue.pop();

    return item;
}

void Player::SetGears(Gears gears) {
    this->mGears = gears;
}

bool Player::IsAttacking() {
    return this->mAttackFlag;
}

void Player::EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(pTargetObject->CompareTag("Enemy")) {
        const Enemy* enemy = dynamic_cast<const Enemy*>(pTargetObject);
        if(enemy == nullptr) return;
        if(enemy->GetEnemyID() == ENEMY_ID::ENEMY_FIRE || !hitbox.CompareTag("attack")) {
            this->Damage();
        }
    } else if(pTargetObject->CompareTag("Coin") && !hitbox.CompareTag("attack")) {
        this->mCoin++;
    }
}

void Player::StayObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(pTargetObject->CompareTag("Item") && !hitbox.CompareTag("attack")) {
        const Item* item = dynamic_cast<const Item*>(pTargetObject);
        if(item == nullptr) return;
        if(!item->CanPickup()) return;

        this->mItemQueue.push(item->GetItemID());
    }
}
