#pragma once
#include "OctGame/OctGame.hpp"
#include "Collider.hpp"

class Object {
public:
    Object();

    int setImageHandle(clock_t time, std::vector<int> handles);
    int getImageHandle();

    void setAnimationNum(int n);
    void setAnimationIndex(int i);

    void setSize(int width, int height);
    int getWidth();
    int getHeight();
    Collider* getCollider();

    void setPosition(double x, double y);
    void translate(double x, double y);
    Vector2d getPosition();

    void setVector(double x, double y);
    void addVector(double x, double y);
    Vector2d getVector();
    void turnLeft();
    void turnRight();

    void updatePosition();

    void setIsGround(bool flag);
    bool getIsGround();

    void setGravity(double g);

    void init() {}
    void update() {}
    void draw(Game* game, Vector2d cameraPos);

protected:
    std::vector<clock_t> m_animationTimes;
    std::vector<std::vector<int>> m_imageHandles;

    int m_animNum;
    int m_animIndex;
    int m_lastAnimNum;

    clock_t m_lastTime;

    int m_width, m_height;
    Vector2d m_position;
    Vector2d m_vector;
    bool m_isLeft;

    double m_gravity;
    bool m_IsGround;

    Collider m_collider;
};
