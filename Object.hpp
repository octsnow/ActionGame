#pragma once

#include <string>

#include "OctGame/OctGame.hpp"
#include "Collider.hpp"

class Object {
public:
    // Methods
    Object();
    Object(std::string tag);

    int setImageHandle(clock_t time, std::vector<int> handles);
    int getImageHandle();

    void setAnimationNum(int n);
    void setAnimationIndex(int i);

    void setSize(int width, int height);
    int getWidth();
    int getHeight();

    void appendCollider(Collider c);
    void changeCollider(int i);
    Collider* getCurrentCollider();
    Collider* getCollider(int i);
    int getNumColliders();

    void setPosition(double x, double y);
    Vector2d getPosition();
    void translate(double x, double y);

    void setVector(double x, double y);
    Vector2d getVector();
    void addVector(double x, double y);

    void turnLeft();
    void turnRight();
    void turnOther();

    void updatePosition();

    void setIsGround(bool flag);
    void setIsWall(bool flag);
    bool isGround();
    bool isWall();

    void setGravity(double g);

    void setTag(std::string tag);
    bool compareTag(std::string tag);

    virtual void init() {};
    virtual void update() {};
    virtual void onCollision(Object obj, HitBox hb) {};
    virtual void draw(Game* game, Vector2d cameraPos);
protected:
    // Methods
    void initParams();

    // Variables
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
    bool m_isGround;
    bool m_isWall;

    std::vector<Collider> m_colliders;
    int m_colliderIndex;

    std::string m_tag;
};
