#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "OctGame/OctGame.hpp"
#include "Util.hpp"
#include "Object.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
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

const int FrameTime = 1000 / FPS;

typedef struct {
    bool a, d, space;
} KeyState;

struct {
    int countTime = 0;
} gSysInf;

struct {
    KeyState ks = {false, false};
    unsigned int money = 0;
} gGameInf;

int g1yenImgHandle;
int gGlassBlock;
Game game;
Stage stage;
LinkedList<Object> objList;

Player gPlayer;
Enemy gEnemy;

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

void drawStage(){
    int scwHlf = SCREEN_W / 2;
    int schHlf = SCREEN_H / 2;
    Vector2d pPos = gPlayer.getPosition();
    int offX = pPos.x > scwHlf ? (pPos.x < STAGE_W * BLOCK_SIZE - scwHlf ? pPos.x - scwHlf : STAGE_W * BLOCK_SIZE - SCREEN_W) : 0;
    for(int y = 0; y < STAGE_H && y < SCREEN_H / BLOCK_SIZE + 1; y++){
        for(int x = 0; x < STAGE_W && x < SCREEN_W / BLOCK_SIZE + 1; x++){
            int x1 = x * BLOCK_SIZE - offX % BLOCK_SIZE,
                y1 = y * BLOCK_SIZE,
                x2 = (x + 1) * BLOCK_SIZE - offX % BLOCK_SIZE,
                y2 = (y + 1) * BLOCK_SIZE;
            int idx = y * STAGE_W + x + offX / BLOCK_SIZE;

            if(idx < 0 || STAGE_W * STAGE_H <= idx) continue;
            switch(gStage[idx]){
            case 1:
                game.drawImage(gGlassBlock, x1, y1);
                break;
            case 2:
                game.drawBox(x1, y1, x2, y2, 0xFFFF00);
                break;
            }
        }
    }
}

void update() {
    bool isMove = false;
    Vector2d pPos, pVec;
    
    if(gGameInf.ks.a) {
        gPlayer.addVector(-1, 0);
        if(gPlayer.getVector().x < -MAX_SPEED_X) {
            gPlayer.setVector(-MAX_SPEED_X, gPlayer.getVector().y);
        }
        isMove = true;
    }
    if(gGameInf.ks.d) {
        gPlayer.addVector(1, 0);
        if(gPlayer.getVector().x > MAX_SPEED_X) {
            gPlayer.setVector(MAX_SPEED_X, gPlayer.getVector().y);
        }
        isMove = true;
    }

    stage.checkHitBlock(&gPlayer);
    if(stage.checkHitBlock(&gEnemy) & 2) {
        gEnemy.setVector(-1, 0);
    }

    if(gGameInf.ks.space && gPlayer.getIsGround()) {
        gPlayer.setVector(gPlayer.getVector().x, -JUNP_SPEED);
    }
    pVec = gPlayer.getVector();

    if(!isMove) {
        gPlayer.addVector(-(gPlayer.getVector().x != 0 ? ((int)(gPlayer.getVector().x > 0) * 2 - 1) : 0), 0);
    }

    drawStage();

    gPlayer.updatePosition();
    gEnemy.updatePosition();

    pPos = gPlayer.getPosition();
    // calculate camera position
    Vector2d cameraPos;
    cameraPos.x = pPos.x < SCREEN_W / 2 ? 0 : (pPos.x > STAGE_W * BLOCK_SIZE - SCREEN_W / 2 ? STAGE_W * BLOCK_SIZE - SCREEN_W : pPos.x - SCREEN_W / 2);
    cameraPos.y = 0;

    // draw
    gPlayer.draw(&game, cameraPos);
    gEnemy.draw(&game, cameraPos);
    objList.for_each([&](auto node) {
        Vector2d oPos = (*node)->m_value.getPosition();
        (*node)->m_value.draw(&game, cameraPos);
    });

    vector<LinkedNode<Object>**> hits = checkHitObject(gPlayer, objList);
    for(auto o : hits) {
        if((*o)->m_value.getImageHandle() == g1yenImgHandle) {
            gGameInf.money++;

            objList.remove(o);
        }
    }

    gPlayer.update();
    gEnemy.update();
    game.text(0, 0, "%d coin", gGameInf.money);
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

void init() {
    glClearColor(0.0, 0.0, 1.0, 0.0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void key(unsigned char key, int x, int y) {
    switch(key) {
    case 'a': gGameInf.ks.a = true; break;
    case 'd': gGameInf.ks.d = true; break;
    case ' ': gGameInf.ks.space = true; break;
    }
}

void keyUp(unsigned char key, int x, int y) {
    switch(key) {
    case 'a': case 'A': gGameInf.ks.a = false; break;
    case 'd': case 'D': gGameInf.ks.d = false; break;
    case ' ': gGameInf.ks.space = false; break;
    }
}

int main(int argc, char** argv) {
  gPlayer.setImageHandle(game.loadImage("images/player.bmp", true));
  gPlayer.setSize(50, 100);

  gEnemy.setImageHandle(game.loadImage("images/slime.png", true));
  gEnemy.setSize(50, 50);
  gEnemy.setPosition(30, 0);

  g1yenImgHandle = game.loadImage(
          "images/1-yen.png",
          BLOCK_SIZE / 150.0f, BLOCK_SIZE / 150.0f);

  gGlassBlock = game.loadRegionImage(
          "images/mapchip2_0724/mapchip2/MapChip/kabe-ue_dungeon1.png",
          BLOCK_SIZE / 16.0f, BLOCK_SIZE / 16.0f, 16, 16, 3, true);

  game.init(&argc, argv, SCREEN_W, SCREEN_H);

  game.displayFunc(display);
  game.reshapeFunc(resize);
  game.idleFunc(idle);
  game.keyboardFunc(key);
  game.keyboardUpFunc(keyUp);

  stage.setStage(gStage, STAGE_W, STAGE_H, BLOCK_SIZE);
  stage.setScreenSize(SCREEN_W, SCREEN_H);

  gPlayer.setGravity(GRAVITY);
  gEnemy.setGravity(GRAVITY);
  Object oneYen;
  oneYen.setImageHandle(g1yenImgHandle);
  oneYen.setSize(50, 50);
  oneYen.setPosition(50, 0);

  objList.append(oneYen);

  init();

  glutMainLoop();

  game.destroy();

  return 0;
}
