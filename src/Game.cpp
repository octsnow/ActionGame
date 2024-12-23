#include "OctGame.hpp"
#include "Game.hpp"
#include "Character.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Stage.hpp"
#include "UI.hpp"
#include "Gimmick.hpp"
#include "ObjectList.hpp"

#include <time.h>
#include <windows.h>

#define SCREEN_H 600
#define SCREEN_W 800
#define STAGE_H 12
#define STAGE_W 100
#define BLOCK_SIZE 50
#define GRAVITY 0.5
#define FPS 60
#define OBJECT_COIN 0

using namespace std;

namespace {
    const clock_t FrameTime = 1000 / FPS;
    struct {
        clock_t countTime = 0;
        clock_t lastTime;
    } g_sys_info;

    EnemyInitData enemyInitData[] = {
        {ENEMY_ID::ENEMY_SLIME, {30, 0}},
        {ENEMY_ID::ENEMY_SLIME, {90, 0}},
        //{ENEMY_ID::ENEMY_FIRE, {70, 0}},
    };

    OctGame g_oct_game;
    Stage g_stage;
    ObjectList g_object_list(g_stage.GetWidth() * BLOCK_SIZE, g_stage.GetHeight() * BLOCK_SIZE);
    Player* g_player;
    UI g_ui;
    bool koma_mode = false;

    void InitStageObjects(ObjectList *pObjectList, std::vector<STAGEOBJECTINFO> *pSois) {
        for(STAGEOBJECTINFO soi : *pSois) {
            switch(soi.blockId) {
                case BLOCK_ID::BID_GOAL:{
                    Goal *goal = new Goal();
                    goal->SetPosition(soi.position);
                    pObjectList->AppendObject(goal);
                    break;
                }
                default:
                    printf("InitStageObjects: invalid block id (%d)\n", soi.blockId);
                    break;
            }
        }
    }

    void Update() {
        Camera camera(g_stage.GetWidth(), g_stage.GetHeight(), BLOCK_SIZE, SCREEN_W);
        
        // calculate camera position
        camera.SetPlayerPosition(g_player->GetPosition());

        g_player->SetGears(g_ui.GetGears());
     
        // update
        StatusData statusData;
        statusData.coin = g_player->GetCoin();
        statusData.hp = g_player->GetHP();

        if(!g_ui.IsMenu()) {
            g_object_list.Update(&g_oct_game, &g_stage);
        }

        ITEM_ID item = g_player->PopItem();
        while(item != ITEM_ID::ITEM_ID_NONE) {
            g_ui.AddItem(item);
            item = g_player->PopItem();
        }
        g_ui.Update(&g_oct_game);

        g_object_list.Draw(&g_oct_game, &camera);
        g_stage.Draw(&g_oct_game, &camera);
        g_ui.Draw(&g_oct_game, statusData);
        g_oct_game.Update();
    }

    void idle() {
        glutPostRedisplay();
    }

    void display() {
        if(g_sys_info.countTime >= FrameTime) {
            glClear(GL_COLOR_BUFFER_BIT);
            g_oct_game.ClearScreen();
#ifndef OCT_DEBUG
            g_oct_game.DrawBox(0, 0, SCREEN_W, SCREEN_H, 0xFFD7B2, true);
#endif
            Update();
            g_sys_info.countTime -= FrameTime;
            g_oct_game.ScreenSwap();
        }
        g_sys_info.countTime += clock() - g_sys_info.lastTime;
        g_sys_info.lastTime = clock();
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
    g_oct_game.audio.Play("BGM");
    glutMainLoop();
    Term();
}

void Game::Init(int argc, char** argv) {
    g_oct_game.Init(&argc, argv, SCREEN_W, SCREEN_H);
    g_oct_game.DisplayFunc(display);
    g_oct_game.ReshapeFunc(resize);
    g_oct_game.IdleFunc(idle);

    g_oct_game.audio.Load("assets/sounds/H.wav", "BGM");
    g_oct_game.audio.Load("assets/sounds/damage.wav", "Damage");
    g_oct_game.audio.Load("assets/sounds/coin.wav", "Coin");
    g_oct_game.audio.Load("assets/sounds/select.wav", "Cursor");
    g_oct_game.audio.SetVolume("BGM", 0.1);
    g_oct_game.audio.SetIsLooping("BGM", true);
    g_oct_game.audio.SetVolume("Damage", 0.5);
    g_oct_game.audio.SetVolume("Coin", 0.5);
    g_oct_game.audio.SetVolume("Cursor", 0.5);

    /*
    // Initialize Enemy
    for(EnemyInitData data : enemyInitData) {
        switch(data.id) {
        case ENEMY_ID::ENEMY_SLIME:
            {
                Slime* slime = new Slime();
                slime->SetPosition(data.pos.x, data.pos.y);
                slime->SetGravity(GRAVITY);
                g_object_list.AppendObject(slime);
            }
            break;
        case ENEMY_ID::ENEMY_FIRE:
            {
                Fire* fire = new Fire();
                fire->SetPosition(data.pos.x, data.pos.y);
                fire->SetGravity(GRAVITY);
                g_object_list.AppendObject(fire);
            }
            break;
        }
    }
    */

    // Initialize Flooting
    Flooting* flooting = new Flooting();
    flooting->SetPosition(BLOCK_SIZE * 2, BLOCK_SIZE);
    g_object_list.AppendObject(flooting);

    // Initialize Coin
    Coin* coin;
    coin = new Coin();
    coin->SetPosition(BLOCK_SIZE * 12, SCREEN_H - BLOCK_SIZE * 5);
    g_object_list.AppendObject(coin);

    // Initialize Player
    g_player = new Player();
    g_player->SetPosition(BLOCK_SIZE, BLOCK_SIZE);
    g_player->SetGravity(GRAVITY);
    g_object_list.AppendObject(g_player);

    // Initialize Stage
    std::vector<STAGEOBJECTINFO> sois;
    g_stage.Init(&g_oct_game);
    //g_stage.LoadStage(&g_oct_game, "1_test.stg", BLOCK_SIZE, &sois);
    g_stage.LoadStage(&g_oct_game, "2_test.stg", BLOCK_SIZE, &sois);
    //g_stage.LoadStage(&g_oct_game, "test.stg", BLOCK_SIZE, &sois);
    InitStageObjects(&g_object_list, &sois);

    // Initialize ObjecList
    g_object_list.for_each([&](LinkedNode<ObjectListData>* node) {
            Object* pObject = node->GetValue()->pObject;
            pObject->Init(&g_oct_game);
    });

    g_stage.SetScreenSize(SCREEN_W, SCREEN_H);
    glClearColor(0.0, 0.0, 1.0, 0.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    g_sys_info.lastTime = clock();
}

void Game::Term() {
    g_oct_game.Destroy();
    g_object_list.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;

        ////////////////////// Too denger ////////////////////////////////
        if(pObject != nullptr) {
            delete pObject;
        }
    });
}


