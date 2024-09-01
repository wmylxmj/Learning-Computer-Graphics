//
// Created by user on 2024/8/28.
//

#ifndef LEARNINGCG_BVH_H
#define LEARNINGCG_BVH_H
#include "obb.h"

typedef struct {
    unsigned int leftChild, rightChild;
    unsigned int index, ntris;
    OrientedBoundingBox obb;

} BVHNode;

extern  std::vector<BVHNode> buildBVH(const std::vector<Vertex>& vertices, std::vector<Triangle>& triangles, unsigned int maxNtris = 2);

typedef struct alignas(16) {
    unsigned int leftChild, rightChild;
    unsigned int index, ntris;
    OrientedBoundingBoxStd430 obb;
} BVHNodeStd430;


extern BVHNodeStd430 BVHNode2BVHNodeStd430(const BVHNode& node);
extern std::vector<BVHNodeStd430> BVHNodes2BVHNodesStd430(const std::vector<BVHNode>& nodes);

#endif //LEARNINGCG_BVH_H
