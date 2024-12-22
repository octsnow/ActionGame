#include "ObjectList.hpp"
#include <vector>

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

    int DetectTreeIndex(Vector2D pos, const HitBox& hitbox, const int maxDepth, unsigned int rootWidth, unsigned int rootHeight) {
        double uWidth = (double)rootWidth / (1 << maxDepth);
        double uHeight = (double)rootHeight / (1 << maxDepth);

        Vector2D ltPos(pos.x + hitbox.pos.x, pos.y + hitbox.pos.y);
        Vector2D rbPos(ltPos.x + hitbox.width - 1, ltPos.y + hitbox.height - 1);

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

    void checkHitTree(uint32_t morton, int depth, const int& maxDepth, std::vector<int>& checkList, LQTNode* tree) {
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

    Vector2D Bounce(HitBox h, HitBox ht, Vector2D p, Vector2D pt, Vector2D v, Vector2D vt, bool *isGround, bool *isWall) {
        Vector2D newVec;
        double t[4], tMin = 1;
        double ignore = -1;
        int hitCount = 0;
        // left side
        {
            Vector2D p0, pt0;
            p0.x = p.x + h.pos.x;
            p0.y = p.y + h.pos.y;
            pt0.x = pt.x + ht.pos.x + ht.width - 1;
            pt0.y = pt.y + ht.pos.y;

            if(v.x == vt.x) {
                t[0] = ignore;
            } else {
                t[0] = (pt0.x - p0.x) / (v.x - vt.x);
            }

            if(t[0] < 0 || t[0] > 1 || t[0] == ignore) {
                t[0] = ignore;
            } else {
                double y1, y2, yt1, yt2;
                y1 = p0.y + t[0] * v.y;
                y2 = y1 + h.height - 1;
                yt1 = pt0.y + t[0] * vt.y;
                yt2 = yt1 + ht.height - 1;

                if(!(y1 <= yt1 && yt1 <= y2
                  || y1 <= yt2 && yt2 <= y2
                  || yt1 <= y1 && y2 <= yt2)) {
                    t[0] = ignore;
                } else {
                    t[0] = (pt0.x - p0.x + 1) / (v.x - vt.x);
                }
            }
        }
        // right side
        {
            Vector2D p0, pt0;
            p0.x = p.x + h.pos.x + h.width - 1;
            p0.y = p.y + h.pos.y;
            pt0.x = pt.x + ht.pos.x;
            pt0.y = pt.y + ht.pos.y;

            if(v.x == vt.x) {
                t[1] = 2;
            } else {
                t[1] = (pt0.x - p0.x) / (v.x - vt.x);
            }

            if(t[1] < 0 || t[1] > 1 || t[1] == ignore) {
                t[1] = 2;
            } else {
                double y1, y2, yt1, yt2;
                y1 = p0.y + t[1] * v.y;
                y2 = y1 + h.height - 1;
                yt1 = pt0.y + t[1] * vt.y;
                yt2 = yt1 + ht.height - 1;

                if(!(y1 <= yt1 && yt1 <= y2
                  || y1 <= yt2 && yt2 <= y2
                  || yt1 <= y1 && y2 <= yt2)) {
                    t[1] = ignore;
                } else {
                    t[1] = (pt0.x - p0.x - 1) / (v.x - vt.x);
                }
            }
        }
        // top
        {
            Vector2D p0, pt0;
            p0.x = p.x + h.pos.x;
            p0.y = p.y + h.pos.y;
            pt0.x = pt.x + ht.pos.x;
            pt0.y = pt.y + ht.pos.y + ht.height - 1;

            if(v.y == vt.y) {
                t[2] = ignore;
            } else {
                t[2] = (pt0.y - p0.y) / (v.y - vt.y);
            }

            if(t[2] < 0 || t[2] > 1 || t[2] == ignore) {
                t[2] = ignore;
            } else {
                double x1, x2, xt1, xt2;
                x1 = p0.x + t[2] * v.x;
                x2 = x1 + h.width - 1;
                xt1 = pt0.x + t[2] * vt.x;
                xt2 = xt1 + ht.width - 1;

                if(!(x1 <= xt1 && xt1 <= x2
                  || x1 <= xt2 && xt2 <= x2
                  || xt1 <= x1 && x2 <= xt2)) {
                    t[2] = ignore;
                } else {
                    t[2] = (pt0.y - p0.y + 1) / (v.y - vt.y);
                }
            }
        }
        // bottom
        {
            Vector2D p0, pt0;
            p0.x = p.x + h.pos.x;
            p0.y = p.y + h.pos.y + h.height - 1;
            pt0.x = pt.x + ht.pos.x;
            pt0.y = pt.y + ht.pos.y;

            if(v.y == vt.y) {
                t[3] = ignore;
            } else {
                t[3] = (pt0.y - p0.y) / (v.y - vt.y);
            }

            if(t[3] < 0 || t[3] > 1 || t[3] == ignore) {
                t[3] = ignore;
            } else {
                double x1, x2, xt1, xt2;
                x1 = p0.x + t[3] * v.x;
                x2 = x1 + h.width - 1;
                xt1 = pt0.x + t[3] * vt.x;
                xt2 = xt1 + ht.width - 1;

                if(!(x1 <= xt1 && xt1 <= x2
                  || x1 <= xt2 && xt2 <= x2
                  || xt1 <= x1 && x2 <= xt2)) {
                    t[3] = ignore;
                } else {
                    t[3] = (pt0.y - p0.y - 1) / (v.y - vt.y);
                }
            }
        }

        for(int i = 0; i < 4; i++) {
            if(t[i] != ignore && t[i] < tMin) {
                tMin = t[i];
            }
        }

        for(int i = 0; i < 4; i++) {
            if(t[i] == tMin) {
                hitCount++;
                if(i < 2) {
                    *isWall = true;
                } else if(i == 3) {
                    *isGround = true;
                }
            }
        }

        if(*isWall) {
            newVec.x = -v.x * (1 - tMin) + vt.x * (1 - tMin);
        }
        if(*isGround || t[2] == tMin) {
            newVec.y = -v.y * (1 - tMin) + vt.y * (1 - tMin);
        }

        if(hitCount > 1) {
            *isWall = false;
            *isGround = false;
        }

        return newVec;
    }
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

void ObjectList::Update(OctGame* pOctGame, Stage *pStage) {
    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        pStage->CheckHitBlock(pObject);
        pObject->UpdatePosition();
        pObject->enters.clear();
        pObject->stays.clear();
    });
    this->CheckHitObjects();

    std::vector<LinkedNode<ObjectListData>*> destroyObjects;
    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        for(HITINFO hi : pObject->stays) {
            if(hi.hitbox.isPhysics
            && hi.target_hitbox.isPhysics
            && pObject->GetWeight() < hi.target_object->GetWeight()
            && hi.hitbox.IsHitBox(hi.target_hitbox, pObject->GetPosition(), hi.target_object->GetPosition())) {
                Vector2D newVec;
                bool isGround = false, isWall = false;

                newVec = Bounce(
                        hi.hitbox,
                        hi.target_hitbox,
                        pObject->GetLastPosition(),
                        hi.target_object->GetLastPosition(),
                        pObject->GetPosition() - pObject->GetLastPosition(),
                        hi.target_object->GetPosition() - hi.target_object->GetLastPosition(),
                        &isGround,
                        &isWall);

                Vector2D vec = pObject->GetVector();
                if(isGround) {
                    vec.y = 0;
                    pObject->SetIsGround(isGround);
                }
                if(isWall) {
                    vec.x = 0;
                    pObject->SetIsWall(isWall);
                }
                pObject->SetVector(vec);

                isGround = pObject->IsGround();
                isWall = pObject->IsWall();
                pStage->Translate(pObject, newVec);

                if(pObject->IsWall()) {
                    Vector2D vec = hi.target_object->GetVector();
                    vec.x = 0;
                    hi.target_object->Translate(-newVec.x, 0);
                    hi.target_object->SetVector(vec);
                    hi.target_object->SetIsWall(true);
                }
                if(pObject->IsGround() && newVec.y > 0) {
                    Vector2D vec = hi.target_object->GetVector();
                    vec.y = 0;
                    hi.target_object->Translate(0, -newVec.y);
                    hi.target_object->SetVector(vec);
                    hi.target_object->SetIsGround(true);
                }

                pObject->SetIsGround(pObject->IsGround() | isGround);
                pObject->SetIsWall(pObject->IsWall() | isWall);
            }
        }
    });
    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object* pObject = node->GetValue()->pObject;
        pObject->UpdateLastPosition();

        for(HITINFO hi : pObject->enters) {
            pObject->EnterObject(pOctGame, hi.hitbox, hi.target_object, &hi.target_hitbox);
        }
        for(HITINFO hi : pObject->stays) {
            pObject->StayObject(pOctGame, hi.hitbox, hi.target_object, &hi.target_hitbox);
        }

        pObject->Update(pOctGame);

        // reregist LQT
        if(pObject->IsMoved()) {
            std::vector<LQTNodeNode*> nodes, newNodes;
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
        std::vector<LQTNodeNode*> nodes = v->GetValue()->lqtNodeNodes;
        for(LQTNodeNode* node : nodes) {
            this->mHitBoxList.Remove(node);
        }
        this->mObjectList.Remove(v);
    }
}

void ObjectList::Draw(OctGame *pOctGame, Camera *pCamera) {
    this->mObjectList.for_each([&](LinkedNode<ObjectListData>* node) {
        Object *pObject = node->GetValue()->pObject;
        pObject->Draw(pOctGame, pCamera);
    });
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

std::vector<LQTNodeNode*> LinearQuaternaryTree::Append(Object* pObject) {
    std::vector<LQTNodeNode*> nodes;

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
    std::vector<int> checkList;
    checkHitTree(0, 0, this->mDepth, checkList, this->mTree);

    /*
    for(int i = 0; i < (POW4(this->mDepth + 1) - 1) / 3; i++) {
        printf("%d:\n", i);
        this->mTree[i].for_each([&](LinkedNode<LQTData> *node) {
            Object* pObject = node->GetValue()->pObject;

            printf("    %s\n", pObject->GetTag().c_str());
        });
    }
    */
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
