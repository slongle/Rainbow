#ifndef __BVHACCELERATOR_H
#define __BVHACCELERATOR_H

#include "../core/primitive.h"

RAINBOW_NAMESPACE_BEGIN

struct BVHPrimitiveInfo;
struct BVHBuildNode;
struct LinearBVHNode;

class BVHAccelerator :public Aggregate {
public:
    enum SplitMethod { SAH, Middle, EqualCounts };

    BVHAccelerator(std::vector<std::shared_ptr<Primitive>>& m_primitives, 
        const SplitMethod& m_splitMethod = SAH, int maxPrimsInNode = 255);

    BVHBuildNode * RecursiveBuild(MemoryArena& arena,
        std::vector<BVHPrimitiveInfo> &primitiveInfo,
        int begin, int end, int *totalNodes,
        std::vector<std::shared_ptr<Primitive>> &orderedPrims);

    int FlattenBVHTree(BVHBuildNode *node, int *offset);

    bool IntersectP(const Ray & ray, SurfaceInteraction*inter) const;
    bool Intersect(const Ray & ray) const;

    std::string toString() const override {
        return tfm::format(
            "# of shapes : %d"            
            , primitives.size());
    }

    int maxPrimsInNode;
    SplitMethod splitMethod;
    LinearBVHNode* nodes = nullptr;
};

BVHAccelerator* CreateBVHAccelerator(std::vector<std::shared_ptr<Primitive>>& primitives);

RAINBOW_NAMESPACE_END

#endif // __BVHACCELERATOR_H
