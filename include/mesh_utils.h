#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include <cstddef>
#include <vector>
#include <glad/glad.h>

namespace mesh_utils
{
    struct Mesh
    {
        GLuint vao = 0;
        GLuint vbo = 0;
        int vertexCount = 0;
    };

    inline Mesh createTexturedMesh(const void* vertexData, std::size_t vertexDataSize, int vertexCount)
    {
        Mesh mesh{};
        mesh.vertexCount = vertexCount;

        glGenVertexArrays(1, &mesh.vao);
        glGenBuffers(1, &mesh.vbo);

        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexDataSize), vertexData, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * static_cast<GLsizei>(sizeof(float)), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * static_cast<GLsizei>(sizeof(float)), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);

        return mesh;
    }

    inline Mesh createTexturedCubeMesh()
    {
        const float vertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

            // Front face
            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

            // Left face
            -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,

            // Right face
             0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
             0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

            // Top face
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f, 0.0f, 0.0f
        };

        return createTexturedMesh(vertices, sizeof(vertices), 36);
    }

    inline void destroyMesh(Mesh& mesh)
    {
        if (mesh.vao != 0)
        {
            glDeleteVertexArrays(1, &mesh.vao);
            mesh.vao = 0;
        }
        if (mesh.vbo != 0)
        {
            glDeleteBuffers(1, &mesh.vbo);
            mesh.vbo = 0;
        }
    }
}

#endif
