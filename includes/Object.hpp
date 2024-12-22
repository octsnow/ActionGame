#pragma once

#include <string>

#include "OctGame.hpp"
#include "Camera.hpp"
#include "Collider.hpp"

class Object;

// TODO:
typedef struct hit_info_tag {
    HitBox hitbox;
    Object* target_object;
    HitBox target_hitbox;
} HITINFO;

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
    void SetPosition(Vector2D position);
    void SetOffsetPosition(double x, double y);
    Vector2D GetPosition() const;
    Vector2D GetLastPosition() const;
    Vector2D GetOffsetPosition() const;
    void Translate(double x, double y);

    void SetVector(double x, double y);
    void SetVector(Vector2D vector);
    Vector2D GetTranslateVector() const;
    Vector2D GetVector() const;
    void AddVector(double x, double y);
    void AddVector(Vector2D vector);

    void SetWeight(double weight);
    double GetWeight();

    void TurnLeft();
    void TurnRight();
    void TurnOther();

    void UpdatePosition();
    void UpdateLastPosition();

    void SetIsGround(bool flag);
    void SetIsWall(bool flag);
    void SetNotMoved();
    bool IsGround();
    bool IsWall();
    bool IsMoved();

    void SetGravity(double gravity);
    void AddGravity();

    void SetTag(std::string tag);
    std::string GetTag() const;
    bool CompareTag(std::string tag) const;

    // Message Queue
    void PushMessage(ObjectMessage message);
    ObjectMessage PopMessage();

    // Object Queue
    void PushObject(Object* pObject);
    Object* PopObject();

    virtual void Init(OctGame* pOctGame) {};
    virtual void Update(OctGame* pOctGame) {};
    virtual void Draw(OctGame* pOctGame, Camera* pCamera);

    // event call backs
    virtual void EnterObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {};
    virtual void StayObject(OctGame* pOctGame, HitBox hitbox, const Object* pTargetObject, const HitBox* pTargetHitbox) {};

    // TODO:
    std::vector<HITINFO> enters;
    std::vector<HITINFO> stays;

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
    Vector2D mPosition;
    Vector2D mLastPosition;
    Vector2D mOffsetPosition;
    Vector2D mVector;
    Vector2D mTranslateVector;
    bool mIsLeft;
    double mWeight;

    double mGravity;
    bool mIsGround;
    bool mIsWall;

    Collider mCollider;
    std::vector<std::vector<uint32_t>> mColliderIndices;
    int mColliderIndex;
    std::string mTag;
    std::queue<ObjectMessage> mMsgQue;
    std::queue<Object*> mObjectQue;

    LinkedList<std::string> mIgnoreTags;

    bool mIsMoved;
};

