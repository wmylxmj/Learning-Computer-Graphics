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

#endif //LEARNINGCG_OBB_H
