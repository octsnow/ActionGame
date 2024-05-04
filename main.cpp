#include "Game.hpp"

#pragma comment(lib, "opencv_world455.lib")

/*
int FrameTime = 1000 / 60;
OctGame octGame;
Vector2d pos(0, 0);
Vector2d tPos(385, 285);
int w = 30, h = 30;
HitBox hitbox(0, 0, w, h, false, false);
HitBox target(0, 0, w, h, false, false);
int count = 0;

void Update() {
    if(hitbox.IsHitBox(target, pos, tPos)) {
        octGame.DrawBox(pos.x + hitbox.pos.x, pos.y + hitbox.pos.y, pos.x + hitbox.pos.x + w, pos.y + hitbox.pos.y + h, 0x00FF00);
        octGame.DrawBox(tPos.x + target.pos.x, tPos.y + target.pos.y, tPos.x + target.pos.x + w, tPos.y + target.pos.y + h, 0x00FF00);
    } else {
        octGame.DrawBox(pos.x + hitbox.pos.x, pos.y + hitbox.pos.y, pos.x + hitbox.pos.x + w, pos.y + hitbox.pos.y + h, 0xFF0000);
        octGame.DrawBox(tPos.x + target.pos.x, tPos.y + target.pos.y, tPos.x + target.pos.x + w, tPos.y + target.pos.y + h, 0xFF0000);
    }

    if(octGame.IsPressed('a')) {
        pos.x--;
    }
    if(octGame.IsPressed('d')) {
        pos.x++;
    }
    if(octGame.IsPressed('w')) {
        pos.y--;
    }
    if(octGame.IsPressed('s')) {
        pos.y++;
    }
}

void idle() {
    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if(count >= FrameTime) {
        octGame.ClearScreen();
        Update();
        count -= FrameTime;
    }
    count += time(NULL);

    octGame.ScreenSwap();
}

void resize(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
}
*/

int main(int argc, char** argv) {
    Game game;
    game.Start(argc, argv);


    /*
    octGame.Init(&argc, argv, 800, 600);
    octGame.DisplayFunc(display);
    octGame.ReshapeFunc(resize);
    octGame.IdleFunc(idle);

    glClearColor(0.0, 0.0, 1.0, 0.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glutMainLoop();
    octGame.Destroy();
    */

    return 0;
}
