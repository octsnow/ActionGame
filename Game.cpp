#include "Game.hpp"
#include "OctGame/OctGame.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

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

using namespace std;

namespace {
    const int FrameTime = 1000 / FPS;
    struct {
        int countTime = 0;
    } gSysInf;
    struct {
        KeyState key = {false, false};
        unsigned int money = 0;
        unsigned int HP = 100;
    } gOctGameInfo;

    const int gStageData[] = {
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

    int g1yenImgHandle;
    int gGlassBlock;
    OctGame gOctGame;
    Stage gStage;
    LinkedList<Object*> gObjList;
    Player* gPlayer;

    void isHitAttack() {
        Vector2d pPos = gPlayer->getPosition();

        for(auto pHitBox : gPlayer->getCurrentCollider()->getHitBoxes()) {
            if(pHitBox.isAttack) {
                gObjList.for_each([&](auto node) {
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
        int playerAnimNum = 0;
        Vector2d pPos, pVec;
        
        // move when a or d is pressed
        if(gOctGame.isPressed('a')) {
            gPlayer->addVector(-1, 0);
            gPlayer->turnLeft();
            if(gPlayer->getVector().x < -MAX_SPEED_X) {
                gPlayer->setVector(-MAX_SPEED_X, gPlayer->getVector().y);
            }
            playerAnimNum = 1;
        }
        if(gOctGame.isPressed('d')) {
            gPlayer->addVector(1, 0);
            gPlayer->turnRight();
            if(gPlayer->getVector().x > MAX_SPEED_X) {
                gPlayer->setVector(MAX_SPEED_X, gPlayer->getVector().y);
            }
            playerAnimNum = 1;
        }
        if(gOctGame.isDown('j')) {
            gPlayer->attack();
            playerAnimNum = 2;
        } else if(gPlayer->isAttacking()) {
            playerAnimNum = 2;
        }

        if(gOctGame.isPressed(' ') && gPlayer->isGround()) {
            gPlayer->setVector(gPlayer->getVector().x, -JUNP_SPEED);
        }
     
        pVec = gPlayer->getVector();
        gPlayer->setAnimationNum(playerAnimNum);
        if(playerAnimNum == 0) {
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
     
        gObjList.for_each([&](auto node) {
            Object* obj = (*node)->m_value;
            gStage.checkHitBlock(obj);
            obj->updatePosition();
            obj->draw(&gOctGame, cameraPos);
            obj->update();
        });

        gStage.draw(&gOctGame, cameraPos);

        vector<LinkedNode<Object*>**> hits = checkHitObject(*gPlayer, gObjList);
        for(auto o : hits) {
            if((*o)->m_value->compareTag("Enemy")) {
                gOctGameInfo.HP--;
            } else if((*o)->m_value->compareTag("Item")) {
                gOctGameInfo.money++;
                gObjList.remove(o);
            }
        }

        isHitAttack();

        gOctGame.text(0, 0, "coin: %d", gOctGameInfo.money);
//        gOctGame.text(100, 0, "HP: %d", gOctGameInfo.HP);

        gOctGame.drawBox(100, 0, 100 + gOctGameInfo.HP, 30, 0x00FF00);

        vector<LinkedNode<Object*>**> destroyObjects;
        gObjList.for_each([&](auto node) {
            Object* obj = (*node)->m_value;
            while(true) {
                ObjMsg msg = obj->getMessage();

                if(msg == OBJMSG_NONE) {
                    break;
                }

                switch(msg) {
                case OBJMSG_DESTROY:
                    destroyObjects.push_back(node);
                    break;
                default:
                    break;
                }
            }
        });

        for(auto node : destroyObjects) {
            gObjList.remove(node);
        }

        gOctGame.update();
    }

    void idle() {
        glutPostRedisplay();
    }

    void display() {
        glClear(GL_COLOR_BUFFER_BIT);

        if(gSysInf.countTime >= FrameTime) {
            gOctGame.clearScreen();
            update();
            gSysInf.countTime -= FrameTime;
        }
        gSysInf.countTime += time(NULL);

        gOctGame.screenSwap();
    }

    void resize(int w, int h) {
        glViewport(0, 0, w, h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
        
        glMatrixMode(GL_MODELVIEW);
    }

}

void Game::start(int argc, char** argv) {
    init(argc, argv);
    glutMainLoop();
    term();
}

void Game::init(int argc, char** argv) {
    Enemy* enemy;
    Item* oneYen;
    Collider eCol, cCol;

    gPlayer = new Player();
    enemy = new Enemy();
    oneYen = new Item();

    // load image
    gPlayer->setImageHandle(0, {
        gOctGame.loadImage("images/player.bmp", true)});
    gPlayer->setImageHandle(80, {
        gOctGame.loadImage("images/player_walk0.bmp", true),
        gOctGame.loadImage("images/player_walk1.bmp", true),
        gOctGame.loadImage("images/player_walk2.bmp", true),
        gOctGame.loadImage("images/player_walk3.bmp", true)});
    gPlayer->setImageHandle(150, {
        gOctGame.loadImage("images/player_punch0.bmp", true),
        gOctGame.loadImage("images/player_punch1.bmp", true)});
    enemy->setImageHandle(0, {
        gOctGame.loadImage("images/slime.bmp", true)});
    oneYen->setImageHandle(0, {
        gOctGame.loadImage("images/1-yen.bmp",
        BLOCK_SIZE / 150.0f, BLOCK_SIZE / 150.0f)});
    gGlassBlock = gOctGame.loadRegionImage(
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

    gStage.setStage(gStageData, STAGE_W, STAGE_H, BLOCK_SIZE);
    gStage.setScreenSize(SCREEN_W, SCREEN_H);

    gOctGame.init(&argc, argv, SCREEN_W, SCREEN_H);
    gOctGame.displayFunc(display);
    gOctGame.reshapeFunc(resize);
    gOctGame.idleFunc(idle);

    gObjList.append(oneYen);
    gObjList.append(gPlayer);
    gObjList.append(enemy);
    gObjList.for_each([&](auto node) {
            Object* obj = (*node)->m_value;
            obj->init();
    });

    glClearColor(0.0, 0.0, 1.0, 0.0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void Game::term() {
    gOctGame.destroy();
    gObjList.for_each([&](auto node) {
        Object* obj = (*node)->m_value;

        if(obj != nullptr) {
            delete obj;
        }
    });
}


