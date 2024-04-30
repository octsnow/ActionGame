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
    } gGameInfo;

    const int gStageData[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    OctGame gOctGame;
    Stage gStage;
    ObjectList gObjectList(STAGE_W * BLOCK_SIZE, STAGE_H * BLOCK_SIZE);
    Player* gPPlayer;

    void Update() {
        int playerAnimNum = 0;
        Vector2d pPos, pVec;
        
        // move when a or d is pressed
        if(gOctGame.IsPressed('a')) {
            gPPlayer->AddVector(-1, 0);
            gPPlayer->TurnLeft();
            if(gPPlayer->GetVector().x < -MAX_SPEED_X) {
                gPPlayer->SetVector(-MAX_SPEED_X, gPPlayer->GetVector().y);
            }
            playerAnimNum = 1;
        }
        if(gOctGame.IsPressed('d')) {
            gPPlayer->AddVector(1, 0);
            gPPlayer->TurnRight();
            if(gPPlayer->GetVector().x > MAX_SPEED_X) {
                gPPlayer->SetVector(MAX_SPEED_X, gPPlayer->GetVector().y);
            }
            playerAnimNum = 1;
        }
        if(gOctGame.IsDown('j')) {
            gPPlayer->Attack();
            playerAnimNum = 2;
        } else if(gPPlayer->IsAttacking()) {
            playerAnimNum = 2;
        }

        if(gOctGame.IsPressed(' ') && gPPlayer->IsGround()) {
            gPPlayer->SetVector(gPPlayer->GetVector().x, -JUNP_SPEED);
        }
     
        pVec = gPPlayer->GetVector();
        gPPlayer->SetAnimationNum(playerAnimNum);
        if(playerAnimNum == 0) {
            gPPlayer->AddVector(-(pVec.x != 0 ? ((int)(pVec.x > 0) * 2 - 1) : 0), 0);
            gPPlayer->SetAnimationNum(0);
        }

        // calculate camera position
        pPos = gPPlayer->GetPosition();
        Vector2d cameraPos;
        if(pPos.x < static_cast<int>(SCREEN_W / 2)) {
            cameraPos.x = 0;
        } else if(pPos.x > STAGE_W * BLOCK_SIZE - static_cast<int>(SCREEN_W / 2)) {
            cameraPos.x = STAGE_W * BLOCK_SIZE - SCREEN_W;
        } else {
            cameraPos.x = pPos.x - static_cast<int>(SCREEN_W / 2);
        }
        cameraPos.y = 0;
     
        // update
        gObjectList.Update(&gOctGame, cameraPos);
        gStage.CheckHitBlock(gObjectList);
        gStage.Draw(&gOctGame, cameraPos);

        gOctGame.Text(0, 0, "coin: %d", gPPlayer->GetCoin());
        if(gPPlayer->GetHP() > 0) {
            gOctGame.DrawBox(100, 0, 100 + gPPlayer->GetHP(), 30, 0x00FF00);
        }


        gOctGame.Update();
    }

    void idle() {
        glutPostRedisplay();
    }

    void display() {
        glClear(GL_COLOR_BUFFER_BIT);

        if(gSysInf.countTime >= FrameTime) {
            gOctGame.ClearScreen();
            Update();
            gSysInf.countTime -= FrameTime;
        }
        gSysInf.countTime += time(NULL);

        gOctGame.ScreenSwap();
    }

    void resize(int w, int h) {
        glViewport(0, 0, w, h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
        
        glMatrixMode(GL_MODELVIEW);
    }

}

void Game::Start(int argc, char** argv) {
    Init(argc, argv);
    glutMainLoop();
    Term();
}

void Game::Init(int argc, char** argv) {
    Enemy* enemy;
    Coin* coin;

    gPPlayer = new Player();
    enemy = new Enemy();
    coin = new Coin();

    // load image
    coin->SetImageHandle(0, {
        gOctGame.LoadImageFile("images/1-yen.bmp",
        BLOCK_SIZE / 150.0f, BLOCK_SIZE / 150.0f)});

    coin->SetSize(50, 50);
    coin->SetPosition(BLOCK_SIZE * 12, SCREEN_H - BLOCK_SIZE * 5);

    gPPlayer->SetGravity(GRAVITY);
    enemy->SetGravity(GRAVITY);



    gStage.SetStage(gStageData, STAGE_W, STAGE_H, BLOCK_SIZE);
    gStage.SetScreenSize(SCREEN_W, SCREEN_H);

    gOctGame.Init(&argc, argv, SCREEN_W, SCREEN_H);
    gOctGame.DisplayFunc(display);
    gOctGame.ReshapeFunc(resize);
    gOctGame.IdleFunc(idle);

    gObjectList.AppendObject(coin);
    gObjectList.AppendObject(gPPlayer);
    gObjectList.AppendObject(enemy);
    gObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
            Object* pObject = node->GetValue()->pObject;
            pObject->Init(&gOctGame);
    });

    glClearColor(0.0, 0.0, 1.0, 0.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void Game::Term() {
    gOctGame.Destroy();
    gObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;

        ////////////////////// Too denger ////////////////////////////////
        if(pObject != nullptr) {
            delete pObject;
        }
    });
}


