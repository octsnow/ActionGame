#pragma once

#include <string>
#include <queue>
#include <vector>
#include <tuple>

#include "OctGame/OctGame.hpp"
#include "Collider.hpp"

class Object;
class ObjectList;
class LQTData;
class LinearQuaternaryTree;

struct ObjectListData{
    Object* pObject;
    std::vector<LinkedNode<LQTData>*> lqtNodes;
};

typedef LinkedList<LQTData> LQTNode;
typedef LinkedNode<LQTData> LQTNodeNode;

enum ObjectMessage {
    OBJECTMESSAGE_DESTROY,
    OBJECTMESSAGE_NONE
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

    void AppendCollider(Collider c);
    void SwitchCollider(int i);
    Collider* GetCurrentCollider();
    Collider* GetCollider(int i);
    int GetNumColliders() const;

    void SetPosition(double x, double y);
    Vector2d GetPosition() const;
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

    void SetTag(std::string tag);
    bool CompareTag(std::string tag) const;

    // Message Queue
    void PushMessage(ObjectMessage message);
    ObjectMessage PopMessage();

    // Object Queue
    void PushObject(Object* pObject);
    Object* PopObject();

    virtual void Init(OctGame* pOctGame) {};
    virtual void Update() {};
    virtual void Draw(OctGame* game, Vector2d cameraPos);

    // event call backs
    virtual void HitObject(const Object* object, const HitBox* hitbox) {};

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
    Vector2d mVector;
    bool mIsLeft;

    double mGravity;
    bool mIsGround;
    bool mIsWall;

    std::vector<Collider> mColliders;
    int mColliderIndex;
    std::string mTag;
    std::queue<ObjectMessage> mMsgQue;
    std::queue<Object*> mObjectQue;

    bool mIsMoved;
};

class LQTData{
public:
    LQTData() : hitbox(HitBox(0, 0, 0, 0, false, false)), pObject(nullptr), pList(nullptr) {}
    LQTData(HitBox hitbox, Object* pObject, LQTNode* pList) : hitbox(hitbox), pObject(pObject), pList(pList) {}
    HitBox hitbox;
    Object* pObject;
    LQTNode* pList;
};

class LinearQuaternaryTree {
public:
    LinearQuaternaryTree(int depth, unsigned int rootWidth, unsigned int rootHeight);
    ~LinearQuaternaryTree();
    std::vector<LQTNodeNode*> Append(Object* object);
    void checkHit() const;

private:
    LQTNode* mTree;
    int mDepth;
    unsigned int mRootWidth;
    unsigned int mRootHeight;
};

class ObjectList {
public:
    ObjectList(unsigned int worldWidth, unsigned int worldHeight);
    void AppendObject(Object* object);
    void CheckHitObjects() const;
    void Update(OctGame* pOctGame, Vector2d);
    template <typename Func> void for_each(Func func) {
        this->mObjectList.for_each(func);
    }

private:
    LinkedList<ObjectListData> mObjectList;
    LinearQuaternaryTree mHitBoxList;
};
