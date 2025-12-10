// include/flags/ImageEdgeFlag.hpp
#pragma once

#include "../Core.hpp"
#include <string>

// Flag that draws an image as white edges using stb_image + a special shader
class ImageEdgeFlag : public Flag {
public:
    // imagePath: path to PNG/JPG file
    explicit ImageEdgeFlag(const std::string& imagePath);

    // build VAO/VBO + load texture + compile shaders
    void init() override;

    // override draw to use our own shader (not the global color shader)
    void draw() override;

    // cleanup (optional)
    void cleanup() override;

private:
    std::string imagePath;

    GLuint texture = 0;     // texture handle
    GLuint vao = 0, vbo = 0;
    GLuint program = 0;     // edge-detection shader

    int imgW = 0, imgH = 0, imgChannels = 0;

    GLuint compileShader(GLenum type, const char* src);
    GLuint createProgram(const char* vs, const char* fs);
};
