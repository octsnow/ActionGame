#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "OctGame/OctGame.hpp"

#pragma comment(lib, "opencv_world455.lib")

using namespace std;

#define SCREEN_H 600
#define SCREEN_W 800

#define STAGE_H 12
#define STAGE_W 20

#define BLOCK_SIZE 50

#define MAX_SPEED_X 5
#define MAX_SPEED_Y 50
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
    bool isGround = false;
    unsigned int money = 0;
} gGameInf;

class ObjNode {
public:
    ObjNode(Object obj): obj(obj) {}

    Object obj;
    ObjNode* nextObj = nullptr;
};

int g1yenImgHandle;
int gGlassBlock;
Object gPlayer;
Game game;
ObjNode* objList = nullptr;

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

bool checkHitBox( int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2 ){
    int lx, ly, lw, lh, hx, hy;
    if( x1 < x2 ){
        lx = x1;
        lw = w1;
        hx = x2;
    }else{
        lx = x2;
        lw = w2;
        hx = x1;
    }

    if( y1 < y2 ){
        ly = y1;
        lh = h1;
        hy = y2;
    }else{
        ly = y2;
        lh = h2;
        hy = y1;
    }

    return ( lx <= hx && hx < lx + lw ) && ( ly <= hy && hy < ly + lh );
}

int checkGroundHead(bool isGround){
    int const& pW = gPlayer.width;
    int const& pH = gPlayer.height;
    int const  leftPx = gPlayer.position.x;
    int const  rightPx = gPlayer.position.x + pW - 1;
    int groundY = -1;
    int leftBlkX, rightBlkX, checkBlkY, offset;

    if( gPlayer.position.y + pH >= SCREEN_H ){
        return SCREEN_H - pH;
    }

    if( isGround ){
        checkBlkY = ( gPlayer.position.y + pH ) / BLOCK_SIZE;
    }else{
        checkBlkY = ( gPlayer.position.y - 1 ) / BLOCK_SIZE;
    }

    offset = checkBlkY * STAGE_W;

    leftBlkX = leftPx / BLOCK_SIZE;
    rightBlkX = rightPx / BLOCK_SIZE;

    for( int i = leftBlkX; i <= rightBlkX; i++ ){
        int index = offset + i;
        if( index < 0 || index >= STAGE_W * STAGE_H ) continue;
        if(0 < gStage[index] && gStage[index] < 3){
            if( isGround ){
                groundY = checkBlkY * BLOCK_SIZE - pH;
            }else{
                groundY = checkBlkY * BLOCK_SIZE + BLOCK_SIZE;
            }
            break;
        }
    }

    return groundY;
}

int checkWall(bool isRight){
    int const& pW = gPlayer.width;
    int const& pH = gPlayer.height;
    int const  topPy = gPlayer.position.y;
    int const  bottomPy = gPlayer.position.y + pH - 1;
    int WallX = -1;
    int topBlkY, bottomBlkY, checkBlkX;
    if( isRight ){
        checkBlkX = ( gPlayer.position.x + pW ) / BLOCK_SIZE;
    }else{
        checkBlkX = ( gPlayer.position.x - 1 ) / BLOCK_SIZE;
    }

    topBlkY = topPy / BLOCK_SIZE;
    bottomBlkY = bottomPy / BLOCK_SIZE;

    for( int i = topBlkY; i <= bottomBlkY; i++ ){
        int index = i * STAGE_W + checkBlkX;
        if( index < 0 || index >= STAGE_W * STAGE_H ) continue;
        if( 0 < gStage[index] && gStage[index] < 3 ){
            if( isRight ){
                WallX = checkBlkX * BLOCK_SIZE - pW;
            }else{
                WallX = ( checkBlkX + 1 ) * BLOCK_SIZE;
            }
        }
    }

    return WallX;
}

