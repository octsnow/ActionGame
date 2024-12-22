#pragma once

#include "OctGame.hpp"
#include "Item.hpp"
#include "Player.hpp"

#define INVENTORY_LEFTTOP_X 30
#define INVENTORY_LEFTTOP_Y 30
#define INVENTORY_W 10
#define INVENTORY_H 5
#define INVENTORY_GRIDSIZE 50
#define INVENTORY_PULLDOWN_W 200
#define INVENTORY_PULLDOWN_H 100
#define INVENTORY_PULLDOWN_ELEM_H 30
#define MENU_LEFTTOP_X 30
#define MENU_LEFTTOP_Y 30
#define MENU_W (INVENTORY_W * INVENTORY_GRIDSIZE + INVENTORY_LEFTTOP_X * 2)
#define MENU_H (INVENTORY_H * INVENTORY_GRIDSIZE + INVENTORY_LEFTTOP_Y * 2)

class Menu;

typedef void (*pdElemCallback)(Menu* pMenu);

struct StatusData {
    int coin;
    int hp;
};

struct PulldownElem {
    std::string name;
    pdElemCallback f;
};

class Status {
public:
    void Update(OctGame* pOctGame);
    void Draw(OctGame *pOctGame, StatusData& data);
};

class Menu {
public:
    Menu();
    void Init();
    void Update(OctGame *pOctGame);
    void Draw(OctGame *pOctGame);
    void SetPosition(unsigned int x, unsigned int y);
    void AddItem(ITEM_ID item);
    ITEM_ID GetItem(unsigned int x, unsigned int y);
    ITEM_ID GetCurrentItem();
    Gears GetGears();

    bool IsRoot();

    void Left();
    void Right();
    void Up();
    void Down();
    void Select();
    void Cansel();

    void Equip();
    void Remove();
    static void CallbackEquip(Menu *pMenu);
    static void CallbackRemove(Menu *pMenu);

private:
    ITEM_ID mInventory[INVENTORY_W * INVENTORY_H];
    unsigned int mInventoryTop;
    unsigned int mX, mY;
    unsigned int mPulldownIndex;
    bool mIsPulldown;
    std::vector<PulldownElem> mPulldownList = {
        {"equip", CallbackEquip},
        {"remove", CallbackRemove}
    };
    Gears mGears;
};

class UI {
public:
    UI();
    void Update(OctGame* pOctGame);
    void Draw(OctGame *pOctGame, StatusData& data);
    void AddItem(ITEM_ID item);
    void OnMenu();
    void OffMenu();
    bool IsMenu();
    Gears GetGears();

private:
    bool mIsMenu;
    Menu mMenu;
    Status mStatus;
};

