//
// Created by user on 2024/8/28.
//

#include "bvh.h"
#include <algorithm>

struct CompareTriangle {
    std::vector<Vertex> vertices;
    glm::mat3 basis;
    int axis;

public:
    CompareTriangle(const std::vector<Vertex>& vertices, const glm::mat3& basis, int axis) :
    vertices((vertices)) , basis(basis), axis(axis) {}





    bool operator()(const Triangle& l_triangle, const Triangle& r_triangle) const {
        const glm::vec3 l_center = (vertices[l_triangle.indices[0]].position + vertices[l_triangle.indices[1]].position + vertices[l_triangle.indices[2]].position) / 3.0f;
        const glm::vec3 r_center = (vertices[r_triangle.indices[0]].position + vertices[r_triangle.indices[1]].position + vertices[r_triangle.indices[2]].position) / 3.0f;
        glm::vec3 l_basisCoord = glm::inverse(basis) * l_center;
        glm::vec3 r_basisCoord = glm::inverse(basis) * r_center;
        return l_basisCoord[axis] < r_basisCoord[axis];
    }


};

unsigned int buildBVHCore(const std::vector<Vertex>& vertices, std::vector<Triangle>& triangles, std::vector<BVHNode>& nodes, const unsigned int left, const unsigned int right, unsigned int maxNtris) {
    if (right <= left) return 0;
    unsigned int index = nodes.size();
    nodes.emplace_back();
    nodes[index].obb = GenerateOBB(vertices, std::vector<Triangle>(triangles.begin() + left, triangles.begin() + right));
    if ((right - left) <= maxNtris) {
        nodes[index].index = left;
        nodes[index].ntris = right - left;
        nodes[index].leftChild = 0;
        nodes[index].rightChild = 0;
        return index;
    }
    int axis = nodes[index].obb.size[0] > nodes[index].obb.size[1] ? 0 : 1;
    axis = nodes[index].obb.size[axis] > nodes[index].obb.size[2] ? axis : 2;
    std::sort(triangles.begin() + left, triangles.begin() + right, CompareTriangle(vertices, nodes[index].obb.basis, axis));
    const unsigned int mid = (left + right) / 2;
    nodes[index].leftChild = buildBVHCore(vertices, triangles, nodes, left, mid, maxNtris);
    nodes[index].rightChild = buildBVHCore(vertices, triangles, nodes, mid, right, maxNtris);
    nodes[index].index = 0;
    nodes[index].ntris = 0;
    return index;
}

std::vector<BVHNode> buildBVH(const std::vector<Vertex>& vertices, std::vector<Triangle>& triangles, unsigned int maxNtris) {
    std::vector<BVHNode> nodes;
    buildBVHCore(vertices, triangles, nodes, 0, triangles.size(), maxNtris);
    return std::move(nodes);
}

BVHNodeStd430 BVHNode2BVHNodeStd430(const BVHNode &node) {
    BVHNodeStd430 node_std430;
    node_std430.leftChild = node.leftChild;
    node_std430.rightChild = node.rightChild;
    node_std430.index = node.index;
    node_std430.ntris = node.ntris;
    node_std430.obb = OBB2OBBStd430(node.obb);
    return node_std430;
}

std::vector<BVHNodeStd430> BVHNodes2BVHNodesStd430(const std::vector<BVHNode> &nodes) {
    std::vector<BVHNodeStd430> nodes_std430;
    nodes_std430.reserve(nodes.size());
    for (auto node: nodes) {
        nodes_std430.push_back(BVHNode2BVHNodeStd430(node));
    }
    return nodes_std430;
}
