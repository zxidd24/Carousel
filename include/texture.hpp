//
// Created by xmmmmmovo on 12/16/2023.
//

#ifndef INCLUDE_TEXTURE_069BC28650C645DE94E0C88FB6089BB7
#define INCLUDE_TEXTURE_069BC28650C645DE94E0C88FB6089BB7

#include <cstdint>
#include <filesystem>
#include <stb_image.h>

#include <glad/glad.h>
#include <array>

namespace cg {

inline uint32_t loadTexture(std::filesystem::path const& path, bool SRGB = false) {

    int      width, height, channels;
    uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &channels,
                              STBI_rgb_alpha);

    if (data == nullptr) { return 0; }

    uint32_t texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (SRGB) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
    }
    glGenerateMipmap(texture_id);

    stbi_image_free(data);

    return texture_id;
}

inline uint32_t loadCubeMapTexture(std::filesystem::path const&                dir,
                            std::array<std::filesystem::path, 6> const& paths) {

    uint32_t texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    int width, height, nr_channels;

    int cnt = 0;
    for (auto& path : paths) {
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load((dir / path).generic_string().c_str(), &width, &height,
                                        &nr_channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cnt, 0, GL_RGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << path
                      << '\n';
        }
        stbi_image_free(data);
        cnt++;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture_id;
}

}// namespace cg

#endif// INCLUDE_TEXTURE_069BC28650C645DE94E0C88FB6089BB7
