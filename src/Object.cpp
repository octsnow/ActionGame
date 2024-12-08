#include "Object.hpp"
#include <cassert>

using namespace std;

#define AIR_RESISTANCE 0.5

namespace {
#define POW4(x) (1 << ((x) << 1))
#define START_INDEX(x) ((POW4(x) - 1) / 3)

    uint32_t BitSeparate32(uint32_t x) {
        x = (x | (x << 8)) & 0x00FF00FF;
        x = (x | (x << 4)) & 0x0F0F0F0F;
        x = (x | (x << 2)) & 0x33333333;
        x = (x | (x << 1)) & 0x55555555;

        return x;
    }
    uint32_t ToMortonNumber(const uint32_t x, const uint32_t y) {
        return BitSeparate32(x) | (BitSeparate32(y) << 1);
    }

    int DetectTreeIndex(Vector2d pos, const HitBox& hitbox, const int maxDepth, unsigned int rootWidth, unsigned int rootHeight) {

        double uWidth = (double)rootWidth / (1 << maxDepth);
        double uHeight = (double)rootHeight / (1 << maxDepth);

        Vector2d ltPos(pos.x + hitbox.pos.x, pos.y + hitbox.pos.y);
        Vector2d rbPos(ltPos.x + hitbox.width - 1, ltPos.y + hitbox.height - 1);

        if(ltPos.x < 0) ltPos.x = 0;
        if(ltPos.y < 0) ltPos.y = 0;
        if(ltPos.x >= rootWidth) ltPos.x = rootWidth - 1;
        if(ltPos.y >= rootHeight) ltPos.y = rootHeight - 1;
        if(rbPos.x < 0) rbPos.x = 0;
        if(rbPos.y < 0) rbPos.y = 0;
        if(rbPos.x >= rootWidth) rbPos.x = rootWidth - 1;
        if(rbPos.y >= rootHeight) rbPos.y = rootHeight - 1;

        uint32_t ltMorton = ToMortonNumber(ltPos.x / uWidth, ltPos.y / uHeight);
        uint32_t rbMorton = ToMortonNumber(rbPos.x / uWidth, rbPos.y / uHeight);
        uint32_t bits = ltMorton ^ rbMorton;
        int depth = maxDepth, index = 0;
        while(bits != 0) {
            depth--;
            bits >>= 2;
            ltMorton >>= 2;
        }

        return (POW4(depth) - 1) / 3 + ltMorton;
    }

    bool checkHitBox(LQTNodeNode* node, LQTNodeNode* targetNode) {
        assert(node != nullptr);
        assert(targetNode != nullptr);

        if(node->GetValue()->pObject == targetNode->GetValue()->pObject) return false;
        LQTData* d = node->GetValue();
        LQTData* td = targetNode->GetValue();
        Object* o = d->pObject;
        Object* to = td->pObject;
        HitBox h = o->GetCollider().GetHitBoxes()[d->hitboxIndex];
        HitBox th = to->GetCollider().GetHitBoxes()[td->hitboxIndex];
        HitBoxHandle handle = d->handle;
        HitBoxHandle targetHandle = td->handle;

        if(h.isActive && th.isActive && h.IsHitBox(th, o->GetPosition(), to->GetPosition())) {
            HITINFO hi;

            hi.hitbox = h;
            hi.target_object = to;
            hi.target_hitbox = th;
            o->stays.push_back(hi);

            hi.hitbox = th;
            hi.target_object = o;
            hi.target_hitbox = h;
            to->stays.push_back(hi);

            if(std::find(d->lastHitBoxes.begin(), d->lastHitBoxes.end(), targetHandle) == d->lastHitBoxes.end()) {
                hi.hitbox = h;
                hi.target_object = to;
                hi.target_hitbox = th;
                o->enters.push_back(hi);
            }

            if(std::find(td->lastHitBoxes.begin(), td->lastHitBoxes.end(), handle) == td->lastHitBoxes.end()) {
                hi.hitbox = th;
                hi.target_object = o;
                hi.target_hitbox = h;
                to->enters.push_back(hi);
            }

            return true;
        }

        return false;
    }

