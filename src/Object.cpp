#include "Object.hpp"
#include <cassert>

using namespace std;

#define AIR_RESISTANCE 0.5

Object::Object()
: mAnimNum(0)
, mAnimIndex(0)
, mLastAnimNum(0)
, mLastTime(clock())
, mWidth(0)
, mHeight(0)
, mPosition(0, 0)
, mOffsetPosition(0, 0)
, mVector(0, 0)
, mIsLeft(false)
, mColliderIndex(0)
, mIsMoved(false)
, mGravity(0)
, mWeight(1) {
    //this->InitParams();
    this->SetTag("");
}

Object::Object(std::string tag)
: mAnimNum(0)
, mAnimIndex(0)
, mLastAnimNum(0)
, mLastTime(clock())
, mWidth(0)
, mHeight(0)
, mPosition(0, 0)
, mOffsetPosition(0, 0)
, mVector(0, 0)
, mIsLeft(false)
, mColliderIndex(0)
, mIsMoved(false)
, mGravity(0)
, mWeight(1) {
    //this->InitParams();
    this->SetTag(tag);
}

int Object::SetImageHandle(clock_t time, std::vector<int> handles) {
    this->mAnimationTimes.push_back(time);
    this->mImageHandles.push_back(handles);

    return this->mImageHandles.size() - 1;
}

int Object::GetImageHandle() {
    int handle =  this->mImageHandles[this->mAnimNum][this->mAnimIndex];

    if(clock() - this->mLastTime >= this->mAnimationTimes[this->mAnimNum]) {
        this->mAnimIndex = (this->mAnimIndex + 1) % this->mImageHandles[this->mAnimNum].size();
        this->mLastTime = clock();
    }

    return handle;
}

void Object::SetAnimationNum(int n) {
    if(n < 0 || n >= this->mImageHandles.size()) return;

    this->mAnimNum = n;
    if(this->mLastAnimNum != this->mAnimNum) {
        this->mAnimIndex = 0;
        this->mLastTime = clock();
    }
    this->mLastAnimNum = this->mAnimNum;
}

void Object::SetAnimationIndex(int i) {
    if(i >= this->mImageHandles[this->mAnimNum].size()) return;

    this->mAnimIndex = i;
}

void Object::SetSize(int width, int height) {
    if(width <= 0) return;
    if(height <= 0) return;

    this->mWidth = width;
    this->mHeight = height;
}

int Object::GetWidth() const {
    return this->mWidth;
}

int Object::GetHeight() const {
    return this->mHeight;
}

void Object::SetCollider(Collider c) {
    this->mCollider = c;
}

void Object::SetColliderSet(vector<uint32_t> indices) {
    this->mColliderIndices.push_back(indices);
    this->mIsMoved = true;
}

void Object::SwitchCollider(int i) {
    assert(i < this->mColliderIndices.size());
    this->mColliderIndex = i;
    for(int j = 0; j < this->mCollider.GetHitBoxes().size(); j++) {
        this->mCollider.DeactiveHitBox(j);
    }
    for(int j : this->mColliderIndices[i]) {
        this->mCollider.ActiveHitBox(j);
    }
}

Collider Object::GetCollider() {
    Collider collider;
    if(this->mIsLeft) {
        vector<HitBox> hitBoxes = this->mCollider.GetHitBoxes();
        for(int i = 0; i < hitBoxes.size(); i++) {
            collider.AddHitBox(
                    this->mWidth - hitBoxes[i].pos.x - hitBoxes[i].width,
                    hitBoxes[i].pos.y,
                    hitBoxes[i].width,
                    hitBoxes[i].height,
                    hitBoxes[i].isPhysics,
                    hitBoxes[i].GetTag());
            if(hitBoxes[i].isActive) {
                collider.ActiveHitBox(i);
            }
        }
    } else {
        collider = this->mCollider;
    }

    return collider;
}

void Object::SetPosition(double x, double y) {
    if(this->mPosition.x != x || this->mPosition.y != y) {
        this->mPosition.x = x;
        this->mPosition.y = y;
        this->mIsMoved = true;
    }
}

void Object::SetPosition(Vector2D position) {
    this->SetPosition(position.x, position.y);
}

void Object::SetOffsetPosition(double x, double y) {
    this->mOffsetPosition.x = x;
    this->mOffsetPosition.y = y;
}

void Object::Translate(double x, double y) {
    this->SetPosition(this->mPosition.x + x, this->mPosition.y + y);
}

Vector2D Object::GetPosition() const {
    return this->mPosition;
}

Vector2D Object::GetLastPosition() const {
    return this->mLastPosition;
}

Vector2D Object::GetOffsetPosition() const {
    return this->mOffsetPosition;
}

void Object::SetVector(double x, double y) {
    this->mVector.x = x;
    this->mVector.y = y;
}

void Object::SetVector(Vector2D vector) {
    this->SetVector(vector.x, vector.y);
}

void Object::AddVector(double x, double y) {
    this->mVector.x += x;
    this->mVector.y += y;
}

void Object::AddVector(Vector2D vector) {
    this->AddVector(vector.x, vector.y);
}

