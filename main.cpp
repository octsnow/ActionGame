#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "OctGame/OctGame.hpp"
#include "Util.hpp"
#include "Object.hpp"
#include "Item.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Stage.hpp"
#include "Character.hpp"

#pragma comment(lib, "opencv_world455.lib")

using namespace std;

#define SCREEN_H 600
#define SCREEN_W 800

#define STAGE_H 12
#define STAGE_W 20

#define BLOCK_SIZE 50

#define MAX_SPEED_X 5

#define JUNP_SPEED 20
#define GRAVITY 1

#define FPS 1

#define OBJECT_COIN 0

enum class Tag {
    Player = 1,
    Enemy,
    Coin
};
const int FrameTime = 1000 / FPS;

typedef struct {
    bool a, d, space;
} KeyState;

struct {
    int countTime = 0;
} gSysInf;

struct {
    KeyState key = {false, false};
    unsigned int money = 0;
    unsigned int HP = 100;
} gGameInfo;

int g1yenImgHandle;
int gGlassBlock;
Game game;
Stage stage;
LinkedList<Object*> objList;

Player* gPlayer;

int gStage[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void isHitAttack() {
    Vector2d pPos = gPlayer->getPosition();

    for(auto pHitBox : gPlayer->getCurrentCollider()->getHitBoxes()) {
        if(pHitBox.isAttack) {
            objList.for_each([&](auto node) {
                Object* obj = (*node)->m_value;
                
                if(obj->compareTag("Enemy")) {
                    Vector2d ePos = obj->getPosition();

                    for(auto eHitBox : obj->getCurrentCollider()->getHitBoxes()) {
                        if(pHitBox.isHitBox(eHitBox, pPos, ePos)) {
                            obj->onCollision(*gPlayer, pHitBox);
                        }
                    }
                }
            });
        }
    }
}

void update() {
    bool isMove = false;
    Vector2d pPos, pVec;
    
    // move when a or d is pressed
    if(game.isPressed('a')) {
        gPlayer->addVector(-1, 0);
        gPlayer->turnLeft();
        if(gPlayer->getVector().x < -MAX_SPEED_X) {
            gPlayer->setVector(-MAX_SPEED_X, gPlayer->getVector().y);
        }
        isMove = true;
    }
    if(game.isPressed('d')) {
        gPlayer->addVector(1, 0);
        gPlayer->turnRight();
        if(gPlayer->getVector().x > MAX_SPEED_X) {
            gPlayer->setVector(MAX_SPEED_X, gPlayer->getVector().y);
        }
        isMove = true;
    }
    if(game.isDown('j')) {
        gPlayer->attack();
    }

    if(game.isPressed(' ') && gPlayer->isGround()) {
        gPlayer->setVector(gPlayer->getVector().x, -JUNP_SPEED);
    }
 
    pVec = gPlayer->getVector();
    if(isMove) {
        gPlayer->setAnimationNum(1);
    } else {
        gPlayer->addVector(-(pVec.x != 0 ? ((int)(pVec.x > 0) * 2 - 1) : 0), 0);
        gPlayer->setAnimationNum(0);
    }

    // calculate camera position
    pPos = gPlayer->getPosition();
    Vector2d cameraPos;
    if(pPos.x < static_cast<int>(SCREEN_W / 2)) {
        cameraPos.x = 0;
    } else if(pPos.x > STAGE_W * BLOCK_SIZE - static_cast<int>(SCREEN_W / 2)) {
        cameraPos.x = STAGE_W * BLOCK_SIZE - SCREEN_W;
    } else {
        cameraPos.x = pPos.x - static_cast<int>(SCREEN_W / 2);
    }
    cameraPos.y = 0;
 
    objList.for_each([&](auto node) {
            Object* obj = (*node)->m_value;
            stage.checkHitBlock(obj);
            obj->updatePosition();
            obj->draw(&game, cameraPos);
            obj->update();
    });

    stage.draw(&game, cameraPos);

    vector<LinkedNode<Object*>**> hits = checkHitObject(*gPlayer, objList);
    for(auto o : hits) {
        if((*o)->m_value->compareTag("Enemy")) {
            gGameInfo.HP--;
        } else if((*o)->m_value->compareTag("Item")) {
            gGameInfo.money++;
            objList.remove(o);
        }
    }

    isHitAttack();

    game.text(0, 0, "coin: %d", gGameInfo.money);
    game.text(100, 0, "HP: %d", gGameInfo.HP);

    game.update();
}

void idle() {
    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if(gSysInf.countTime >= FrameTime) {
        game.clearScreen();
        update();
        gSysInf.countTime -= FrameTime;
    }
    gSysInf.countTime += time(NULL);

    game.screenSwap();
}

void resize(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
}

void init(int argc, char** argv) {
    Enemy* enemy;
    Item* oneYen;
    Collider eCol, cCol;

    gPlayer = new Player();
    enemy = new Enemy();
    oneYen = new Item();

    // load image
    gPlayer->setImageHandle(0, {
        game.loadImage("images/player.bmp", true)});
    gPlayer->setImageHandle(80, {
        game.loadImage("images/player_walk0.bmp", true),
        game.loadImage("images/player_walk1.bmp", true),
        game.loadImage("images/player_walk2.bmp", true),
        game.loadImage("images/player_walk3.bmp", true)});
    enemy->setImageHandle(0, {
        game.loadImage("images/slime.bmp", true)});
    oneYen->setImageHandle(0, {
        game.loadImage("images/1-yen.bmp",
        BLOCK_SIZE / 150.0f, BLOCK_SIZE / 150.0f)});
    gGlassBlock = game.loadRegionImage(
        "images/mapchip2_0724/mapchip2/MapChip/kabe-ue_dungeon1.png",
        BLOCK_SIZE / 16.0f, BLOCK_SIZE / 16.0f, 16, 16, 3, true);

    enemy->setSize(50, 50);
    enemy->setPosition(30, 0);

    oneYen->setSize(50, 50);
    oneYen->setPosition(BLOCK_SIZE * 12, SCREEN_H - BLOCK_SIZE * 5);

    gPlayer->setGravity(GRAVITY);
    enemy->setGravity(GRAVITY);

    eCol.addHitBox({0, 0}, 50, 50, true, false);
    cCol.addHitBox({0, 0}, 50, 50, true, false);
    enemy->appendCollider(eCol);
    oneYen->appendCollider(cCol);

    stage.setStage(gStage, STAGE_W, STAGE_H, BLOCK_SIZE);
    stage.setScreenSize(SCREEN_W, SCREEN_H);

    game.init(&argc, argv, SCREEN_W, SCREEN_H);
    game.displayFunc(display);
    game.reshapeFunc(resize);
    game.idleFunc(idle);

    objList.append(oneYen);
    objList.append(gPlayer);
    objList.append(enemy);
    objList.for_each([&](auto node) {
            Object* obj = (*node)->m_value;
            obj->init();
    });

    glClearColor(0.0, 0.0, 1.0, 0.0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void term() {
    game.destroy();
    objList.for_each([&](auto node) {
        Object* obj = (*node)->m_value;

        if(obj != nullptr) {
            delete obj;
        }
    });
}

int main(int argc, char** argv) {
    init(argc, argv);
    glutMainLoop();
    term();

    return 0;
}
