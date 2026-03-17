#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <input_controller.h>
#include <mesh_utils.h>
#include <obj_loader.h>
#include <scene_layout.h>
#include <shader_utils.h>
#include <texture_utils.h>
#include <window_utils.h>

namespace
{
    constexpr int WINDOW_WIDTH = 1200;
    constexpr int WINDOW_HEIGHT = 800;
    constexpr float FOV_DEGREES = 45.0f;
    constexpr float NEAR_PLANE = 0.1f;
    constexpr float FAR_PLANE = 100.0f;
    constexpr float CLEAR_R = 0.07f;
    constexpr float CLEAR_G = 0.08f;
    constexpr float CLEAR_B = 0.12f;
    constexpr float CLEAR_A = 1.0f;

    struct SceneObjectGpu
    {
        mesh_utils::Mesh mesh;
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 color = glm::vec3(1.0f);
        GLuint numberTexture = 0;
        GLuint materialTexture = 0;
    };

    struct ShaderUniforms
    {
        GLint mvp = -1;
        GLint color = -1;
        GLint numberTex = -1;
        GLint materialTex = -1;
        GLint numberMix = -1;
        GLint textureMix = -1;

        static ShaderUniforms query(GLuint shaderProgram)
        {
            ShaderUniforms uniforms;
            uniforms.mvp = glGetUniformLocation(shaderProgram, "uMVP");
            uniforms.color = glGetUniformLocation(shaderProgram, "uColor");
            uniforms.numberTex = glGetUniformLocation(shaderProgram, "uNumberTex");
            uniforms.materialTex = glGetUniformLocation(shaderProgram, "uMaterialTex");
            uniforms.numberMix = glGetUniformLocation(shaderProgram, "uNumberMix");
            uniforms.textureMix = glGetUniformLocation(shaderProgram, "uTextureMix");
            return uniforms;
        }

        void bindTextureUnits() const
        {
            glUniform1i(numberTex, 0);
            glUniform1i(materialTex, 1);
        }
    };

    struct TextureCache
    {
        std::unordered_map<std::string, GLuint> textures;

        GLuint loadOrGet(const std::string& path)
        {
            const auto it = textures.find(path);
            if (it != textures.end())
            {
                return it->second;
            }

            const GLuint textureId = texture_utils::loadTexture2D(path);
            if (textureId != 0)
            {
                textures[path] = textureId;
            }
            return textureId;
        }

        void destroyAll() const
        {
            for (const auto& entry : textures)
            {
                glDeleteTextures(1, &entry.second);
            }
        }
    };

    bool createMeshForSpec(const scene_layout::ObjectSpec& spec, mesh_utils::Mesh& mesh)
    {
        if (spec.useCubeMesh)
        {
            mesh = mesh_utils::createTexturedCubeMesh();
            return true;
        }

        obj_loader::TexturedMeshData meshData;
        std::string errorMessage;
        if (!obj_loader::loadModelPosUv(spec.modelPath, meshData, errorMessage))
        {
            std::cout << errorMessage << std::endl;
            return false;
        }

        mesh = mesh_utils::createTexturedMesh(
            meshData.vertices.data(),
            meshData.vertices.size() * sizeof(float),
            meshData.vertexCount()
        );
        return true;
    }

    bool tryCreateSceneObject(
        const scene_layout::ObjectSpec& spec,
        TextureCache& textureCache,
        SceneObjectGpu& gpuObject
    )
    {
        gpuObject.model = spec.model;
        gpuObject.color = spec.color;
        gpuObject.numberTexture = textureCache.loadOrGet(spec.numberTexturePath);
        gpuObject.materialTexture = textureCache.loadOrGet(spec.materialTexturePath);

        if (gpuObject.numberTexture == 0 || gpuObject.materialTexture == 0)
        {
            std::cout << "Skipping object due to texture loading failure." << std::endl;
            return false;
        }

        if (!createMeshForSpec(spec, gpuObject.mesh))
        {
            return false;
        }

        if (gpuObject.mesh.vao == 0 || gpuObject.mesh.vertexCount <= 0)
        {
            std::cout << "Skipping object because mesh creation failed." << std::endl;
            return false;
        }

        return true;
    }

    std::vector<SceneObjectGpu> buildSceneObjects(
        const std::vector<scene_layout::ObjectSpec>& objectSpecs,
        TextureCache& textureCache
    )
    {
        std::vector<SceneObjectGpu> sceneObjects;
        for (const scene_layout::ObjectSpec& spec : objectSpecs)
        {
            SceneObjectGpu gpuObject;
            if (tryCreateSceneObject(spec, textureCache, gpuObject))
            {
                sceneObjects.push_back(gpuObject);
            }
        }
        return sceneObjects;
    }

    glm::mat4 createViewMatrix(const input_controller::AppState& appState)
    {
        return glm::lookAt(
            appState.cameraPosition,
            appState.cameraPosition + appState.cameraFront,
            appState.cameraUp
        );
    }

