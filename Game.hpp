#include "OctGame/OctGame.hpp"
#include "Util.hpp"
#include "Object.hpp"
#include "Item.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Stage.hpp"
#include "Character.hpp"

enum class Tag {
    Player = 1,
    Enemy,
    Coin
};

typedef struct {
    bool a, d, space;
} KeyState;

class Game {
public:
    void start(int argc, char** argv);
    void init(int argc, char** argv);
    void term();
};
