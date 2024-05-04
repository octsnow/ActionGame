#include "Object.hpp"
#include <cassert>
#include <iostream>

#define POW4(x) (1 << ((x) << 1))

using namespace std;

namespace {
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
        bool result = false;

        LQTData* d = node->GetValue();
        LQTData* td = targetNode->GetValue();
        Object* o = d->pObject;
        Object* to = td->pObject;
        HitBox h = o->GetCollider().GetHitBoxes()[d->hitboxIndex];
        HitBox th = to->GetCollider().GetHitBoxes()[td->hitboxIndex];
        HitBoxHandle handle = d->handle;
        HitBoxHandle targetHandle = td->handle;

        if(h.isActive && th.isActive && h.IsHitBox(th, o->GetPosition(), to->GetPosition())) {
            if(std::find(d->lastHitBoxes.begin(), d->lastHitBoxes.end(), targetHandle) == d->lastHitBoxes.end()) {
                o->EnterObject(to, &th);
                to->EnterObject(o, &h);
            } else {
                o->StayObject(to, &th);
                to->StayObject(o, &h);
            }

            result = true;
        }

        return result;
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

    void checkHitTree(uint32_t nodeIndex, int depth, const int& maxDepth, vector<int>& checkList, LQTNode* tree) {
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
            for(int i = 0; i < 4; i++) {
                checkHitTree((nodeIndex << 2) + i, depth + 1, maxDepth, checkList, tree);
            }
        }
        checkList.pop_back();

        tree[nodeIndex].for_each([&](LQTNodeNode* node) {
            node->GetValue()->lastHitBoxes = node->GetValue()->newHitBoxes;
            node->GetValue()->newHitBoxes.clear();
        });
    }
}

Object::Object() {
    this->InitParams();
    this->SetTag("");
}

Object::Object(std::string tag) {
    this->InitParams();
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
                    hitBoxes[i].isAttack);
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

void Object::Translate(double x, double y) {
    this->SetPosition(this->mPosition.x + x, this->mPosition.y + y);
}

Vector2d Object::GetPosition() const {
    return this->mPosition;
}

void Object::SetVector(double x, double y) {
    this->mVector.x = x;
    this->mVector.y = y;
}

void Object::AddVector(double x, double y) {
    this->mVector.x += x;
    this->mVector.y += y;
}

Vector2d Object::GetVector() const {
    return this->mVector;
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

void Object::Draw(OctGame* pOctGame, Vector2d cameraPos) {
    pOctGame->DrawImage(
        this->GetImageHandle(),
        this->mPosition.x - cameraPos.x,
        this->mPosition.y - cameraPos.y,
        true, this->mIsLeft);

    for(HitBox hitBox : this->GetCollider().GetHitBoxes()) {
        if(!hitBox.isActive) continue;
        Vector2d ltPos, rbPos;
        ltPos.x = hitBox.pos.x + this->GetPosition().x - cameraPos.x;
        ltPos.y = hitBox.pos.y + this->GetPosition().y - cameraPos.y;
        rbPos.x = ltPos.x + hitBox.width - 1;
        rbPos.y = ltPos.y + hitBox.height - 1;
        if(hitBox.isAttack) {
            pOctGame->DrawBox(ltPos.x, ltPos.y, rbPos.x, rbPos.y, 0xFF0000);
        } else {
            pOctGame->DrawBox(ltPos.x, ltPos.y, rbPos.x, rbPos.y, 0x00FF00);
        }
    }
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
    this->mVector.x = 0;
    this->mVector.y = 0;
    this->mIsLeft = false;
    this->mColliderIndex = 0;
    this->mIsMoved = false;
}

ObjectList::ObjectList(unsigned int worldWidth, unsigned int worldHeight)
: mHitBoxList(3, worldWidth, worldHeight) {
}

void ObjectList::AppendObject(Object* pObject) {
    auto nodes = this->mHitBoxList.Append(pObject);
    ObjectListData data;
    data.pObject = pObject;
    data.lqtNodes = nodes;
    this->mObjectList.Append(data);
}

void ObjectList::CheckHitObjects() const {
    this->mHitBoxList.checkHit();
}

void ObjectList::Update(OctGame* pOctGame, Vector2d cameraPos) {
    this->CheckHitObjects();
    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        pObject->UpdatePosition();
        pObject->Draw(pOctGame, cameraPos);
        pObject->Update();
    });
    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        // reregist LQT
        if(pObject->IsMoved()) {
            vector<LQTNodeNode*> nodes, newNodes;
            nodes = node->GetValue()->lqtNodes;
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
                node->GetValue()->lqtNodes = newNodes;
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
        vector<LQTNodeNode*> nodes = v->GetValue()->lqtNodes;
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
    LQTNodeNode* newNode = this->AppendHitBox(node->GetValue()->pObject, node->GetValue()->hitboxIndex, node->GetValue()->handle);
    node->GetValue()->pList->Remove(node);

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
