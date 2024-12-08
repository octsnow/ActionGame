#pragma once

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
    void Start(int argc, char** argv);
    void Init(int argc, char** argv);
    void Term();
};
