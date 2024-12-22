#include "MovableObject.hpp"

#define MAX_SPEED 3

MovableObject::MovableObject()
: Object()
, m_move_speed(1) {
}
void MovableObject::SetMoveSpeed(float move_speed) {
    if(move_speed >= 0) {
        this->m_move_speed = move_speed;
    }
}

void MovableObject::StayObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {
    if(pTargetObject->CompareTag("Player")) {
        if(pTargetHitbox->CompareTag("vacuum")) {
            Vector2D vec;
            double length;

            vec.x = pTargetObject->GetPosition().x - this->GetPosition().x;
            vec.y = pTargetObject->GetPosition().y - this->GetPosition().y;

            length = sqrt(vec.x * vec.x + vec.y * vec.y);

            vec *= (1 / length);
            vec *= this->m_move_speed;

            if(vec.y < 0 && this->GetVector().y > 0) {
                Vector2D vec = this->GetVector();
                vec.y = 0;
                this->SetVector(vec);
            }
            this->AddVector(vec);

            if(this->GetVector().Length() > MAX_SPEED) {
                Vector2D _vec = this->GetVector();

                this->AddVector(-vec.x, -vec.y);
                if(this->GetVector().Length() < MAX_SPEED) {
                    _vec *= (1 / _vec.Length());
                    _vec *= MAX_SPEED;
                    this->SetVector(_vec);
                }
            }
        } else if(pTargetHitbox->CompareTag("blower")) {
            Vector2D vec;
            double length;

            vec.x = pTargetObject->GetPosition().x - this->GetPosition().x;
            vec.y = pTargetObject->GetPosition().y - this->GetPosition().y;

            length = sqrt(vec.x * vec.x + vec.y * vec.y);

            vec *= (1 / length);
            vec *= -this->m_move_speed;

            if(vec.y < 0 && this->GetVector().y > 0) {
                Vector2D vec = this->GetVector();
                vec.y = 0;
                this->SetVector(vec);
            }
            this->AddVector(vec);

            if(this->GetVector().Length() > MAX_SPEED) {
                Vector2D _vec = this->GetVector();

                this->AddVector(-vec.x, -vec.y);
                if(this->GetVector().Length() < MAX_SPEED) {
                    _vec *= (1 / _vec.Length());
                    _vec *= MAX_SPEED;
                    this->SetVector(_vec);
                }
            }
        }
    }
}
