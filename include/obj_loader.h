#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace obj_loader
{
    struct TexturedMeshData
    {
        std::vector<float> vertices;

        bool empty() const
        {
            return vertices.empty();
        }

        int vertexCount() const
        {
            return static_cast<int>(vertices.size() / 5);
        }
    };

    inline void appendVertex(TexturedMeshData& data, const aiVector3D& pos, const aiVector3D& uv)
    {
        data.vertices.push_back(pos.x);
        data.vertices.push_back(pos.y);
        data.vertices.push_back(pos.z);
        data.vertices.push_back(uv.x);
        data.vertices.push_back(uv.y);
    }

    inline bool loadModelPosUv(const std::string& filePath, TexturedMeshData& outData, std::string& errorMessage)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            filePath,
            aiProcess_Triangulate |
                aiProcess_JoinIdenticalVertices |
                aiProcess_FlipUVs |
                aiProcess_GenUVCoords
        );

        if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0 || scene->mRootNode == nullptr)
        {
            errorMessage = "Assimp load failed for " + filePath + ": " + importer.GetErrorString();
            return false;
        }

        for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
        {
            const aiMesh* mesh = scene->mMeshes[meshIndex];
            if (mesh == nullptr)
            {
                continue;
            }

            const bool hasTexCoords = mesh->HasTextureCoords(0);

            for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
            {
                const aiFace& face = mesh->mFaces[faceIndex];
                if (face.mNumIndices != 3)
                {
                    continue;
                }

                for (unsigned int i = 0; i < 3; ++i)
                {
                    const unsigned int vertexIndex = face.mIndices[i];
                    const aiVector3D& pos = mesh->mVertices[vertexIndex];
                    const aiVector3D uv = hasTexCoords ? mesh->mTextureCoords[0][vertexIndex] : aiVector3D(0.0f, 0.0f, 0.0f);
                    appendVertex(outData, pos, uv);
                }
            }
        }

        if (outData.empty())
        {
            errorMessage = "Model has no triangle data: " + filePath;
            return false;
        }

        return true;
    }
}

#endif
