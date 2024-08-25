//
// Created by wmy on 2024/8/18.
//

#include "mesh.h"


Mesh::Mesh(std::vector<Vertex> &&vertices, std::vector<Triangle> &&triangles) {
    this->vertices = vertices;
    this->triangles = triangles;
}
