#include "bvh.h"

RAINBOW_NAMESPACE_BEGIN

struct BVHPrimitiveInfo {
    BVHPrimitiveInfo() {}
    BVHPrimitiveInfo(const int &m_idx, const Bounds3f &m_bounds) :
        idx(m_idx), bounds(m_bounds), centroid(bounds.Centroid()) {}

    Bounds3f bounds;
    Point3f centroid;
    int idx;          // the index in primitives aray
};

struct BVHBuildNode {
    void InitLeaf(const Bounds3f& m_bounds, const size_t &m_firstPrimOffset, const int& m_nPrimitives) {
        bounds = m_bounds;
        firstPrimOffset = m_firstPrimOffset;
        nPrimitives = m_nPrimitives;
        children[0] = children[1] = nullptr;
    }

    void InitInterior(const Bounds3f& m_bounds, 
        BVHBuildNode* lchild, BVHBuildNode* rchild, const int& m_splitAxis) {
        bounds = m_bounds;
        children[0] = lchild;
        children[1] = rchild;
        splitAxis = m_splitAxis;
        nPrimitives = 0;
    }

    Bounds3f bounds;
    BVHBuildNode* children[2];
    int splitAxis;
    int firstPrimOffset; // the offset in orderedPrims array
    int nPrimitives;     // the number of primitives in this node
};

struct BucketInfo {
    int count = 0;
    Bounds3f bounds;
};

struct LinearBVHNode {
    Bounds3f bounds;
    union {
        int primitivesOffset; // Leaf      the offset in primitives array
        int rightChildOffset; // Interior  the offset in node array
    };
    uint16_t nPrimitives;     // the number of primitives in this node
    uint8_t axis;             // SplitAxis   
    uint8_t pad;              // Ensure 32 byte size
};

BVHAccelerator::BVHAccelerator(std::vector<std::shared_ptr<Primitive>>& m_primitives, const SplitMethod& m_splitMethod,
    int m_maxPrimsInNode) :
    Aggregate(m_primitives), splitMethod(m_splitMethod), maxPrimsInNode(std::min(m_maxPrimsInNode, 255)) {
    
    if (primitives.size() == 0)
        return;

    //Initialize primitiveInfo array for primitives
    std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
    for (int i = 0; i < primitives.size(); i++)
        primitiveInfo[i] = BVHPrimitiveInfo(i, primitives[i]->shape->WorldBounds());

    // Build BVH tree for primitives using primitiveInfo
    MemoryArena arena(1 << 20);
    int totalNodes = 0;
    std::vector<std::shared_ptr<Primitive>> orderedPrims;
    orderedPrims.resize(primitives.size());
    BVHBuildNode *root;
    root = RecursiveBuild(arena, primitiveInfo, 0, primitives.size(), &totalNodes, orderedPrims);
    primitives.swap(orderedPrims);
    orderedPrims.resize(0);

    // Compute representation of depth-first traversal of BVH tree
    nodes = AllocAligned<LinearBVHNode>(totalNodes);
    int offset = 0;
    FlattenBVHTree(root, &offset);
}

