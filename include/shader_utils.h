#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <glad/glad.h>

namespace shader_utils
{
    const std::string defaultVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uMVP;

out vec2 vTexCoord;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
    vTexCoord = aTexCoord;
}

)";

    const std::string defaultFragmentShader = R"(
#version 330 core
in vec2 vTexCoord;

uniform sampler2D uNumberTex;
uniform sampler2D uMaterialTex;
uniform vec3 uColor;
uniform float uNumberMix;
uniform float uTextureMix;

out vec4 FragColor;

void main()
{
    vec3 materialColor = texture(uMaterialTex, vTexCoord).rgb;
    vec3 numberColor = texture(uNumberTex, vTexCoord).rgb;

    vec3 mergedTextures = mix(materialColor, numberColor, clamp(uNumberMix, 0.0, 1.0));
    vec3 colorTintedTextures = uColor * mergedTextures;
    vec3 finalColor = mix(uColor, colorTintedTextures, clamp(uTextureMix, 0.0, 1.0));

    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}
)";

    inline std::string readTextFile(const std::string& filePath)
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            std::cout << "Failed to open file: " << filePath << std::endl;
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    inline GLuint compileShader(GLenum shaderType, const char* source)
    {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        int success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cout << "Shader compilation failed: " << infoLog << std::endl;
        }

        return shader;
    }

    inline GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource)
    {
        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        int success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
            std::cout << "Program link failed: " << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return program;
    }
}

#endif
