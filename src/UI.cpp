#include "UI.hpp"

using namespace std;

void Status::Update(OctGame* pOctGame) {
}

void Status::Draw(OctGame* pOctGame, StatusData& data) {
    pOctGame->DrawText(0, 0, RGB(0xFF, 0xFF, 0xFF), "coin: %d", data.coin);
    if(data.hp > 0) {
        pOctGame->DrawBox(100, 0, 100 + data.hp, 30, 0x00FF00, true);
    }
}

Menu::Menu() {
    this->mInventoryTop = 0;
    this->Init();

    for(int i = 0; i < INVENTORY_W * INVENTORY_H; i++) {
        this->mInventory[i] = ITEM_ID::ITEM_ID_NONE;
    }

    this->mGears.Hat = ITEM_ID::ITEM_ID_NONE;
}

void Menu::Init() {
    this->mX = 0;
    this->mY = 0;
    this->mIsPulldown = false;
    this->mPulldownIndex = 0;
}

void Menu::Update(OctGame* pOctGame) {
    if(pOctGame == nullptr) return;

    int x = this->mX;
    int y = this->mY;

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

    if(this->mX != x || this->mY != y) {
        pOctGame->audio.Play("Cursor");
    }
}

void Menu::Draw(OctGame* pOctGame) {
    pOctGame->DrawBox(MENU_LEFTTOP_X, MENU_LEFTTOP_Y, MENU_LEFTTOP_X + MENU_W, MENU_LEFTTOP_Y + MENU_H, 0xCFA063, true);

    int invLtX = MENU_LEFTTOP_X + INVENTORY_LEFTTOP_X;
    int invLtY = MENU_LEFTTOP_Y + INVENTORY_LEFTTOP_Y;
    for(int x = 0; x < INVENTORY_W; x++) {
        for(int y = 0; y < INVENTORY_H; y++) {
            int alX = invLtX + (x * INVENTORY_GRIDSIZE);
            int alY = invLtY + (y * INVENTORY_GRIDSIZE);
            pOctGame->DrawBox(alX, alY, alX + INVENTORY_GRIDSIZE, alY + INVENTORY_GRIDSIZE, 0x000000);

            ITEM_ID itemId = this->GetItem(x, y);
            if(itemId == ITEM_ID::ITEM_ID_NONE) continue;
            int color = 0;
            switch(itemId) {
                case ITEM_ID::ITEM_ID_NONE:
                    continue;
                case ITEM_ID::HAT_SLIME:
                    color = 0x4FBFDF;
                    break;
                case ITEM_ID::WEAPON_SLIME:
                    color = 0xFF3300;
                    break;
                case ITEM_ID::WEAPON_FIRE:
                    color = 0xFF3300;
                    break;
            }
            pOctGame->DrawBox(alX + 5, alY + 5, alX + INVENTORY_GRIDSIZE - 5, alY + INVENTORY_GRIDSIZE - 5, color, true);
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
        pOctGame->DrawBox(
            alX, alY,
            alX + INVENTORY_PULLDOWN_W, alY + INVENTORY_PULLDOWN_H,
            0xFFFFFF, true);
        pOctGame->DrawBox(
            alX, alY,
            alX + INVENTORY_PULLDOWN_W, alY + INVENTORY_PULLDOWN_H,
            0x000000);
        pOctGame->DrawBox(
            alX + 10, alY + 10 + (INVENTORY_PULLDOWN_ELEM_H * this->mPulldownIndex),
//            alX + INVENTORY_PULLDOWN_W,
            alX + 20,
//            alY + (INVENTORY_PULLDOWN_ELEM_H * (this->mPulldownIndex + 1)),
            alY + 20 + (INVENTORY_PULLDOWN_ELEM_H * this->mPulldownIndex),
            0x888888, true);
        for(int i = 0; i < this->mPulldownList.size(); i++) {
            pOctGame->DrawText(
                30 + alX, alY + (INVENTORY_PULLDOWN_ELEM_H * i),
                15, RGB(0x00, 0x00, 0x00),
                this->mPulldownList[i].name.c_str());
        }
    }
}

void Menu::SetPosition(unsigned int x, unsigned int y) {
    if(x >= INVENTORY_W) return;
    if(y >= INVENTORY_H) return;

    this->mX = x;
    this->mY = y;
}

void Menu::AddItem(ITEM_ID item) {
    if(item == ITEM_ID::ITEM_ID_NONE) return;
    if(this->mInventoryTop >= INVENTORY_W * INVENTORY_H - 1) return;

    this->mInventory[this->mInventoryTop] = item;
    this->mInventoryTop++;
}

ITEM_ID Menu::GetItem(unsigned int x, unsigned int y) {
    if(x >= INVENTORY_W || y >= INVENTORY_H) return ITEM_ID::ITEM_ID_NONE;
    return this->mInventory[y * INVENTORY_W + x];
}

ITEM_ID Menu::GetCurrentItem() {
    return this->GetItem(this->mX, this->mY);
}

Gears Menu::GetGears() {
    return this->mGears;
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
    if(this->mIsPulldown) {
        if(this->mPulldownIndex == 0) return;
        this->mPulldownIndex--;
    } else {
        if(this->mY == 0) return;
        this->mY--;
    }
}

void Menu::Down() {
    if(this->mIsPulldown) {
        if(this->mPulldownIndex >= this->mPulldownList.size() - 1) return;
        this->mPulldownIndex++;
    } else {
        if(this->mY >= INVENTORY_H - 1) return;
        this->mY++;
    }
}

void Menu::Select() {
    if(this->mIsPulldown) {
        this->mPulldownList[this->mPulldownIndex].f(this);
        this->mIsPulldown = false;
    } else {
        if(this->GetItem(this->mX, this->mY) == ITEM_ID::ITEM_ID_NONE) return;
        this->mPulldownIndex = 0;
        this->mIsPulldown = true;
    }
}

void Menu::Cansel() {
    this->mIsPulldown = false;
}

void Menu::Equip() {
    // TODO: fix condition
    ITEM_ID item = this->GetCurrentItem();
    if(item == ITEM_ID::HAT_SLIME) {
        this->mGears.Hat = item;
    }
}

void Menu::Remove() {
    // TODO: fix condition
    ITEM_ID item = this->GetCurrentItem();
    if(item == this->mGears.Hat) {
        this->mGears.Hat = ITEM_ID::ITEM_ID_NONE;
    }
}

void Menu::CallbackEquip(Menu* pMenu) {
    pMenu->Equip();
}
void Menu::CallbackRemove(Menu* pMenu) {
    pMenu->Remove();
}

UI::UI() {
    this->mIsMenu = false;
}

void UI::Update(OctGame* pOctGame) {
    if(pOctGame->IsDown(KEY_ESC)) {
        if(this->IsMenu() && this->mMenu.IsRoot()) {
            this->OffMenu();
        } else if(!this->IsMenu()) {
            this->OnMenu();
        }
    }
    this->mStatus.Update(pOctGame);
    if(this->IsMenu()) {
        this->mMenu.Update(pOctGame);
    }
}

void UI::Draw(OctGame* pOctGame, StatusData& data) {
    this->mStatus.Draw(pOctGame, data);
    if(this->IsMenu()) {
        this->mMenu.Draw(pOctGame);
    }
}

void UI::AddItem(ITEM_ID item) {
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

Gears UI::GetGears() {
    return this->mMenu.GetGears();
}
