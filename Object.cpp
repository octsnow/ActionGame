#include "Object.hpp"
#include <cassert>
#include <iostream>

#define POW4(x) (1 << (x << 1))

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
        if(pos.x < 0) pos.x = 0;
        if(pos.y < 0) pos.y = 0;
        if(pos.x >= rootWidth) pos.x = rootWidth - 1;
        if(pos.y >= rootHeight) pos.y = rootHeight - 1;

        double uWidth = (double)rootWidth / (1 << maxDepth);
        double uHeight = (double)rootHeight / (1 << maxDepth);

        Vector2d ltPos(pos.x + hitbox.pos.x, pos.y + hitbox.pos.y);
        Vector2d rbPos(ltPos.x + hitbox.width - 1, ltPos.y + hitbox.height - 1);
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

    void checkHitHitBoxes(const HitBox hitBox, Object* pObject, LQTNode& hitBoxes) {
        hitBoxes.for_each([&](LinkedNode<LQTData>* node) {
            HitBox tergetHitBox = node->GetValue()->hitbox;
            Object* pTergetObject = node->GetValue()->pObject;

            if(hitBox.IsHitBox(tergetHitBox, pObject->GetPosition(), pTergetObject->GetPosition())) {
                pObject->HitObject(pTergetObject, &tergetHitBox);
                pTergetObject->HitObject(pObject, &hitBox);
            }
        });
    }

    void checkHitHitBoxes(LQTNode& hitBoxes) {
        hitBoxes.for_each([&](LinkedNode<LQTData>* pNode) {

            const HitBox hitbox = pNode->GetValue()->hitbox;
            Object* pObject = pNode->GetValue()->pObject;
            LinkedNode<LQTData>* pTargetNode = pNode->mNext;

            while(pTargetNode != nullptr) {
                const HitBox tergetHitbox = pTargetNode->GetValue()->hitbox;
                Object* pTergetObject = pTargetNode->GetValue()->pObject;

                if(hitbox.IsHitBox(tergetHitbox, pObject->GetPosition(), pTergetObject->GetPosition())) {
                    pObject->HitObject(pTergetObject, &tergetHitbox);
                    pTergetObject->HitObject(pObject, &hitbox);
                }

                pTargetNode = pTargetNode->mNext;
            }
        });
    }

    void checkHitTree(uint32_t nodeIndex, int depth, const int& maxDepth, vector<int>& checkList, LQTNode* tree) {
        checkHitHitBoxes(tree[nodeIndex]);

        if(checkList.size() > 0 && tree[nodeIndex].GetHead() != nullptr) {
            tree[nodeIndex].for_each([&](LinkedNode<LQTData>* node) {
                for(int j : checkList) {
                    const HitBox hitBox = node->GetValue()->hitbox;
                    Object* pObject = node->GetValue()->pObject;

                    checkHitHitBoxes(hitBox, pObject, tree[j]);
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

void Object::AppendCollider(Collider c) {
    this->mColliders.push_back(c);
    this->mIsMoved = true;
}

void Object::SwitchCollider(int i) {
    this->mColliderIndex = i;
    this->mIsMoved = true;
}

Collider* Object::GetCurrentCollider() {
    if(this->mColliders.size() == 0) {
        return nullptr;
    }

    return &this->mColliders[this->mColliderIndex];
}

Collider* Object::GetCollider(int i) {
    assert(i < this->mColliders.size());

    return &this->mColliders[i];
}

int Object::GetNumColliders() const {
    return this->mColliders.size();
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

void Object::SetTag(string tag) {
    this->mTag = tag;
}

bool Object::CompareTag(string tag) const{
    return (tag == this->mTag);
}

void Object::PushMessage(ObjectMessage msg) {
    if(msg == ObjectMessage::OBJECTMESSAGE_NONE) return;

    this->mMsgQue.push(msg);
}

ObjectMessage Object::PopMessage() {
    if(this->mMsgQue.size() == 0) {
        return OBJECTMESSAGE_NONE;
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

void Object::Draw(OctGame* game, Vector2d cameraPos) {
    game->DrawImage(
        this->GetImageHandle(),
        this->mPosition.x - cameraPos.x,
        this->mPosition.y - cameraPos.y,
        true, this->mIsLeft);
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

    vector<LinkedNode<ObjectListData>*> destroyObjects;
    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        ObjectMessage msg;
        // do message
        while(true) {
            msg = pObject->PopMessage();

            if(msg == OBJECTMESSAGE_NONE) {
                break;
            }

            switch(msg) {
            case OBJECTMESSAGE_DESTROY:
                destroyObjects.push_back(node);
                break;
            default:
                break;
            }
        }

        /*
        // append new object
        while(true) {
            Object* pNewObject = pObject->PopObject();

            if(pNewObject == nullptr) {
                break;
            } else {
                break;
            }

            pNewObject->Init(pOctGame);
            delete pNewObject;
//            this->AppendObject(pNewObject);
        }
        */
/*
        // reregist LQT
        if(pObject->IsMoved()) {
            vector<LQTNodeNode*> nodes = node->GetValue()->lqtNodes;
            for(LQTNodeNode* v : nodes) {
                LinkedList<LQTData>* pList = v->GetValue()->pList;
                pList->Remove(v);
            }
            node->GetValue()->lqtNodes = this->mHitBoxList.Append(pObject);
//            this->mObjectList.Remove(node);
//            this->AppendObject(pObject);
            pObject->SetNotMoved();
        }
        */
    });

    for(LinkedNode<ObjectListData>* v : destroyObjects) {
        cout << "start destroing" << endl;
        vector<LQTNodeNode*> nodes = v->GetValue()->lqtNodes;
        cout << "remove LQTNodes" << endl;
        for(LQTNodeNode* node : nodes) {
            LinkedList<LQTData>* pList = node->GetValue()->pList;
            cout << "remove LQTNode" << endl;
            pList->Remove(node);
        }
        cout << "end removing LQTNodes" << endl;
        this->mObjectList.Remove(v);
        cout << "end destroing Object" << endl;
    }
}

LinearQuaternaryTree::LinearQuaternaryTree(int depth, unsigned int rootWidth, unsigned int rootHeight) {
    this->mDepth = depth;
    this->mRootWidth = rootWidth;
    this->mRootHeight = rootHeight;
    this->mTree = new LQTNode[(pow(4, depth + 1) - 1) / 3];
}

LinearQuaternaryTree::~LinearQuaternaryTree() {
    delete[] this->mTree;
}

vector<LQTNodeNode*> LinearQuaternaryTree::Append(Object* pObject) {
    vector<LQTNodeNode*> nodes;

    if(pObject == nullptr) return nodes;
    if(pObject->GetCurrentCollider() == nullptr) return nodes;

    for(HitBox hitbox : pObject->GetCurrentCollider()->GetHitBoxes()) {
        int index = DetectTreeIndex(pObject->GetPosition(), hitbox, this->mDepth, this->mRootWidth, this->mRootHeight);
        LQTData lqtData(hitbox, pObject, &this->mTree[index]);
        this->mTree[index].Append(lqtData);
        nodes.push_back(this->mTree[index].GetTail());
    }

    return nodes;
}

void LinearQuaternaryTree::checkHit() const {
    vector<int> checkList;
    checkHitTree(0, 0, this->mDepth, checkList, this->mTree);
}
