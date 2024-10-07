//
// Created by wmy on 2024/8/18.
//

#include "fileio.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

Model::Model(const std::string& pFile) {
    Assimp::Importer importer;
    unsigned int pFlag = aiProcess_Triangulate |
                         aiProcess_JoinIdenticalVertices ;

    const aiScene *scene = importer.ReadFile(pFile, pFlag);

    if (scene == nullptr) {
        std::cout << importer.GetErrorString() << std::endl;
        return;
    }
    SceneProcessing(scene);
}

void Model::SceneProcessing(const aiScene *scene) {
    NodeProcessing(scene->mRootNode, scene);
}

void Model::NodeProcessing(aiNode* node, const aiScene* scene) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        MeshProcessing(mesh, scene);
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        NodeProcessing(node->mChildren[i], scene);
    }
}

void Model::MeshProcessing(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        Triangle triangle;

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            triangle.indices[j] = face.mIndices[j];
        }
        triangles.push_back(triangle);
    }
    meshes.emplace_back(std::move(vertices), std::move(triangles));
}