BVHBuildNode * BVHAccelerator::RecursiveBuild(MemoryArena& arena, std::vector<BVHPrimitiveInfo> &primitiveInfo,
    int begin, int end, int *totalNodes, 
    std::vector<std::shared_ptr<Primitive>> &orderedPrims) {
    BVHBuildNode *node = arena.Alloc<BVHBuildNode>();
    (*totalNodes)++;

    // Compute bounds of all primitives in this BVH node
    Bounds3f bounds;
    for (int i = begin; i < end; i++)
        bounds = Union(bounds, primitiveInfo[i].bounds);

    int nPrimitives = end - begin;
    if (nPrimitives == 1) {
        // Create leaf node
        size_t firstPrimOffset = orderedPrims.size();
        for (int i = begin; i < end; i++) {
            int idx = primitiveInfo[i].idx;
            orderedPrims.push_back(primitives[idx]);
        }
        node->InitLeaf(bounds, firstPrimOffset, nPrimitives);
    }
    else {
        // Compute bound of primitive centroid, choose split dimension
        Bounds3f centroidBounds;
        for (int i = begin; i < end; i++)
            centroidBounds = Union(centroidBounds, primitiveInfo[i].centroid);
        int dim = centroidBounds.MaximumExtent();

        // Partition primitives into two sets and build children
        int mid = (begin + end) >> 1;
        if (centroidBounds.pMin[dim] == centroidBounds.pMax[dim]) {
            // Create leaf node
            size_t firstPrimOffset = orderedPrims.size();
            for (int i = begin; i < end; i++) {
                int idx = primitiveInfo[i].idx;
                orderedPrims.push_back(primitives[idx]);
            }
            node->InitLeaf(bounds, firstPrimOffset, nPrimitives);
        }
        else {
            // Partition primitives based on splitMethod
            switch (splitMethod) {
            case SplitMethod::Middle: {
                // Partition primitives through node's midpoint
                Float pMid = (centroidBounds.pMin[dim] + centroidBounds.pMax[dim])*0.5;
                BVHPrimitiveInfo *midPtr = std::partition(&primitiveInfo[begin], &primitiveInfo[end - 1] + 1,
                    [dim, pMid](BVHPrimitiveInfo &prim) {
                    return prim.centroid[dim] < pMid;
                });
                mid = midPtr - &primitiveInfo[0];
                if (mid != begin && mid != end)
                    break;
            }
            case SplitMethod::EqualCounts: {
                // Partition primitives into equally-sized subsets
                mid = (begin + end) >> 1;
                std::nth_element(&primitiveInfo[begin], &primitiveInfo[mid], &primitiveInfo[end - 1] + 1,
                    [dim](BVHPrimitiveInfo &prim1, BVHPrimitiveInfo &prim2) {
                    return prim1.centroid[dim] < prim2.centroid[dim];
                });
                break;
            }
            case SplitMethod::SAH:
            default: {
                // Partition primitives using approximate SAH
                if (nPrimitives <= 4) {
                    // Partition primitives into equally-sized subsets
                    mid = (begin + end) >> 1;
                    std::nth_element(&primitiveInfo[begin], &primitiveInfo[mid], &primitiveInfo[end - 1] + 1,
                        [dim](BVHPrimitiveInfo &prim1, BVHPrimitiveInfo &prim2) {
                        return prim1.centroid[dim] < prim2.centroid[dim];
                    });
                }
                else {
                    // Allocate BucketInfo for SAH partition buckets
                    constexpr int nBuckets = 12;                    
                    BucketInfo buckets[nBuckets];

                    // Initialize BucketInfo for SAH partition buckets
                    for(size_t i=begin;i<end;i++) {
                        int b = nBuckets * centroidBounds.Offset(primitiveInfo[i].centroid)[dim];
                        if (b == nBuckets) b = nBuckets - 1;
                        buckets[b].count++;
                        buckets[b].bounds = Union(buckets[b].bounds, primitiveInfo[i].bounds);
                    }

                    // Compute costs for splitting after each bucket
                    // cost = 1 + (countPre * boundsPre.Area + countSuf * boundsSuf.Area) / bounds.Area
                    Float costPre[nBuckets], costSuf[nBuckets];
                    Bounds3f  boundsPre;
                    int countPre = 0;
                    for (size_t i = 0; i < nBuckets; i++) {
                        boundsPre = Union(boundsPre, buckets[i].bounds);
                        countPre += buckets[i].count;
                        costPre[i] = countPre * boundsPre.Area();
                    }
                    Bounds3f boundsSuf;
                    int countSuf = 0;
                    for (int i = nBuckets - 1; i >= 0; i--) {
                        boundsSuf = Union(boundsSuf, buckets[i].bounds);
                        countSuf += buckets[i].count;
                        costSuf[i] = countSuf * boundsSuf.Area();
                    }

                    // Find bucket to split at that minimizes SAH metric
                    Float minCost = costPre[0] + costSuf[1];                    
                    int minCostSplitBucket = 0; // Split after minCostSplitBucket                     
                    for (int i = 1; i < nBuckets - 1; i++) {
                        if (costPre[i] + costSuf[i + 1] < minCost) {
                            minCost = costPre[i] + costSuf[i + 1];
                            minCostSplitBucket = i;
                        }
                    }
                    minCost = minCost * bounds.Area() + 1;

                    // Either create leaf or split primitives at selected SAH bucket
                    Float leafCost = nPrimitives;
                    if (nPrimitives > maxPrimsInNode || minCost < leafCost) {
                        BVHPrimitiveInfo* midPtr = std::partition(&primitiveInfo[begin], &primitiveInfo[end - 1] + 1,
                        [=](BVHPrimitiveInfo &prim) {
                            int b = nBuckets * centroidBounds.Offset(prim.centroid)[dim];
                            if (b == nBuckets) b = nBuckets - 1;
                            return b <= minCostSplitBucket;
                        });
                        mid = midPtr - &primitiveInfo[0];
                    }
                    else {
                        // Create leaf node
                        size_t firstPrimOffset = orderedPrims.size();
                        for (int i = begin; i < end; i++) {
                            int idx = primitiveInfo[i].idx;
                            orderedPrims.push_back(primitives[idx]);
                        }
                        node->InitLeaf(bounds, firstPrimOffset, nPrimitives);
                    }

                }
            }
            }
            node->InitInterior(bounds,
                RecursiveBuild(arena, primitiveInfo, begin, mid, totalNodes, orderedPrims),
                RecursiveBuild(arena, primitiveInfo, mid, end, totalNodes, orderedPrims), dim);
        }
    }

    return node;
}

