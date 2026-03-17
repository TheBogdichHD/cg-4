#ifndef WINDOW_UTILS_H
#define WINDOW_UTILS_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace window_utils
{
    inline GLFWwindow* createWindowAndInitOpenGL(int width, int height, const char* title, GLFWframebuffersizefun resizeCallback)
    {
        if (!glfwInit())
        {
            std::cout << "Failed to initialize GLFW" << std::endl;
            return nullptr;
        }

        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (window == nullptr)
        {
            std::cout << "Failed to open GLFW window" << std::endl;
            glfwTerminate();
            return nullptr;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            return nullptr;
        }

        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(window, resizeCallback);
        glEnable(GL_DEPTH_TEST);

        return window;
    }
}

#endif
