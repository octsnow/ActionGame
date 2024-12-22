#include "Player.hpp"
#include "Enemy.hpp"

#define HITBOX_BODY_WIDTH  45
#define HITBOX_BODY_HEIGHT 100
#define HITBOX_ATTACK_WIDTH  40
#define HITBOX_ATTACK_HEIGHT 100
#define HITBOX_VACUUM_WIDTH  500
#define HITBOX_VACUUM_HEIGHT 500
#define HITBOX_BLOWER_WIDTH  HITBOX_VACUUM_WIDTH
#define HITBOX_BLOWER_HEIGHT HITBOX_VACUUM_HEIGHT

#define HITBOX_BODY_OFFSET_X   ((100 - HITBOX_BODY_WIDTH) / 2.0)
#define HITBOX_ATTACK_OFFSET_X (HITBOX_BODY_OFFSET_X + HITBOX_BODY_WIDTH)

#define MAX_SPEED_X 4
#define MAX_SPEED_Y 50
#define JUNP_SPEED 20

#define COLLIDER_SET_IDLE 0
#define COLLIDER_SET_ATTACK 1
#define COLLIDER_SET_VACUUM 2
#define COLLIDER_SET_BLOWER 3

using namespace std;

namespace {
    clock_t g_lastTime;
}

void Player::Init(OctGame* pOctGame) {
    Collider collider;
    this->SetImageHandle(200, {
        pOctGame->LoadImageFile("assets/images/player1.bmp", true),
        pOctGame->LoadImageFile("assets/images/player2.bmp", true)});
    this->SetImageHandle(80, {
        pOctGame->LoadImageFile("assets/images/player_walk0.bmp", true),
        pOctGame->LoadImageFile("assets/images/player_walk1.bmp", true),
        pOctGame->LoadImageFile("assets/images/player_walk2.bmp", true),
        pOctGame->LoadImageFile("assets/images/player_walk3.bmp", true)});
    this->SetImageHandle(150, {
        pOctGame->LoadImageFile("assets/images/player_punch0.bmp", true),
        pOctGame->LoadImageFile("assets/images/player_punch1.bmp", true)});

    collider.AddHitBox(
            HITBOX_BODY_OFFSET_X,
            0,
            HITBOX_BODY_WIDTH,
            HITBOX_BODY_HEIGHT,
            true, "body");
    collider.AddHitBox(
            HITBOX_ATTACK_OFFSET_X,
            0,
            HITBOX_ATTACK_WIDTH,
            HITBOX_ATTACK_HEIGHT,
            false, "attack");
    collider.AddHitBox(
            HITBOX_BODY_OFFSET_X + HITBOX_BODY_WIDTH / 2.0 - HITBOX_VACUUM_WIDTH / 2.0,
            HITBOX_BODY_HEIGHT / 2.0 - HITBOX_VACUUM_HEIGHT / 2.0,
            HITBOX_VACUUM_WIDTH,
            HITBOX_VACUUM_HEIGHT,
            false, "vacuum");
    collider.AddHitBox(
            HITBOX_BODY_OFFSET_X + HITBOX_BODY_WIDTH / 2.0 - HITBOX_BLOWER_WIDTH / 2.0,
            HITBOX_BODY_HEIGHT / 2.0 - HITBOX_BLOWER_HEIGHT / 2.0,
            HITBOX_BLOWER_WIDTH,
            HITBOX_BLOWER_HEIGHT,
            false, "blower");
    this->SetCollider(collider);

    this->SetColliderSet({0});
    this->SetColliderSet({0, 1});
    this->SetColliderSet({0, 2});
    this->SetColliderSet({0, 3});
    this->SwitchCollider(COLLIDER_SET_IDLE);

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
    if(pOctGame->IsPressed('k')) {
        this->SwitchCollider(COLLIDER_SET_VACUUM);
    } else if(pOctGame->IsPressed('l')) {
        this->SwitchCollider(COLLIDER_SET_BLOWER);
    } else {
        if(this->IsAttacking()) {
            this->SwitchCollider(COLLIDER_SET_ATTACK);
        } else {
            this->SwitchCollider(COLLIDER_SET_IDLE);
        }
    }
    Vector2D vec = this->GetVector();
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
    this->SwitchCollider(COLLIDER_SET_VACUUM);
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

void Player::EnterObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(hitbox.CompareTag("body")) {
        if(pTargetObject->CompareTag("Enemy")) {
            const Enemy* enemy = dynamic_cast<const Enemy*>(pTargetObject);
            if(enemy == nullptr) return;
            if(enemy->GetEnemyID() == ENEMY_ID::ENEMY_FIRE) {
                this->Damage();
            }
        }
    }
}

void Player::StayObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(hitbox.CompareTag("body")) {
        if(pTargetObject->CompareTag("Item")) {
            const Item* item = dynamic_cast<const Item*>(pTargetObject);
            if(item == nullptr) return;
            if(!item->CanPickup()) return;

            this->mItemQueue.push(item->GetItemID());
        } else if(pTargetObject->CompareTag("Coin")) {
            pOctGame->audio.Play("Coin");
            this->mCoin++;
        }
    }

}
