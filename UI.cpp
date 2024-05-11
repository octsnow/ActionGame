#include "UI.hpp"

void Status::Update(OctGame* pOctGame, StatusData& data) {
    pOctGame->Text(0, 0, "coin: %d", data.coin);
    if(data.hp > 0) {
        pOctGame->DrawBox(100, 0, 100 + data.hp, 30, 0x00FF00, true);
    }
}

Menu::Menu() {
    this->mX = 0;
    this->mY = 0;
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

    // draw
    pOctGame->DrawBox(MENU_LEFTTOP_X, MENU_LEFTTOP_Y, MENU_LEFTTOP_X + MENU_W, MENU_LEFTTOP_Y + MENU_H, 0x888888, true);
    for(int x = 0; x < INVENTORY_W; x++) {
        for(int y = 0; y < INVENTORY_H; y++) {
            pOctGame->DrawBox(
                    MENU_LEFTTOP_X + INVENTORY_LEFTTOP_X + (x * INVENTORY_GRIDSIZE),
                    MENU_LEFTTOP_Y + INVENTORY_LEFTTOP_Y + (y * INVENTORY_GRIDSIZE),
                    MENU_LEFTTOP_X + INVENTORY_LEFTTOP_X + ((x + 1) * INVENTORY_GRIDSIZE),
                    MENU_LEFTTOP_Y + INVENTORY_LEFTTOP_Y + ((y + 1) * INVENTORY_GRIDSIZE),
                    0x000000);
        }
    }
    pOctGame->DrawBox(
            MENU_LEFTTOP_X + INVENTORY_LEFTTOP_X + (this->mX * INVENTORY_GRIDSIZE),
            MENU_LEFTTOP_Y + INVENTORY_LEFTTOP_Y + (this->mY * INVENTORY_GRIDSIZE),
            MENU_LEFTTOP_X + INVENTORY_LEFTTOP_X + ((this->mX + 1) * INVENTORY_GRIDSIZE),
            MENU_LEFTTOP_Y + INVENTORY_LEFTTOP_Y + ((this->mY + 1) * INVENTORY_GRIDSIZE),
            0xFFFFFFFF);
}

void Menu::SetPosition(unsigned int x, unsigned int y) {
    if(x >= INVENTORY_W) return;
    if(y >= INVENTORY_H) return;

    this->mX = x;
    this->mY = y;
}

void Menu::Left() {
    if(this->mX == 0) return;
    this->mX--;
}

void Menu::Right() {
    if(this->mX >= INVENTORY_W - 1) return;
    this->mX++;
}

void Menu::Up() {
    if(this->mY == 0) return;
    this->mY--;
}

void Menu::Down() {
    if(this->mY >= INVENTORY_H - 1) return;
    this->mY++;
}

UI::UI() {
    this->mIsMenu = false;
}

void UI::Update(OctGame* pOctGame, StatusData& data) {
    this->mStatus.Update(pOctGame, data);

    if(this->IsMenu()) {
        this->mMenu.Update(pOctGame);
    }
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
