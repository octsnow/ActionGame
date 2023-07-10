#pragma once

typedef struct _Vector2d {
    double x, y;
} Vector2d;

typedef struct _Object {
    int imageHandle;
    int height, width;
    Vector2d position = {0, 0};
    Vector2d vector = {0, 0};
} Object;

