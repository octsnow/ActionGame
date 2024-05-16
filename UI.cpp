#include "UI.hpp"
#include <iostream>

using namespace std;

void Status::Update(OctGame* pOctGame, StatusData& data) {
    pOctGame->Text(0, 0, RGB(0xFF, 0xFF, 0xFF), "coin: %d", data.coin);
    if(data.hp > 0) {
        pOctGame->DrawBox(100, 0, 100 + data.hp, 30, 0x00FF00, true);
    }
}

Menu::Menu() {
    this->mInventoryTop = 0;
    this->mX = 0;
    this->mY = 0;
    this->mIsPulldown = false;

    for(int i = 0; i < INVENTORY_W * INVENTORY_H; i++) {
        this->mInventory[i] = ITEM::HAT_NONE;
    }
}

void Menu::Update(OctGame* pOctGame) {
    if(pOctGame == nullptr) return;

    // key
    if(pOctGame->IsDown('a')) {
        this->Left();
    }
    if(pOctGame->IsDown('d')) {
        this->Right();
    }
    if(pOctGame->IsDown('w')) {
        this->Up();
    }
    if(pOctGame->IsDown('s')) {
        this->Down();
    }
    if(pOctGame->IsDown('j')) {
        this->Select();
    }
    if(pOctGame->IsDown(KEY_ESC)) {
        this->Cansel();
    }

    // draw
    pOctGame->DrawBox(MENU_LEFTTOP_X, MENU_LEFTTOP_Y, MENU_LEFTTOP_X + MENU_W, MENU_LEFTTOP_Y + MENU_H, 0x888888, true);

    int invLtX = MENU_LEFTTOP_X + INVENTORY_LEFTTOP_X;
    int invLtY = MENU_LEFTTOP_Y + INVENTORY_LEFTTOP_Y;
    for(int x = 0; x < INVENTORY_W; x++) {
        for(int y = 0; y < INVENTORY_H; y++) {
            int alX = invLtX + (x * INVENTORY_GRIDSIZE);
            int alY = invLtY + (y * INVENTORY_GRIDSIZE);
            pOctGame->DrawBox(alX, alY, alX + INVENTORY_GRIDSIZE, alY + INVENTORY_GRIDSIZE, 0x000000);

            if(this->GetItem(x, y) != ITEM::HAT_NONE) {
                pOctGame->DrawBox(alX + 5, alY + 5, alX + INVENTORY_GRIDSIZE - 5, alY + INVENTORY_GRIDSIZE - 5, 0x8888FF, true);
            }
        }
    }

    pOctGame->DrawBox(
            invLtX + (this->mX * INVENTORY_GRIDSIZE),
            invLtY + (this->mY * INVENTORY_GRIDSIZE),
            invLtX + ((this->mX + 1) * INVENTORY_GRIDSIZE),
            invLtY + ((this->mY + 1) * INVENTORY_GRIDSIZE),
            0xFFFFFFFF);

    if(this->mIsPulldown) {
        int alX = invLtX + (this->mX * INVENTORY_GRIDSIZE) + (INVENTORY_GRIDSIZE / 2);
        int alY = invLtY + (this->mY * INVENTORY_GRIDSIZE) + (INVENTORY_GRIDSIZE / 2);
        pOctGame->DrawBox(alX, alY, alX + INVENTORY_PULLDOWN_W, alY + INVENTORY_PULLDOWN_H, 0xFFFFFF, true);
        pOctGame->Text(alX, alY, RGB(0x00, 0x00, 0x00), "equip");
        pOctGame->Text(alX, alY + 30, RGB(0x00, 0x00, 0x00), "remove");
    }
}

void Menu::SetPosition(unsigned int x, unsigned int y) {
    if(x >= INVENTORY_W) return;
    if(y >= INVENTORY_H) return;

    this->mX = x;
    this->mY = y;
}

void Menu::AddItem(ITEM item) {
    if(item == ITEM::HAT_NONE) return;
    if(this->mInventoryTop >= INVENTORY_W * INVENTORY_H - 1) return;

    this->mInventory[this->mInventoryTop] = item;
    this->mInventoryTop++;
}

ITEM Menu::GetItem(unsigned int x, unsigned int y) {
    if(x >= INVENTORY_W || y >= INVENTORY_H) return ITEM::HAT_NONE;
    return this->mInventory[y * INVENTORY_W + x];
}

bool Menu::IsRoot() {
    return !this->mIsPulldown;
}

void Menu::Left() {
    if(this->mX == 0) return;
    if(this->mIsPulldown) return;
    this->mX--;
}

void Menu::Right() {
    if(this->mX >= INVENTORY_W - 1) return;
    if(this->mIsPulldown) return;
    this->mX++;
}

void Menu::Up() {
    if(this->mY == 0) return;
    if(this->mIsPulldown) return;
    this->mY--;
}

void Menu::Down() {
    if(this->mY >= INVENTORY_H - 1) return;
    if(this->mIsPulldown) return;
    this->mY++;
}

void Menu::Select() {
    if(this->GetItem(this->mX, this->mY) != ITEM::HAT_NONE) {
        this->mIsPulldown = true;
    }
}

void Menu::Cansel() {
    this->mIsPulldown = false;
}

UI::UI() {
    this->mIsMenu = false;
}

void UI::Update(OctGame* pOctGame, StatusData& data) {
    if(pOctGame->IsDown(KEY_ESC)) {
        if(this->IsMenu() && this->mMenu.IsRoot()) {
            this->OffMenu();
        } else if(!this->IsMenu()) {
            this->OnMenu();
        }
    }

    this->mStatus.Update(pOctGame, data);
    if(this->IsMenu()) {
        this->mMenu.Update(pOctGame);
    }
}

void UI::AddItem(ITEM item) {
    this->mMenu.AddItem(item);
}

void UI::OnMenu() {
    this->mIsMenu = true;
}

void UI::OffMenu() {
    this->mIsMenu = false;
}

bool UI::IsMenu() {
    return this->mIsMenu;
}
