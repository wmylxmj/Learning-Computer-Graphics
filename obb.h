//
// Created by wmy on 2024/8/18.
//

#ifndef LEARNINGCG_OBB_H
#define LEARNINGCG_OBB_H

#include <glm/glm.hpp>
#include "mesh.h"

typedef struct {
    glm::mat3 basis;
    glm::vec3 size;
    glm::vec3 center;

} OrientedBoundingBox;

extern OrientedBoundingBox GenerateOBB(const std::vector<Vertex>& vertices, const std::vector<Triangle>& triangles);


typedef struct {
    glm::vec4 basis[3];
    glm::vec4 size;
    glm::vec4 center;
} OrientedBoundingBoxStd430;

extern OrientedBoundingBoxStd430 OBB2OBBStd430(const OrientedBoundingBox& obb);


#endif //LEARNINGCG_OBB_H
