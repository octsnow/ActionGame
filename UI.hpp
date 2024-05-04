#include "OctGame/OctGame.hpp"

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
    void Update(OctGame* pOctGame);
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

