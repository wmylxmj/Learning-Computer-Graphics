//
// Created by wmy on 2024/8/18.
//

#include "obb.h"
#include <set>
#include <Eigen/Dense>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

OrientedBoundingBox GenerateOBB(const std::vector<Vertex>& vertices, const std::vector<Triangle>& triangles) {
    OrientedBoundingBox obb;
    std::set<unsigned int> vertexSet;
    glm::vec3 centroid(0.0f);
    for (Triangle triangle: triangles) {
        for (unsigned int index: triangle.indices) {
            if (vertexSet.find(index) == vertexSet.end()) {
                vertexSet.insert(index);
                centroid += vertices[index].position;
            }
        }
    }
    centroid /= vertexSet.size();
    glm::mat3 covariance(0.0f);
    for (unsigned int index: vertexSet) {
        glm::vec3 diff = vertices[index].position - centroid;
        covariance += glm::outerProduct(diff, diff);
    }
    covariance /= vertexSet.size();
    Eigen::Matrix3f covMatrix = Eigen::Map<Eigen::Matrix3f>(&covariance[0][0]);
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> solver(covMatrix);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            obb.basis[i][j] = solver.eigenvectors().col(i)[j];
        }
    }
    glm::vec3 minExtentsOBBCoord(+FLT_MAX, +FLT_MAX, +FLT_MAX);
    glm::vec3 maxExtentsOBBCoord(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (unsigned int index: vertexSet) {
        minExtentsOBBCoord = glm::min(minExtentsOBBCoord, glm::inverse(obb.basis) * vertices[index].position);
        maxExtentsOBBCoord = glm::max(maxExtentsOBBCoord, glm::inverse(obb.basis) * vertices[index].position);
    }
    obb.size = maxExtentsOBBCoord - minExtentsOBBCoord;
    glm::vec3 centerOBBCoord = (maxExtentsOBBCoord + minExtentsOBBCoord) * 0.5f;
    obb.center = obb.basis * centerOBBCoord;
    return obb;
}




OrientedBoundingBoxStd430 OBB2OBBStd430(const OrientedBoundingBox &obb) {
    OrientedBoundingBoxStd430 obb_std430;
    obb_std430.basis[0] = glm::vec4(obb.basis[0], 1.0);
    obb_std430.basis[1] = glm::vec4(obb.basis[1], 1.0);
    obb_std430.basis[2] = glm::vec4(obb.basis[2], 1.0);
    obb_std430.size = glm::vec4(obb.size, 0.0);
    obb_std430.center = glm::vec4(obb.center, 1.0);
    return obb_std430;
}