// Return the offset of LinearNode in nodes array
int BVHAccelerator::FlattenBVHTree(BVHBuildNode * node, int * offset) {
    LinearBVHNode *linearNode = &nodes[*offset];
    linearNode->bounds = node->bounds;
    int myOffset = (*offset)++;
    if (node->nPrimitives > 0) {
        // Leaf Node
        linearNode->nPrimitives = node->nPrimitives;
        linearNode->primitivesOffset = node->firstPrimOffset;
    }
    else {
        // Interior Node
        linearNode->nPrimitives = 0;
        linearNode->axis = node->splitAxis;
        FlattenBVHTree(node->children[0], offset);
        linearNode->rightChildOffset = FlattenBVHTree(node->children[1], offset);
    }
    return myOffset;
}

bool BVHAccelerator::IntersectP(const Ray & ray, SurfaceInteraction * inter) const {
    if (!nodes) return false;
    bool hit = false;
    Vector3f invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
    int dirIsNeg[3] = { invDir.x < 0,invDir.y < 0,invDir.z < 0 };

    int currentNodeIndex = 0, toVisitOffset = 0;
    int nodesToVisit[64];
    while(true) {
        LinearBVHNode *node = &nodes[currentNodeIndex];
        if (node->bounds.Intersect(ray,invDir,dirIsNeg)) {
            if (node->nPrimitives > 0) {
                // Leaf node
                for (int i = 0; i < node->nPrimitives; i++)
                    if (primitives[node->primitivesOffset + i]->IntersectP(ray, inter))
                        hit = true;
                if (toVisitOffset == 0) break;
                currentNodeIndex = nodesToVisit[--toVisitOffset];
            }
            else {
                // Interior node
                if (dirIsNeg[node->axis]) {
                    nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                    currentNodeIndex = node->rightChildOffset;
                }
                else {
                    nodesToVisit[toVisitOffset++] = node->rightChildOffset;
                    currentNodeIndex = currentNodeIndex + 1;
                }
            }
        }
        else {
            if (toVisitOffset == 0) break;
            currentNodeIndex = nodesToVisit[--toVisitOffset];
        }
    }
    return hit;
}

bool BVHAccelerator::Intersect(const Ray & ray) const {
    if (!nodes) return false;
    Vector3f invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
    int dirIsNeg[3] = { invDir.x < 0,invDir.y < 0,invDir.z < 0 };

    int currentNodeIndex = 0, toVisitOffset = 0;
    int nodesToVisit[64];
    while (true) {
        LinearBVHNode *node = &nodes[currentNodeIndex];
        if (node->bounds.Intersect(ray, invDir, dirIsNeg)) {
            if (node->nPrimitives > 0) {
                // Leaf node
                for (int i = 0; i < node->nPrimitives; i++)
                    if (primitives[node->primitivesOffset + i]->Intersect(ray))
                        return true;
                if (toVisitOffset == 0) break;
                currentNodeIndex = nodesToVisit[--toVisitOffset];
            }
            else {
                // Interior node
                if (dirIsNeg[node->axis]) {
                    nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                    currentNodeIndex = node->rightChildOffset;
                }
                else {
                    nodesToVisit[toVisitOffset++] = node->rightChildOffset;
                    currentNodeIndex = currentNodeIndex + 1;
                }
            }
        }
        else {
            if (toVisitOffset == 0) break;
            currentNodeIndex = nodesToVisit[--toVisitOffset];
        }
    }
    return false;
}

BVHAccelerator* CreateBVHAccelerator(std::vector<std::shared_ptr<Primitive>>& primitives) {
    BVHAccelerator::SplitMethod splitMethod = BVHAccelerator::SplitMethod::SAH;
    return new BVHAccelerator(primitives, splitMethod);
}

RAINBOW_NAMESPACE_END