    glm::mat4 createProjectionMatrix(int fbWidth, int fbHeight)
    {
        const float aspect = static_cast<float>(fbWidth) / static_cast<float>(fbHeight > 0 ? fbHeight : 1);
        return glm::perspective(glm::radians(FOV_DEGREES), aspect, NEAR_PLANE, FAR_PLANE);
    }

    void renderSceneObjects(
        const std::vector<SceneObjectGpu>& sceneObjects,
        const ShaderUniforms& uniforms,
        const glm::mat4& projection,
        const glm::mat4& view,
        float numberMix,
        float textureMix
    )
    {
        for (const SceneObjectGpu& object : sceneObjects)
        {
            const glm::mat4 mvp = projection * view * object.model;
            glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform3fv(uniforms.color, 1, glm::value_ptr(object.color));
            glUniform1f(uniforms.numberMix, numberMix);
            glUniform1f(uniforms.textureMix, textureMix);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, object.numberTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, object.materialTexture);

            glBindVertexArray(object.mesh.vao);
            glDrawArrays(GL_TRIANGLES, 0, object.mesh.vertexCount);
            glBindVertexArray(0);
        }
    }

    void destroySceneMeshes(std::vector<SceneObjectGpu>& sceneObjects)
    {
        for (SceneObjectGpu& object : sceneObjects)
        {
            mesh_utils::destroyMesh(object.mesh);
        }
    }

    bool createShaderProgramFromFiles(GLuint& shaderProgram)
    {
        const std::string vertexShaderCode = shader_utils::readTextFile("shaders/basic.vert");
        const std::string fragmentShaderCode = shader_utils::readTextFile("shaders/basic.frag");
        if (vertexShaderCode.empty() || fragmentShaderCode.empty())
        {
            std::cout << "Shaders are missing. Ensure shaders/basic.vert and shaders/basic.frag are copied." << std::endl;
            return false;
        }

        shaderProgram = shader_utils::createShaderProgram(vertexShaderCode.c_str(), fragmentShaderCode.c_str());
        return true;
    }

    bool initializeSceneResources(
        GLuint shaderProgram,
        ShaderUniforms& uniforms,
        TextureCache& textureCache,
        std::vector<SceneObjectGpu>& sceneObjects
    )
    {
        uniforms = ShaderUniforms::query(shaderProgram);
        glUseProgram(shaderProgram);
        uniforms.bindTextureUnits();

        const std::vector<scene_layout::ObjectSpec> objectSpecs = scene_layout::createObjectSpecs();
        sceneObjects = buildSceneObjects(objectSpecs, textureCache);

        if (sceneObjects.empty())
        {
            std::cout << "No renderable objects were loaded." << std::endl;
            return false;
        }

        return true;
    }

    void processFrame(
        GLFWwindow* window,
        input_controller::AppState& appState,
        input_controller::MixDebugState& mixDebugState,
        float& lastFrame
    )
    {
        const float currentFrame = static_cast<float>(glfwGetTime());
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        input_controller::processInput(window, deltaTime, appState, mixDebugState);

        double mouseX = 0.0;
        double mouseY = 0.0;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        input_controller::processMouseLook(appState, mouseX, mouseY);
    }

    void renderFrame(
        GLFWwindow* window,
        GLuint shaderProgram,
        const ShaderUniforms& uniforms,
        const std::vector<SceneObjectGpu>& sceneObjects,
        const input_controller::AppState& appState
    )
    {
        int fbWidth = 1;
        int fbHeight = 1;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        const glm::mat4 projection = createProjectionMatrix(fbWidth, fbHeight);
        const glm::mat4 view = createViewMatrix(appState);

        glClearColor(CLEAR_R, CLEAR_G, CLEAR_B, CLEAR_A);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        renderSceneObjects(sceneObjects, uniforms, projection, view, appState.numberMix, appState.textureMix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void shutdownApp(GLuint shaderProgram, std::vector<SceneObjectGpu>& sceneObjects, const TextureCache& textureCache)
    {
        destroySceneMeshes(sceneObjects);
        textureCache.destroyAll();
        glDeleteProgram(shaderProgram);
        glfwTerminate();
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

int main()
{
    GLFWwindow* window = window_utils::createWindowAndInitOpenGL(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "OpenGL Tasks",
        framebuffer_size_callback
    );
    if (window == nullptr)
    {
        return -1;
    }

    input_controller::AppState appState;
    input_controller::MixDebugState mixDebugState;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLuint shaderProgram = 0;
    if (!createShaderProgramFromFiles(shaderProgram))
    {
        glfwTerminate();
        return -1;
    }

    ShaderUniforms uniforms;
    TextureCache textureCache;
    std::vector<SceneObjectGpu> sceneObjects;
    if (!initializeSceneResources(shaderProgram, uniforms, textureCache, sceneObjects))
    {
        glDeleteProgram(shaderProgram);
        glfwTerminate();
        return -1;
    }

    float lastFrame = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window))
    {
        processFrame(window, appState, mixDebugState, lastFrame);
        renderFrame(window, shaderProgram, uniforms, sceneObjects, appState);
    }

    shutdownApp(shaderProgram, sceneObjects, textureCache);
    return 0;
}