vector<ObjNode**> checkHitObject() {
	vector<ObjNode**> result;

	for(ObjNode** node = &objList; *node != nullptr; node = &(*node)->nextObj) {
            Object* object = &(*node)->obj;
            double x1, x2, y1, y2;
            int width, height;

            if(object->position.x < gPlayer.position.x) {
                x1 = object->position.x;
                x2 = gPlayer.position.x;
                width = object->width;
            } else {
                x1 = gPlayer.position.x;
                x2 = object->position.x;
                width = gPlayer.width;
            }

            if(object->position.y < gPlayer.position.y) {
                y1 = object->position.y;
                y2 = gPlayer.position.y;
                height = object->height;
            } else {
                y1 = gPlayer.position.y;
                y2 = object->position.y;
                height = gPlayer.height;
            }

            if(x2 - x1 < width && y2 - y1 < height) {
                result.push_back(node);
            }
	}

	return result;
}



void drawStage(){
    int scwHlf = SCREEN_W / 2;
    int schHlf = SCREEN_H / 2;
    int offX = gPlayer.position.x > scwHlf ? ( gPlayer.position.x < STAGE_W * BLOCK_SIZE - scwHlf ? gPlayer.position.x - scwHlf : STAGE_W * BLOCK_SIZE - SCREEN_W ) : 0;
    for( int y = 0; y < STAGE_H && y < SCREEN_H / BLOCK_SIZE + 1; y++ ){
        for( int x = 0; x < STAGE_W && x < SCREEN_W / BLOCK_SIZE + 1; x++ ){
            int x1 = x * BLOCK_SIZE - offX % BLOCK_SIZE,
                y1 = y * BLOCK_SIZE,
                x2 = ( x + 1 ) * BLOCK_SIZE - offX % BLOCK_SIZE,
                y2 = ( y + 1 ) * BLOCK_SIZE;
            int idx = y * STAGE_W + x + offX / BLOCK_SIZE;

            if( idx < 0 || STAGE_W * STAGE_H <= idx ) continue;
            switch( gStage[ idx ] ){
            case 1:
                game.drawImage(gGlassBlock, x1, y1);
                //game.drawBox( x1, y1, x2, y2, 0xFF0000 );
                break;
            case 2:
                game.drawBox( x1, y1, x2, y2, 0xFFFF00 );
                break;
            }
        }
    }
}

void update(){
    bool isMove = false;
    int groundY = -1, wallX = -1;
    
    if( gGameInf.ks.a ){
        gPlayer.vector.x--;
        if( gPlayer.vector.x < -MAX_SPEED_X )   gPlayer.vector.x = -MAX_SPEED_X;
        isMove = true;
    }
    if( gGameInf.ks.d ){
        gPlayer.vector.x++;
        if( gPlayer.vector.x >  MAX_SPEED_X )   gPlayer.vector.x =  MAX_SPEED_X;
        isMove = true;
    }

    if( gGameInf.ks.space && gGameInf.isGround ){
        gPlayer.vector.y -= JUNP_SPEED;
    }

    if( gPlayer.vector.y != 0 ){
        double tmpY = gPlayer.position.y;
        gPlayer.position.y += gPlayer.vector.y;
        groundY = checkGroundHead( gPlayer.vector.y > 0 );
        gPlayer.position.y = tmpY;
    }
    if( gPlayer.vector.x != 0 ){
        double tmpX = gPlayer.position.x;
        gPlayer.position.x += gPlayer.vector.x;
        wallX = checkWall( gPlayer.vector.x > 0 );
        gPlayer.position.x = tmpX;
    }

    if( groundY >= 0 ){
        gGameInf.isGround = gPlayer.vector.y > 0;
        gPlayer.vector.y  = 0;
        gPlayer.position.y = groundY;
    }else{
        gGameInf.isGround = false;
    }

    if( !isMove ){
        gPlayer.vector.x -= gPlayer.vector.x != 0 ? ( (int)( gPlayer.vector.x > 0 ) * 2 - 1 ) : 0;
    }

    if( wallX >= 0 ){
        gPlayer.vector.x = 0;
        gPlayer.position.x = wallX;
    }

    gPlayer.position.x += gPlayer.vector.x;
    gPlayer.position.y += gPlayer.vector.y;

    drawStage();

    int screenX = gPlayer.position.x < SCREEN_W / 2 ? 0 : ( gPlayer.position.x > STAGE_W * BLOCK_SIZE - SCREEN_W / 2 ? STAGE_W * BLOCK_SIZE - SCREEN_W : gPlayer.position.x - SCREEN_W / 2);
    game.drawImage(
        gPlayer.imageHandle,
//        gPlayer.x < SCREEN_W / 2 ? gPlayer.x : ( gPlayer.x > STAGE_W * BLOCK_SIZE - SCREEN_W / 2 ? gPlayer.x - STAGE_W * BLOCK_SIZE + SCREEN_W : SCREEN_W / 2),
        gPlayer.position.x - screenX,
        gPlayer.position.y );
    for(ObjNode* node = objList; node != nullptr; node = node->nextObj) {
        game.drawImage(
            node->obj.imageHandle,
            node->obj.position.x - screenX, node->obj.position.y
        );
    }
    gPlayer.vector.y += GRAVITY;
    if( gPlayer.vector.y > MAX_SPEED_Y )   gPlayer.vector.y =  MAX_SPEED_Y;

    vector<ObjNode**> hits = checkHitObject();
    for(ObjNode** o : hits) {
        if((*o)->obj.imageHandle == g1yenImgHandle) {
            gGameInf.money++;

            ObjNode* node = *o;
            *o = node->nextObj;
            delete node;
        }
    }

    char str[256];
    sprintf_s(str, 256, "%d coin", gGameInf.money);
    game.text(0, 0, str);
}

