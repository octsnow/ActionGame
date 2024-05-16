#pragma once

#include <string>
#include <queue>
#include <stack>
#include <vector>
#include <tuple>

#include "OctGame/OctGame.hpp"
#include "Collider.hpp"

typedef uint32_t HitBoxHandle;

class Object;
class ObjectList;
class LQTData;
class LinearQuaternaryTree;
class HandlePool;

struct ObjectListData {
    Object* pObject;
    std::vector<LinkedNode<LQTData>*> lqtNodes;
};

typedef LinkedList<LQTData> LQTNode;
typedef LinkedNode<LQTData> LQTNodeNode;

enum ObjectMessage {
    OBJMSG_DESTROY,
    OBJMSG_NONE
};

class Object {
public:
    // Methods
    Object();
    Object(std::string tag);

    int SetImageHandle(clock_t time, std::vector<int> handles);
    int GetImageHandle();

    void SetAnimationNum(int n);
    void SetAnimationIndex(int i);

    void SetSize(int width, int height);
    int GetWidth() const;
    int GetHeight() const;

    void SetCollider(Collider c);
    void SetColliderSet(std::vector<uint32_t> indices);
    void SwitchCollider(int i);
    Collider GetCollider();

    void SetPosition(double x, double y);
    void SetOffsetPosition(double x, double y);
    Vector2d GetPosition() const;
    Vector2d GetOffsetPosition() const;
    void Translate(double x, double y);

    void SetVector(double x, double y);
    Vector2d GetVector() const;
    void AddVector(double x, double y);

    void TurnLeft();
    void TurnRight();
    void TurnOther();

    void UpdatePosition();

    void SetIsGround(bool flag);
    void SetIsWall(bool flag);
    void SetNotMoved();
    bool IsGround();
    bool IsWall();
    bool IsMoved();

    void SetGravity(double gravity);
    void AddGravity();

    void SetTag(std::string tag);
    bool CompareTag(std::string tag) const;

    // Message Queue
    void PushMessage(ObjectMessage message);
    ObjectMessage PopMessage();

    // Object Queue
    void PushObject(Object* pObject);
    Object* PopObject();

    virtual void Init(OctGame* pOctGame) {};
    virtual void Update(OctGame* pOctGame) {};
    virtual void Draw(OctGame* pOctGame, Vector2d cameraPos);

    // event call backs
    virtual void EnterObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {};
    virtual void StayObject(HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {};

protected:
    // Methods
    void InitParams();

    // Variables
    std::vector<clock_t> mAnimationTimes;
    std::vector<std::vector<int>> mImageHandles;

    int mAnimNum;
    int mAnimIndex;
    int mLastAnimNum;

    clock_t mLastTime;

    int mWidth;
    int mHeight;
    Vector2d mPosition;
    Vector2d mOffsetPosition;
    Vector2d mVector;
    bool mIsLeft;

    double mGravity;
    bool mIsGround;
    bool mIsWall;

    Collider mCollider;
    std::vector<std::vector<uint32_t>> mColliderIndices;
//    std::vector<Collider> mColliders;
    int mColliderIndex;
    std::string mTag;
    std::queue<ObjectMessage> mMsgQue;
    std::queue<Object*> mObjectQue;

    bool mIsMoved;
};

class HandlePool {
public:
    HandlePool();
    HitBoxHandle GetHandle();
    void FreeHandle(HitBoxHandle handle);

private:
    std::stack<HitBoxHandle> mPool;
    HitBoxHandle mMax;
};

class LQTData{
public:
    LQTData();
    LQTData(uint32_t hitboxIndex, Object* pObject, uint32_t handle, LQTNode* pList);
    uint32_t hitboxIndex;
    Object* pObject;
    uint32_t handle;
    LQTNode* pList;
    std::vector<HitBoxHandle> lastHitBoxes;
    std::vector<HitBoxHandle> newHitBoxes;
};

class LinearQuaternaryTree {
public:
    LinearQuaternaryTree(int depth, unsigned int rootWidth, unsigned int rootHeight);
    ~LinearQuaternaryTree();
    std::vector<LQTNodeNode*> Append(Object* object);
    LQTNodeNode* AppendHitBox(Object* pObject, uint32_t hitboxIndex);
    LQTNodeNode* AppendHitBox(Object* pObject, uint32_t hitboxIndex, HitBoxHandle handle);
    LQTNodeNode* Reregist(LQTNodeNode* node);
    void Remove(LQTNodeNode* node);
    void checkHit() const;

private:
    LQTNode* mTree;
    HandlePool mHandlePool;
    int mDepth;
    unsigned int mRootWidth;
    unsigned int mRootHeight;
};

class ObjectList {
public:
    ObjectList(unsigned int worldWidth, unsigned int worldHeight);
    void AppendObject(Object* pObject);
    void CheckHitObjects() const;
    void Update(OctGame* pOctGame, Vector2d);
    template <typename Func> void for_each(Func func) {
        this->mObjectList.for_each(func);
    }

private:
    LinkedList<ObjectListData> mObjectList;
    LinearQuaternaryTree mHitBoxList;
};
