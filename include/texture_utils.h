#ifndef TEXTURE_UTILS_H
#define TEXTURE_UTILS_H

#include <iostream>
#include <string>
#include <glad/glad.h>
#include <stb/stb_image.h>

namespace texture_utils
{
    inline GLuint loadTexture2D(const std::string& filePath)
    {
        stbi_set_flip_vertically_on_load(1);

        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* pixels = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
        if (pixels == nullptr)
        {
            std::cout << "Failed to load texture: " << filePath << std::endl;
            return 0;
        }

        GLenum format = GL_RGB;
        GLenum internalFormat = GL_RGB8;
        if (channels == 1)
        {
            format = GL_RED;
            internalFormat = GL_R8;
        }
        else if (channels == 3)
        {
            format = GL_RGB;
            internalFormat = GL_RGB8;
        }
        else if (channels == 4)
        {
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
        }

        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
        return textureId;
    }
}

#endif
