#include "Collider.hpp"
#include <stdio.h>

int main() {
    Vector2d vec1(136.000000, 212.646336);
    Vector2d vec2(115.255116, 312.246336);
    HitBox hitbox1(20, 0, 60, 100, true, "Player");
    HitBox hitbox2(0, 0, 100, 30, true, "Gimmick");

    printf(hitbox1.IsHitBox(hitbox2, vec1, vec2) ? "true" : "false");
    printf("\n");

    return 0;
}
