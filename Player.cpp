#include "Player.hpp"
#include <stdio.h>

#define MAX_SPEED_Y 50

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

    collider.AddHitBox(10, 0, 50, 100, true, false);
    attackCollider.AddHitBox(10, 0, 50, 100, true, false);
    attackCollider.AddHitBox(60, 0, 20, 100, false, true);
    this->AppendCollider(collider);
    this->AppendCollider(attackCollider);

    this->SetSize(70, 100);
    this->SetTag("Player");
    this->mHP = 100;
    this->mCoin = 0;
}

void Player::Update() {
    this->AddVector(0, this->mGravity);

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
}

void Player::Damage() {
    this->mHP--;
}

int Player::GetHP() {
    return this->mHP;
}

int Player::GetCoin() {
    return this->mCoin;
}

bool Player::IsAttacking() {
    return this->mAttackFlag;
}

void Player::HitObject(const Object* pObject, const HitBox* pHitbox) {
    if(pObject->CompareTag("Enemy")) {
        this->Damage();
    } else if(pObject->CompareTag("Coin")) {
        this->mCoin++;
    }
}
