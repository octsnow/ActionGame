#include "OctGame/OctGame.hpp"
#include "Item.hpp"

#define INVENTORY_LEFTTOP_X 30
#define INVENTORY_LEFTTOP_Y 30
#define INVENTORY_W 10
#define INVENTORY_H 5
#define INVENTORY_GRIDSIZE 50
#define MENU_LEFTTOP_X 30
#define MENU_LEFTTOP_Y 30
#define MENU_W (INVENTORY_W * INVENTORY_GRIDSIZE + INVENTORY_LEFTTOP_X * 2)
#define MENU_H (INVENTORY_H * INVENTORY_GRIDSIZE + INVENTORY_LEFTTOP_Y * 2)

struct StatusData {
    int coin;
    int hp;
};

class Status {
public:
    void Update(OctGame* pOctGame, StatusData& data);
};

class Menu {
public:
    Menu();
    void Update(OctGame* pOctGame);
    void SetPosition(unsigned int x, unsigned int y);
    void Left();
    void Right();
    void Up();
    void Down();

private:
    HAT mInventory[INVENTORY_W * INVENTORY_H];
    unsigned int mX, mY;
};

class UI {
public:
    UI();
    void Update(OctGame* pOctGame, StatusData& data);
    void OnMenu();
    void OffMenu();
    bool IsMenu();

private:
    bool mIsMenu;
    Menu mMenu;
    Status mStatus;
};