Vector2D Object::GetTranslateVector() const {
    return this->mTranslateVector;
}

Vector2D Object::GetVector() const {
    return this->mVector;
}

void Object::SetWeight(double weight) {
    if(weight < 0) {
        return;
    }

    this->mWeight = weight;
}

double Object::GetWeight() {
    return this->mWeight;
}

void Object::TurnLeft() {
    this->mIsLeft = true;
}

void Object::TurnRight() {
    this->mIsLeft = false;
}

void Object::TurnOther() {
    this->mIsLeft = !this->mIsLeft;
}

void Object::UpdatePosition() {
    this->Translate(this->mVector.x, this->mVector.y);

    if(this->mVector.x > 0) {
        double new_vec_x = this->mVector.x - AIR_RESISTANCE;
        this->mVector.x = new_vec_x > 0 ? new_vec_x : 0;
    } else if(this->mVector.x < 0) {
        double new_vec_x = this->mVector.x + AIR_RESISTANCE;
        this->mVector.x = new_vec_x < 0 ? new_vec_x : 0;
    }

    if(this->mVector.y && this->mGravity == 0) {
        double new_vec_y = this->mVector.y - AIR_RESISTANCE;
        this->mVector.y = new_vec_y > 0 ? new_vec_y : 0;
    } else if(this->mVector.y < 0) {
        double new_vec_y = this->mVector.y + AIR_RESISTANCE;
        this->mVector.y = new_vec_y < 0 ? new_vec_y : 0;
    }
}

void Object::UpdateLastPosition() {
    this->mLastPosition = this->mPosition;
}

void Object::SetIsGround(bool flag) {
    this->mIsGround = flag;
}

void Object::SetIsWall(bool flag) {
    this->mIsWall = flag;
}

void Object::SetNotMoved() {
    this->mIsMoved = false;
}

bool Object::IsGround() {
    return this->mIsGround;
}

bool Object::IsWall() {
    return this->mIsWall;
}

bool Object::IsMoved() {
    return this->mIsMoved;
}

void Object::SetGravity(double gravity) {
    this->mGravity = gravity;
}

void Object::AddGravity() {
    this->AddVector(0, this->mGravity);
}

void Object::SetTag(string tag) {
    this->mTag = tag;
}

string Object::GetTag() const {
    return this->mTag;
}

bool Object::CompareTag(string tag) const{
    return (tag == this->mTag);
}

void Object::PushMessage(ObjectMessage msg) {
    if(msg == ObjectMessage::OBJMSG_NONE) return;

    this->mMsgQue.push(msg);
}

ObjectMessage Object::PopMessage() {
    if(this->mMsgQue.size() == 0) {
        return ObjectMessage::OBJMSG_NONE;
    }

    ObjectMessage msg = this->mMsgQue.front();
    this->mMsgQue.pop();

    return msg;
}

void Object::PushObject(Object* pObject) {
    if(pObject == nullptr) return;
    this->mObjectQue.push(pObject);
}

Object* Object::PopObject() {
    if(this->mObjectQue.size() <= 0) {
        return nullptr;
    }

    Object* pObject = this->mObjectQue.front();
    this->mObjectQue.pop();

    return pObject;
}

void Object::Draw(OctGame* pOctGame, Camera* pCamera) {
    Vector2D viewPos = pCamera->CalcViewPosition(this->mPosition.x + this->mOffsetPosition.x, this->mPosition.y + this->mOffsetPosition.y);
    pOctGame->DrawImage(
        this->GetImageHandle(),
        viewPos.x, viewPos.y,
        true, this->mIsLeft);

#ifdef OCT_DEBUG
    for(HitBox hitBox : this->GetCollider().GetHitBoxes()) {
        if(!hitBox.isActive) continue;
        Vector2D ltPos, rbPos;
        ltPos = pCamera->CalcViewPosition(hitBox.pos.x + this->GetPosition().x, hitBox.pos.y + this->GetPosition().y);
        rbPos.x = ltPos.x + hitBox.width - 1;
        rbPos.y = ltPos.y + hitBox.height - 1;
        if(hitBox.CompareTag("attack")) {
            pOctGame->DrawBox(ltPos.x + 0.5, ltPos.y + 0.5, rbPos.x + 0.5, rbPos.y + 0.5, 0xFF0000);
        } else {
            pOctGame->DrawBox(ltPos.x + 0.5, ltPos.y + 0.5, rbPos.x + 0.5, rbPos.y + 0.5, 0x00FF00);
        }
    }
#endif
}

void Object::InitParams() {
    this->mAnimNum = 0;
    this->mAnimIndex = 0;
    this->mLastAnimNum = 0;
    this->mLastTime = clock();
    this->mWidth = 0;
    this->mHeight = 0;
    this->mPosition.x = 0;
    this->mPosition.y = 0;
    this->mOffsetPosition.x = 0;
    this->mOffsetPosition.y = 0;
    this->mVector.x = 0;
    this->mVector.y = 0;
    this->mIsLeft = false;
    this->mColliderIndex = 0;
    this->mIsMoved = false;
    this->mWeight = 1;
}

