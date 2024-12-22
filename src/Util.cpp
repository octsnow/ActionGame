#include <cmath>
#include <climits>
#include "Util.hpp"

#define LC_A 48271
#define LC_C 0
#define LC_M 0x7FFFFFFF
#define LFSR32_MASK 0x80200003

namespace {
    unsigned int MiddleSquare32(unsigned int x, unsigned int n) {
        unsigned int xx = x * x;
        unsigned int u = static_cast<unsigned int>(pow(10, static_cast<int>(n / 2)));
        unsigned int v = static_cast<unsigned int>(pow(10, n));

        return (xx / u) % v;
    }

    unsigned int LinearCongruential32(unsigned int A, unsigned int C, unsigned int M, unsigned int x) {
        unsigned int y = (A * x + C) % M;

        return y;
    }

    unsigned int LinearFeedbackShiftRegister32(unsigned int x, unsigned int mask) {
        return (x >> 1) ^ (mask * (x & 1));
    }
}

double Vector2D::Length() {
    return sqrt(this->x * this->x + this->y * this->y);
}

Vector2D& Vector2D::operator*=(const double& scalar) {
    this->x *= scalar;
    this->y *= scalar;

    return *this;
}

Vector2D operator+(const Vector2D& vector1, const Vector2D& vector2) {
    Vector2D newVector;

    newVector.x = vector1.x + vector2.x;
    newVector.y = vector1.y + vector2.y;

    return newVector;
}

Vector2D operator-(const Vector2D& vector1, const Vector2D& vector2) {
    Vector2D newVector;

    newVector.x = vector1.x - vector2.x;
    newVector.y = vector1.y - vector2.y;

    return newVector;
}

Vector2D operator*(const Vector2D& vector, const double& scalar) {
    Vector2D newVector;

    newVector.x = vector.x * scalar;
    newVector.y = vector.x * scalar;

    return newVector;
}

Vector2D operator*(const double& scalar, const Vector2D& vector) {
    return vector * scalar;
}

double Vector2D::CrossProduct(Vector2D v1, Vector2D v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

Random::Random(RANDOM_METHOD method, unsigned int seed) {
    this->mLastValue = seed;
    this->mMethod = method;
}

unsigned int Random::GetRandom() {
    unsigned int v = this->mLastValue;

    switch(this->mMethod) {
        case RANDOM_METHOD::RM_MIDDLE_SQUARE:
            v = MiddleSquare32(this->mLastValue, 4);
            break;
        case RANDOM_METHOD::RM_LINEAR_CONGRUENTIAL:
            v = LinearCongruential32(LC_A, LC_C, LC_M, this->mLastValue);
            break;
        case RANDOM_METHOD::RM_LFSR:
            v = LinearFeedbackShiftRegister32(this->mLastValue, LFSR32_MASK);
            break;
    }

    this->mLastValue = v;
    return v;
}

bool CollisionDetection2D::CheckHitRectanglePoint(CollisionDetection2D::RECTANGLE r, CollisionDetection2D::POINT p) {
    return p.position.x >= r.position.x && p.position.x - r.position.x <= r.width
        && p.position.y >= r.position.y && p.position.y - r.position.y <= r.height;
}

bool CollisionDetection2D::CheckHitRectangleRectangle(CollisionDetection2D::RECTANGLE r1, CollisionDetection2D::RECTANGLE r2) {
    double x1, x2, y1, y2, w1, h1;

    if(r1.position.x < r2.position.x) {
        x1 = r1.position.x;
        w1 = r1.width;
        x2 = r2.position.x;
    } else {
        x1 = r2.position.x;
        w1 = r2.width;
        x2 = r1.position.x;
    }

    if(r1.position.y < r2.position.y ){
        y1 = r1.position.y;
        h1 = r1.height;
        y2 = r2.position.y;
    }else{
        y1 = r2.position.y;
        h1 = r2.height;
        y2 = r1.position.y;
    }

    return (x2 - x1) < w1 && (y2 - y1) < h1;
}

bool CollisionDetection2D::CheckHitLineLine(CollisionDetection2D::LINE l1, CollisionDetection2D::LINE l2) {
    double a1 = (l1.p1.position.y - l1.p2.position.y) / (l1.p1.position.x - l1.p2.position.x);
    double a2 = (l2.p1.position.y - l2.p2.position.y) / (l2.p1.position.x - l2.p2.position.x);
    double b1 = l1.p1.position.y - a1 * l1.p1.position.x;
    double b2 = l2.p1.position.y - a2 * l2.p1.position.x;
    double x = (b2 - b1) / (a1 - a2);
    double y = a1 * x + b1;

    return l1.p1.position.x <= x && x <= l1.p2.position.x
        && l1.p1.position.y <= y && y <= l1.p2.position.y;
}

bool CollisionDetection2D::CheckHitQuadranglePoint(CollisionDetection2D::QUADRANGLE q, CollisionDetection2D::POINT p) {
    double cp1 = Vector2D::CrossProduct(
            q.p2.position - q.p1.position,
            p.position - q.p1.position);
    double cp2 = Vector2D::CrossProduct(
            q.p3.position - q.p2.position,
            p.position - q.p2.position);
    double cp3 = Vector2D::CrossProduct(
            q.p4.position - q.p3.position,
            p.position - q.p3.position);
    double cp4 = Vector2D::CrossProduct(
            q.p1.position - q.p4.position,
            p.position - q.p4.position);

    return cp1 > 0 && cp2 > 0 && cp3 > 0 && cp4 > 0;
}
