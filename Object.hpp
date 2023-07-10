#pragma once

typedef struct _Vector2d {
    double x, y;
} Vector2d;

class Object {
public:
    Object();

    void setImageHandle(int handle);
    int getImageHandle();

    void setSize(int width, int height);
    int getWidth();
    int getHeight();

    void setPosition(double x, double y);
    void translate(double x, double y);
    Vector2d getPosition();

    void setVector(double x, double y);
    void addVector(double x, double y);
    Vector2d getVector();

private:
    int m_imageHandle;
    int m_width, m_height;
    Vector2d m_position;
    Vector2d m_vector;
};