    void checkHitHitBoxes(LQTNodeNode* node, LQTNode& hitBoxes) {
        assert(node != nullptr);

        hitBoxes.for_each([&](LinkedNode<LQTData>* tNode) {
            if(checkHitBox(node, tNode)) {
                node->GetValue()->newHitBoxes.push_back(tNode->GetValue()->handle);
                tNode->GetValue()->newHitBoxes.push_back(node->GetValue()->handle);
            }
        });
    }

    void checkHitHitBoxes(LQTNode& hitBoxes) {
        hitBoxes.for_each([&](LinkedNode<LQTData>* node) {
            LinkedNode<LQTData>* tNode = node->mNext;

            while(tNode != nullptr) {
                if(checkHitBox(node, tNode)) {
                    node->GetValue()->newHitBoxes.push_back(tNode->GetValue()->handle);
                    tNode->GetValue()->newHitBoxes.push_back(node->GetValue()->handle);
                }
                tNode = tNode->mNext;
            }
        });
    }

    void checkHitTree(uint32_t morton, int depth, const int& maxDepth, vector<int>& checkList, LQTNode* tree) {
        uint32_t nodeIndex = START_INDEX(depth) + morton;
        checkHitHitBoxes(tree[nodeIndex]);

        if(checkList.size() > 0 && tree[nodeIndex].GetHead() != nullptr) {
            tree[nodeIndex].for_each([&](LinkedNode<LQTData>* node) {
                for(int j : checkList) {
                    checkHitHitBoxes(node, tree[j]);
                }
            });
        }

        checkList.push_back(nodeIndex);
        if(depth < maxDepth) {
            for(uint32_t i = 0; i < 4; i++) {
                checkHitTree((morton << 2) + i, depth + 1, maxDepth, checkList, tree);
            }
        }
        checkList.pop_back();

        tree[nodeIndex].for_each([&](LQTNodeNode* node) {
            node->GetValue()->lastHitBoxes = node->GetValue()->newHitBoxes;
            node->GetValue()->newHitBoxes.clear();
        });
    }
}

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

void Object::SetOffsetPosition(double x, double y) {
    this->mOffsetPosition.x = x;
    this->mOffsetPosition.y = y;
}

void Object::Translate(double x, double y) {
    this->SetPosition(this->mPosition.x + x, this->mPosition.y + y);
}

Vector2d Object::GetPosition() const {
    return this->mPosition;
}

Vector2d Object::GetLastPosition() const {
    return this->mLastPosition;
}

Vector2d Object::GetOffsetPosition() const {
    return this->mOffsetPosition;
}

void Object::SetVector(double x, double y) {
    this->mVector.x = x;
    this->mVector.y = y;
}

void Object::SetVector(Vector2d vector) {
    this->SetVector(vector.x, vector.y);
}

void Object::AddVector(double x, double y) {
    this->mVector.x += x;
    this->mVector.y += y;
}

void Object::AddVector(Vector2d vector) {
    this->AddVector(vector.x, vector.y);
}

