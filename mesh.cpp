//
// Created by wmy on 2024/8/18.
//

#include "mesh.h"


Mesh::Mesh(std::vector<Vertex> &&vertices, std::vector<Triangle> &&triangles) {
    this->vertices = vertices;
    this->triangles = triangles;
}



VertexStd430 Vertex2VertexStd430(const Vertex& vertex) {
    VertexStd430 vertex_std430;
    vertex_std430.position = glm::vec4(vertex.position, 1.0);
    return vertex_std430;
}

std::vector<VertexStd430> Vertices2VerticesStd430(const std::vector<Vertex>& vertices) {
    std::vector<VertexStd430> vertices_std430;
    vertices_std430.reserve(vertices.size());
    for (auto vertex: vertices) {
        vertices_std430.push_back(Vertex2VertexStd430(vertex));
    }
    return vertices_std430;
}

TriangleStd430 Triangle2TriangleStd430(const Triangle& triangle) {
    return TriangleStd430{{triangle.indices[0], triangle.indices[1], triangle.indices[2]}, 0};
}

std::vector<TriangleStd430> Triangles2TrianglesStd430(const std::vector<Triangle> &triangles) {
    std::vector<TriangleStd430> triangles_std430;
    triangles_std430.reserve(triangles.size());
    for (auto triangle: triangles) {
        triangles_std430.push_back(Triangle2TriangleStd430(triangle));
    }
    return triangles_std430;
}

