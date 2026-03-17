#ifndef SCENE_LAYOUT_H
#define SCENE_LAYOUT_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace scene_layout
{
    struct ObjectSpec
    {
        bool useCubeMesh = false;
        std::string modelPath;
        glm::mat4 model;
        glm::vec3 color;
        std::string numberTexturePath;
        std::string materialTexturePath;
    };

    inline std::vector<ObjectSpec> createObjectSpecs()
    {
        return {
            // Three pedestal cubes with number textures 1, 2, 3.
            {
                true,
                "",
                glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f)),
                glm::vec3(1.0f, 0.35f, 0.35f),
                "textures/one.png",
                "textures/ice.jpg"
            },
            {
                true,
                "",
                glm::mat4(1.0f),
                glm::vec3(0.35f, 1.0f, 0.35f),
                "textures/two.png",
                "textures/bricks.jpg"
            },
            {
                true,
                "",
                glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f)),
                glm::vec3(0.35f, 0.55f, 1.0f),
                "textures/three.png",
                "textures/metal.jpg"
            },
            // Additional file-based objects for task 5.
            {
                false,
                "models/snowman.obj",
                glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -2.0f, -4.0f)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(1.3f)),
                glm::vec3(0.95f, 0.95f, 1.0f),
                "textures/one.png",
                "textures/ice.jpg"
            },
            {
                false,
                "models/tux.obj",
                glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.1f, -4.5f)) *
                    glm::rotate(glm::mat4(1.0f), 0.65f, glm::vec3(0.0f, 1.0f, 0.0f)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(1.4f)),
                glm::vec3(1.0f, 0.92f, 0.85f),
                "textures/two.png",
                "textures/bricks.jpg"
            },
            {
                false,
                "models/firefox.obj",
                glm::translate(glm::mat4(1.0f), glm::vec3(4.8f, -2.1f, -5.0f)) *
                    glm::rotate(glm::mat4(1.0f), -0.55f, glm::vec3(0.0f, 1.0f, 0.0f)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(1.45f)),
                glm::vec3(0.95f, 0.55f, 0.35f),
                "textures/three.png",
                "textures/metal.jpg"
            }
        };
    }
}

#endif
