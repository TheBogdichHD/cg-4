#ifndef INPUT_CONTROLLER_H
#define INPUT_CONTROLLER_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace input_controller
{
    constexpr float CAMERA_SPEED = 5.5f;
    constexpr float FAST_CAMERA_MULTIPLIER = 2.0f;
    constexpr float MOUSE_SENSITIVITY = 0.09f;
    constexpr float MIN_CAMERA_PITCH = -89.0f;
    constexpr float MAX_CAMERA_PITCH = 89.0f;
    constexpr float MIX_CHANGE_SPEED = 0.9f;

    struct AppState
    {
        glm::vec3 cameraPosition = glm::vec3(0.0f, 1.2f, 10.5f);
        glm::vec3 cameraFront = glm::vec3(0.0f, -0.12f, -0.99f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        float cameraYaw = -90.0f;
        float cameraPitch = -7.0f;
        float numberMix = 0.5f;
        float textureMix = 1.0f;
        bool firstMouseSample = true;
        bool cursorCaptured = true;
        double lastMouseX = 0.0;
        double lastMouseY = 0.0;
        bool tabLatch = false;
    };

    struct MixDebugState
    {
        float lastPrintedNumberMix = -1.0f;
        float lastPrintedTextureMix = -1.0f;
    };

    inline void updateCameraFront(AppState& appState)
    {
        const float yawRadians = glm::radians(appState.cameraYaw);
        const float pitchRadians = glm::radians(appState.cameraPitch);

        glm::vec3 front;
        front.x = std::cos(yawRadians) * std::cos(pitchRadians);
        front.y = std::sin(pitchRadians);
        front.z = std::sin(yawRadians) * std::cos(pitchRadians);
        appState.cameraFront = glm::normalize(front);
    }

    inline bool pressedOnce(GLFWwindow* window, int key, bool& latch)
    {
        const bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
        const bool triggered = isPressed && !latch;
        latch = isPressed;
        return triggered;
    }

    inline void processMouseLook(AppState& appState, double mouseX, double mouseY)
    {
        if (!appState.cursorCaptured)
        {
            return;
        }

        if (appState.firstMouseSample)
        {
            appState.lastMouseX = mouseX;
            appState.lastMouseY = mouseY;
            appState.firstMouseSample = false;
            return;
        }

        const float xOffset = static_cast<float>(mouseX - appState.lastMouseX) * MOUSE_SENSITIVITY;
        const float yOffset = static_cast<float>(appState.lastMouseY - mouseY) * MOUSE_SENSITIVITY;

        appState.lastMouseX = mouseX;
        appState.lastMouseY = mouseY;

        appState.cameraYaw += xOffset;
        appState.cameraPitch = std::clamp(appState.cameraPitch + yOffset, MIN_CAMERA_PITCH, MAX_CAMERA_PITCH);
        updateCameraFront(appState);
    }

    inline void processInput(GLFWwindow* window, float deltaTime, AppState& appState, MixDebugState& debugState)
    {
        const float speedMultiplier = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? FAST_CAMERA_MULTIPLIER : 1.0f;
        const float cameraSpeed = CAMERA_SPEED * speedMultiplier * deltaTime;
        const glm::vec3 right = glm::normalize(glm::cross(appState.cameraFront, appState.cameraUp));

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) appState.cameraPosition += appState.cameraFront * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) appState.cameraPosition -= appState.cameraFront * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) appState.cameraPosition -= right * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) appState.cameraPosition += right * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) appState.cameraPosition -= appState.cameraUp * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) appState.cameraPosition += appState.cameraUp * cameraSpeed;

        if (pressedOnce(window, GLFW_KEY_TAB, appState.tabLatch))
        {
            appState.cursorCaptured = !appState.cursorCaptured;
            appState.firstMouseSample = true;
            glfwSetInputMode(window, GLFW_CURSOR, appState.cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }

        bool mixUpdated = false;
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        {
            appState.numberMix = std::clamp(appState.numberMix - MIX_CHANGE_SPEED * deltaTime, 0.0f, 1.0f);
            mixUpdated = true;
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        {
            appState.numberMix = std::clamp(appState.numberMix + MIX_CHANGE_SPEED * deltaTime, 0.0f, 1.0f);
            mixUpdated = true;
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            appState.textureMix = std::clamp(appState.textureMix - MIX_CHANGE_SPEED * deltaTime, 0.0f, 1.0f);
            mixUpdated = true;
        }
        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        {
            appState.textureMix = std::clamp(appState.textureMix + MIX_CHANGE_SPEED * deltaTime, 0.0f, 1.0f);
            mixUpdated = true;
        }

        if (mixUpdated)
        {
            if (std::abs(appState.numberMix - debugState.lastPrintedNumberMix) >= 0.02f)
            {
                std::cout << "numberMix=" << appState.numberMix << std::endl;
                debugState.lastPrintedNumberMix = appState.numberMix;
            }
            if (std::abs(appState.textureMix - debugState.lastPrintedTextureMix) >= 0.02f)
            {
                std::cout << "textureMix=" << appState.textureMix << std::endl;
                debugState.lastPrintedTextureMix = appState.textureMix;
            }
        }
    }

    inline void processInput(GLFWwindow* window, float deltaTime, AppState& appState)
    {
        static MixDebugState fallbackDebugState;
        processInput(window, deltaTime, appState, fallbackDebugState);
    }
}

#endif
