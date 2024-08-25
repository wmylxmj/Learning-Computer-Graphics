//
// Created by wmy on 2024/8/18.
//

#ifndef LEARNINGCG_MESH_H
#define LEARNINGCG_MESH_H

#include <glm/glm.hpp>
#include <vector>

typedef struct {
    glm::vec3 position;
} Vertex;

typedef struct {
    unsigned int indices[3];
} Triangle;

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    explicit Mesh(std::vector<Vertex> &&vertices, std::vector<Triangle> &&triangles);
};

#endif //LEARNINGCG_MESH_H
