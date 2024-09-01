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




typedef struct {
    glm::vec4 position;
} VertexStd430;

extern VertexStd430 Vertex2VertexStd430(const Vertex& vertex);
extern std::vector<VertexStd430> Vertices2VerticesStd430(const std::vector<Vertex>& vertices);

typedef struct {
    unsigned int indices[3];
    unsigned int padding;
} TriangleStd430;

extern TriangleStd430 Triangle2TriangleStd430(const Triangle& triangle);
extern std::vector<TriangleStd430> Triangles2TrianglesStd430(const std::vector<Triangle>& triangles);



#endif //LEARNINGCG_MESH_H