Vector2d Object::GetVector() const {
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

    /*
    if(this->mVector.x > 0) {
        double new_vec_x = this->mVector.x - AIR_RESISTANCE;
        this->mVector.x = new_vec_x > 0 ? new_vec_x : 0;
    } else if(this->mVector.x < 0) {
        double new_vec_x = this->mVector.x + AIR_RESISTANCE;
        this->mVector.x = new_vec_x < 0 ? new_vec_x : 0;
    }

    if(this->mVector.y > 0 && this->mGravity <= 0) {
        double new_vec_y = this->mVector.y - AIR_RESISTANCE;
        this->mVector.y = new_vec_y > 0 ? new_vec_y : 0;
    } else if(this->mVector.y < 0 && this->mGravity >= 0) {
        double new_vec_y = this->mVector.y + AIR_RESISTANCE;
        this->mVector.y = new_vec_y < 0 ? new_vec_y : 0;
    }
    */
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
    Vector2d viewPos = pCamera->CalcViewPosition(this->mPosition.x + this->mOffsetPosition.x, this->mPosition.y + this->mOffsetPosition.y);
    pOctGame->DrawImage(
        this->GetImageHandle(),
        viewPos.x, viewPos.y,
        true, this->mIsLeft);

#ifdef OCT_DEBUG
    for(HitBox hitBox : this->GetCollider().GetHitBoxes()) {
        if(!hitBox.isActive) continue;
        Vector2d ltPos, rbPos;
        ltPos = pCamera->CalcViewPosition(hitBox.pos.x + this->GetPosition().x, hitBox.pos.y + this->GetPosition().y);
        rbPos.x = ltPos.x + hitBox.width - 1;
        rbPos.y = ltPos.y + hitBox.height - 1;
        if(hitBox.CompareTag("attack")) {
            pOctGame->DrawBox(ltPos.x, ltPos.y, rbPos.x, rbPos.y, 0xFF0000);
        } else {
            pOctGame->DrawBox(ltPos.x, ltPos.y, rbPos.x, rbPos.y, 0x00FF00);
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

ObjectList::ObjectList(unsigned int worldWidth, unsigned int worldHeight)
: mHitBoxList(3, worldWidth, worldHeight) {
}

void ObjectList::AppendObject(Object* pObject) {
    auto nodes = this->mHitBoxList.Append(pObject);
    ObjectListData data;
    data.pObject = pObject;
    data.lqtNodeNodes = nodes;
    this->mObjectList.Append(data);
}

void ObjectList::CheckHitObjects() const {
    this->mHitBoxList.checkHit();
}

void ObjectList::Update(OctGame* pOctGame, Camera* pCamera) {
    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        pObject->UpdatePosition();
        pObject->enters.clear();
        pObject->stays.clear();
    });
    this->CheckHitObjects();
    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        for(HITINFO hi : pObject->enters) {
            pObject->EnterObject(pOctGame, hi.hitbox, hi.target_object, &hi.target_hitbox);
        }
        for(HITINFO hi : pObject->stays) {
            if(hi.hitbox.isPhysics
            && hi.target_hitbox.isPhysics
            && pObject->GetWeight() < hi.target_object->GetWeight()
            && hi.hitbox.IsHitBox(hi.target_hitbox, pObject->GetPosition(), hi.target_object->GetPosition())) {
                Vector2d vec = pObject->GetVector(),
                         pos = pObject->GetPosition(),
                         last_pos = pObject->GetLastPosition(),
                         target_pos = hi.target_object->GetPosition(),
                         target_last_pos = hi.target_object->GetLastPosition(),
                         hitbox_pos = {
                            pos.x + hi.hitbox.pos.x,
                            pos.y + hi.hitbox.pos.y},
                         hitbox_last_pos = {
                            last_pos.x + hi.hitbox.pos.x,
                            last_pos.y + hi.hitbox.pos.y},
                         target_hitbox_pos = {
                            target_pos.x + hi.target_hitbox.pos.x,
                            target_pos.y + hi.target_hitbox.pos.y},
                         target_hitbox_last_pos = {
                            target_last_pos.x + hi.target_hitbox.pos.x,
                            target_last_pos.y + hi.target_hitbox.pos.y};

                double dx = pos.x - last_pos.x;
                double dy = pos.y - last_pos.y;
                double dtx = target_pos.x - target_last_pos.x;
                double dty = target_pos.y - target_last_pos.y;
                double abs_dx = abs(dx);
                double abs_dy = abs(dy);
                double abs_dtx = abs(dtx);
                double abs_dty = abs(dty);

                printf("tag: %s\n", pObject->GetTag().c_str());
                printf("last pos   (%lf, %lf)\n", last_pos.x, last_pos.y);
                printf("     pos   (%lf, %lf)\n", pos.x, pos.y);
                printf("hitbox pos (%lf, %lf)\n", hitbox_pos.x, hitbox_pos.y);
                printf("hitbox size (%d, %d)\n", hi.hitbox.width, hi.hitbox.height);
                printf("target:\n");
                printf("last pos   (%lf, %lf)\n", target_last_pos.x, target_last_pos.y);
                printf("     pos   (%lf, %lf)\n", target_pos.x, target_pos.y);
                printf("hitbox pos (%lf, %lf)\n", target_hitbox_pos.x, target_hitbox_pos.y);
                printf("hitbox size (%d, %d)\n", hi.target_hitbox.width, hi.target_hitbox.height);
                printf("\n");

                double hitbox_last_right_x = hitbox_last_pos.x + hi.hitbox.width - 1;
                double hitbox_last_bottom_y = hitbox_last_pos.y + hi.hitbox.height - 1;
                double target_hitbox_last_right_x = target_hitbox_last_pos.x + hi.target_hitbox.width - 1;
                double target_hitbox_last_bottom_y = target_hitbox_last_pos.y + hi.target_hitbox.height - 1;

                double left, right, over, under;

                if(hitbox_last_pos.x < target_hitbox_last_pos.x) {
                    left = hitbox_last_pos.x;
                    right = target_hitbox_last_pos.x + hi.target_hitbox.width;
                } else {
                    left = target_hitbox_last_pos.x;
                    right = hitbox_last_pos.x + hi.hitbox.width;
                }

                if(hitbox_last_pos.y < target_hitbox_last_pos.y) {
                    over = hitbox_last_pos.y;
                    under = target_hitbox_last_pos.y + hi.target_hitbox.height;
                } else {
                    over = target_hitbox_last_pos.y;
                    under = hitbox_last_pos.y + hi.hitbox.height;
                }

                Vector2d new_vec;
                /*
                if((target_last_pos.x <= hitbox_last_pos.x && hitbox_last_pos.x < target_last_pos.x + hi.target_hitbox.width)
                || (target_last_pos.x <= hitbox_last_right_x && hitbox_last_right_x < target_last_pos.x + hi.target_hitbox.width)
                || (last_pos.x <= target_hitbox_last_pos.x && target_hitbox_last_pos.x < last_pos.x + hi.hitbox.width)
                || (last_pos.x <= target_hitbox_last_right_x && target_hitbox_last_right_x < last_pos.x + hi.hitbox.width)) {
                */
                if(right - left < hi.hitbox.width + hi.target_hitbox.width) {
                    if(dy > 0) {
                        printf("***********************************<up>**********************************************************\n");
                        new_vec.y = -((hitbox_pos.y + hi.hitbox.height) - target_hitbox_pos.y);
                        pObject->SetIsGround(true);
                    } else {
                        printf("***********************************<down>**********************************************************\n");
                        new_vec.y = target_hitbox_pos.y + hi.target_hitbox.height - hitbox_pos.y;
                    }
                }

                /*
                if((target_last_pos.y <= hitbox_last_pos.y && hitbox_last_pos.y < target_last_pos.y + hi.target_hitbox.height)
                || (target_last_pos.y <= hitbox_last_bottom_y && hitbox_last_bottom_y < target_last_pos.y + hi.target_hitbox.height)
                || (last_pos.y <= target_hitbox_last_pos.y && target_hitbox_last_pos.y < last_pos.y + hi.hitbox.height)
                || (last_pos.y <= target_hitbox_last_bottom_y && target_hitbox_last_bottom_y < last_pos.y + hi.hitbox.height)) {
                */
                if(under - over < hi.hitbox.height + hi.target_hitbox.height) {
                    if(dx > 0) {
                        printf("***********************************<left>**********************************************************\n");
                        new_vec.x = -((hitbox_pos.x + hi.hitbox.width) - target_hitbox_pos.x);
                        pObject->SetIsWall(true);
                    } else {
                        printf("***********************************<right>**********************************************************\n");
                        new_vec.x = target_hitbox_pos.x + hi.target_hitbox.width - hitbox_pos.x;
                        pObject->SetIsWall(true);
                    }
                }

                if(new_vec.x == 0 && new_vec.y == 0) {
                    if(abs_dtx > abs_dty) {
                        if(dtx > 0) {
                            new_vec.x = target_hitbox_pos.x + hi.target_hitbox.width - hitbox_pos.x;
                            pObject->SetIsWall(true);
                        } else {
                            new_vec.x = -((hitbox_pos.x + hi.hitbox.width) - target_hitbox_pos.x);
                            pObject->SetIsWall(true);
                        }
                    } else if(abs_dty > abs_dtx) {
                        if(dty > 0) {
                            new_vec.y = target_hitbox_pos.y + hi.target_hitbox.height - hitbox_pos.y;
                        } else {
                            new_vec.y = -((hitbox_pos.y + hi.hitbox.height) - target_hitbox_pos.y);
                            pObject->SetIsGround(true);
                        }
                    }
                    else {
                        new_vec.x = -((hitbox_pos.x + hi.hitbox.width) - target_hitbox_pos.x);
                        pObject->SetIsWall(true);
                    }
                }

                /*
                if(abs_dx > abs_dy) {
                    if(dx > 0) {
                        new_vec.x = -((hitbox_pos.x + hi.hitbox.width - 1) - target_hitbox_pos.x + 1);
                        pObject->SetIsWall(true);
                    } else {
                        new_vec.x = target_hitbox_pos.x + hi.target_hitbox.width - hitbox_pos.x - 1 + 1;
                        pObject->SetIsWall(true);
                    }
                } else if(abs_dy > abs_dx) {
                    if(dy > 0) {
                        new_vec.y = -((hitbox_pos.y + hi.hitbox.height - 1) - target_hitbox_pos.y + 1);
                        pObject->SetIsGround(true);
                    } else {
                        new_vec.y = target_hitbox_pos.y + hi.target_hitbox.height - hitbox_pos.y - 1 + 1;
                    }
                } else {
                    if(abs_dtx > abs_dty) {
                        if(dtx > 0) {
                            new_vec.x = target_hitbox_pos.x + hi.target_hitbox.width - hitbox_pos.x - 1 + 1;
                            pObject->SetIsWall(true);
                        } else {
                            new_vec.x = -((hitbox_pos.x + hi.hitbox.width - 1) - target_hitbox_pos.x + 1);
                            pObject->SetIsWall(true);
                        }
                    } else if(abs_dty > abs_dtx) {
                        if(dty > 0) {
                            new_vec.y = target_hitbox_pos.y + hi.target_hitbox.height - hitbox_pos.y - 1 + 1;
                        } else {
                            new_vec.y = -((hitbox_pos.y + hi.hitbox.height - 1) - target_hitbox_pos.y + 1);
                            pObject->SetIsGround(true);
                        }
                    }
                }
                */

                printf("new_vec (%lf, %lf)\n\n", new_vec.x, new_vec.y);

                pObject->Translate(new_vec.x, new_vec.y);
                pObject->SetVector(
                    pObject->IsWall() ? 0 : vec.x,
                    pObject->IsGround() ? 0 : vec.y);
            }

            pObject->StayObject(pOctGame, hi.hitbox, hi.target_object, &hi.target_hitbox);
        }
    });

    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        pObject->UpdateLastPosition();
        pObject->Draw(pOctGame, pCamera);
        pObject->Update(pOctGame);
    });
    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        // reregist LQT
        if(pObject->IsMoved()) {
            vector<LQTNodeNode*> nodes, newNodes;
            nodes = node->GetValue()->lqtNodeNodes;
            if(pObject->GetCollider().GetHitBoxes().size() != nodes.size()) {
                for(LQTNodeNode* v : nodes) {
                    this->mHitBoxList.Remove(v);
                }
                this->mObjectList.Remove(node);
                this->AppendObject(pObject);
            } else {
                for(LQTNodeNode* v : nodes) {
                    newNodes.push_back(this->mHitBoxList.Reregist(v));
                }
                node->GetValue()->lqtNodeNodes = newNodes;
                pObject->SetNotMoved();
            }
        }
    });
    vector<LinkedNode<ObjectListData>*> destroyObjects;
    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        ObjectMessage msg;
        bool isPushed = false;
        // do message
        while(true) {
            msg = pObject->PopMessage();

            if(msg == ObjectMessage::OBJMSG_NONE) {
                break;
            }

            switch(msg) {
            case ObjectMessage::OBJMSG_DESTROY:
                if(!isPushed) {
                    destroyObjects.push_back(node);
                    isPushed = true;
                }
                break;
            default:
                break;
            }
        }

        // append new object
        while(true) {
            Object* pNewObject = pObject->PopObject();

            if(pNewObject == nullptr) {
                break;
            }

            pNewObject->Init(pOctGame);
            this->AppendObject(pNewObject);
        }
        pObject->AddGravity();
    });

    for(LinkedNode<ObjectListData>* v : destroyObjects) {
        vector<LQTNodeNode*> nodes = v->GetValue()->lqtNodeNodes;
        for(LQTNodeNode* node : nodes) {
            this->mHitBoxList.Remove(node);
        }
        this->mObjectList.Remove(v);
    }
}