void idle(){
    glutPostRedisplay();
}

void display(){
    glClear( GL_COLOR_BUFFER_BIT );

    if( gSysInf.countTime >= FrameTime ){
        game.clearScreen();
        update();
        gSysInf.countTime -= FrameTime;
    }
    gSysInf.countTime += time( NULL );

    game.screenSwap();
}

void resize( int w, int h ){
    glViewport( 0, 0, w, h );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 30.0, (double)w / (double)h, 1.0, 100.0 );
    
    glMatrixMode( GL_MODELVIEW );
}

void init(){
    glClearColor( 0.0, 0.0, 1.0, 0.0 );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );
}

void key( unsigned char key, int x, int y ){
    switch( key ){
    case 'a': gGameInf.ks.a = true; break;
    case 'd': gGameInf.ks.d = true; break;
    case ' ': gGameInf.ks.space = true; break;
    }
}

void keyUp( unsigned char key, int x, int y ){
    switch( key ){
    case 'a': case 'A': gGameInf.ks.a = false; break;
    case 'd': case 'D': gGameInf.ks.d = false; break;
    case ' ': gGameInf.ks.space = false; break;
    }
}

int main( int argc, char** argv ){
  gPlayer.imageHandle = game.loadImage("images/player.bmp");
  gPlayer.height = 100;
  gPlayer.width = 50;

  g1yenImgHandle = game.loadImage("images/1-yen.png", BLOCK_SIZE / 150.0f, BLOCK_SIZE / 150.0f);

  gGlassBlock = game.loadRegionImage("images/mapchip2_0724/mapchip2/MapChip/kabe-ue_dungeon1.png", BLOCK_SIZE / 16.0f, BLOCK_SIZE / 16.0f, 16, 16, 3, true);

  game.init(
      &argc, argv,
      SCREEN_W, SCREEN_H
  );

  game.displayFunc(display);
  game.reshapeFunc(resize);
  game.idleFunc(idle);
  game.keyboardFunc(key);
  game.keyboardUpFunc(keyUp);

  objList = new ObjNode({g1yenImgHandle, 50, 50, 50, 0, {0, 0}});

  init();

  glutMainLoop();

  game.destroy();

  while(objList != nullptr) {
      ObjNode* node = objList;
      objList = objList->nextObj;
      delete node;
  }

  return 0;
}
