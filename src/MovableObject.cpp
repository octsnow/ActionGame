#include "MovableObject.hpp"

MovableObject::MovableObject()
: Object()
, m_move_speed(0.2) {
}
void MovableObject::StayObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(pTargetObject->CompareTag("Player")) {
        if(pTargetHitbox->CompareTag("vacuum")) {
            Vector2d vec;
            float length;

            vec.x = pTargetObject->GetPosition().x - this->GetPosition().x;
            vec.y = pTargetObject->GetPosition().y - this->GetPosition().y;

            length = sqrt(vec.x * vec.x + vec.y * vec.y);

            vec.x /= length;
            vec.y /= length;

            vec.x *= this->m_move_speed;
            vec.y *= this->m_move_speed;

            this->AddVector(vec);
        }
    }
}