LQTData::LQTData() : hitboxIndex(0), pObject(nullptr), pList(nullptr) {}

LQTData::LQTData(uint32_t hitboxIndex, Object* pObject, uint32_t handle, LQTNode* pList) : hitboxIndex(hitboxIndex), pObject(pObject), handle(handle), pList(pList) {}

LinearQuaternaryTree::LinearQuaternaryTree(int depth, unsigned int rootWidth, unsigned int rootHeight) {
    this->mDepth = depth;
    this->mRootWidth = rootWidth;
    this->mRootHeight = rootHeight;
    this->mTree = new LQTNode[(POW4(depth + 1) - 1) / 3];
}

LinearQuaternaryTree::~LinearQuaternaryTree() {
    delete[] this->mTree;
}

vector<LQTNodeNode*> LinearQuaternaryTree::Append(Object* pObject) {
    vector<LQTNodeNode*> nodes;

    if(pObject == nullptr) return nodes;

    for(int i = 0; i < pObject->GetCollider().GetHitBoxes().size(); i++) {
        LQTNodeNode* node = this->AppendHitBox(pObject, i);
        assert(node != nullptr);
        nodes.push_back(node);
    }

    return nodes;
}

LQTNodeNode* LinearQuaternaryTree::AppendHitBox(Object* pObject, uint32_t hitboxIndex) {
    return this->AppendHitBox(pObject, hitboxIndex, this->mHandlePool.GetHandle());
}

