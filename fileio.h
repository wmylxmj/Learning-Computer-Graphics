//
// Created by wmy on 2024/8/18.
//

#ifndef LEARNINGCG_FILEIO_H
#define LEARNINGCG_FILEIO_H

#include <string>
#include <vector>
#include <assimp/scene.h>
#include "mesh.h"

class Model {
public:
    std::vector<Mesh> meshes;

    explicit Model(const std::string& pFile);

private:

    void SceneProcessing(const aiScene *scene);

    void NodeProcessing(aiNode *node, const aiScene *scene);

    void MeshProcessing(aiMesh *mesh, const aiScene *scene);
};

#endif //LEARNINGCG_FILEIO_H
