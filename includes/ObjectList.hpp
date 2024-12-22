#include <stack>
#include <vector>

#include "OctGame.hpp"
#include "Camera.hpp"
#include "Stage.hpp"
#include "Object.hpp"

typedef uint32_t HitBoxHandle;

class ObjectList;
class LQTData;
class LinearQuaternaryTree;
class HandlePool;

struct ObjectListData {
    Object* pObject;
    std::vector<LinkedNode<LQTData>*> lqtNodeNodes;
};

typedef LinkedList<LQTData> LQTNode;
typedef LinkedNode<LQTData> LQTNodeNode;

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
    void Update(OctGame *pOctGame, Stage *stage);
    void Draw(OctGame *pOctGame, Camera *pCamera);
    template <typename Func> void for_each(Func func) {
        this->mObjectList.for_each(func);
    }

private:
    LinkedList<ObjectListData> mObjectList;
    LinearQuaternaryTree mHitBoxList;
};