LQTNodeNode* LinearQuaternaryTree::AppendHitBox(Object* pObject, uint32_t hitboxIndex, HitBoxHandle handle) {
    HitBox hitbox = pObject->GetCollider().GetHitBoxes()[hitboxIndex];
    int index = DetectTreeIndex(pObject->GetPosition(), hitbox, this->mDepth, this->mRootWidth, this->mRootHeight);
    LQTData lqtData(hitboxIndex, pObject, handle, &this->mTree[index]);
    this->mTree[index].Append(lqtData);

    return this->mTree[index].GetTail();
}

LQTNodeNode* LinearQuaternaryTree::Reregist(LQTNodeNode* node) {
    if(node == nullptr) return nullptr;
    auto lastHitBoxes = node->GetValue()->lastHitBoxes;
    LQTNodeNode* newNode = this->AppendHitBox(node->GetValue()->pObject, node->GetValue()->hitboxIndex, node->GetValue()->handle);
    node->GetValue()->pList->Remove(node);
    newNode->GetValue()->lastHitBoxes = lastHitBoxes;

    return newNode;
}

void LinearQuaternaryTree::Remove(LQTNodeNode* node) {
    assert(node != nullptr);
    this->mHandlePool.FreeHandle(node->GetValue()->handle);
    node->GetValue()->pList->Remove(node);
}

void LinearQuaternaryTree::checkHit() const {
    vector<int> checkList;
    checkHitTree(0, 0, this->mDepth, checkList, this->mTree);
}

HandlePool::HandlePool() {
    this->mMax = 0;
}

HitBoxHandle HandlePool::GetHandle() {
    HitBoxHandle ret;

    if(mPool.empty()) {
        ret = this->mMax;
        this->mMax++;
    } else {
        ret = this->mPool.top();
        this->mPool.pop();
    }

    return ret;
}

void HandlePool::FreeHandle(HitBoxHandle handle) {
    this->mPool.push(handle);
}
