#include "UI.hpp"

void Status::Update(OctGame* pOctGame, StatusData& data) {
    pOctGame->Text(0, 0, "coin: %d", data.coin);
    if(data.hp > 0) {
        pOctGame->DrawBox(100, 0, 100 + data.hp, 30, 0x00FF00, true);
    }
}

void Menu::Update(OctGame* pOctGame) {
    if(pOctGame == nullptr) return;
    pOctGame->DrawBox(30, 30, 500, 500, 0x888888);
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
