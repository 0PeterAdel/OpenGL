// src/flags/ImageEdgeFlag.cpp
// Flag that loads an image with stb_image and draws only its edges
// as white lines using a Sobel filter in the fragment shader.

#include "../../include/flags/ImageEdgeFlag.hpp"
#include "../../thirdparty/stb_image.h"

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

static const char* edge_vs_src = R"(
#version 330 core
layout(location = 0) in vec2 aPos;  // NDC position
layout(location = 1) in vec2 aUV;   // texture coords

out vec2 vUV;

void main() {
    vUV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

static const char* edge_fs_src = R"(
#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTex;
uniform vec2 uTexelSize;   // 1/width, 1/height

void main() {
    // Sample neighborhood for Sobel edge detection
    float gx[9] = float[9](
        -1,  0,  1,
        -2,  0,  2,
        -1,  0,  1
    );
    float gy[9] = float[9](
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
    );

    vec2 offsets[9] = vec2[9](
        vec2(-1, -1), vec2(0, -1), vec2(1, -1),
        vec2(-1,  0), vec2(0,  0), vec2(1,  0),
        vec2(-1,  1), vec2(0,  1), vec2(1,  1)
    );

    float sx = 0.0;
    float sy = 0.0;
    for (int i = 0; i < 9; ++i) {
        vec2 uv = vUV + offsets[i] * uTexelSize;
        vec3 col = texture(uTex, uv).rgb;
        float lum = dot(col, vec3(0.299, 0.587, 0.114)); // grayscale
        sx += gx[i] * lum;
        sy += gy[i] * lum;
    }

    float edge = length(vec2(sx, sy));

    // threshold
    float line = edge > 0.2 ? 1.0 : 0.0;

    // white lines on transparent/dark background
    FragColor = vec4(vec3(line), line);
}
)";

ImageEdgeFlag::ImageEdgeFlag(const std::string& imagePath)
    : Flag("ImageEdges"), imagePath(imagePath)
{
}

GLuint ImageEdgeFlag::compileShader(GLenum type, const char* src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(sh, 1024, nullptr, log);
        std::cerr << "Shader compile error:\n" << log << std::endl;
    }
    return sh;
}

GLuint ImageEdgeFlag::createProgram(const char* vs, const char* fs) {
    GLuint v = compileShader(GL_VERTEX_SHADER, vs);
    GLuint f = compileShader(GL_FRAGMENT_SHADER, fs);
    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    GLint ok;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(p, 1024, nullptr, log);
        std::cerr << "Program link error:\n" << log << std::endl;
    }
    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

void ImageEdgeFlag::init() {
    // 1) Load image with stb_image
    stbi_set_flip_vertically_on_load(1); // Flip so texture is upright
    unsigned char* data = stbi_load(imagePath.c_str(),
                                    &imgW, &imgH, &imgChannels, 4);
    if (!data) {
        std::cerr << "Failed to load image: " << imagePath << std::endl;
        return;
    }

    // 2) Create texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // basic texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 imgW, imgH,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 data);

    glBindTexture(GL_TEXTURE_2D, 0);

    // We no longer need CPU copy
    stbi_image_free(data);

    // 3) Build shader
    program = createProgram(edge_vs_src, edge_fs_src);

    // 4) Full-screen-ish quad (we’ll keep inside the flag viewport)
    // Positions in NDC: we put a smaller quad so it fits nicely
    float sx = 0.6f; // horizontal half-size
    float sy = 0.5f; // vertical half-size

    float verts[] = {
        //  x,     y,      u, v
        -sx, -sy, 0.0f, 0.0f,
         sx, -sy, 1.0f, 0.0f,
         sx,  sy, 1.0f, 1.0f,
        -sx,  sy, 0.0f, 1.0f
    };
    unsigned int ind[] = {0,1,2, 2,3,0};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    GLuint ebo;
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind), ind, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);              // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float))); // uv
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // store EBO in VAO's state, no need to keep handle separate
    // (we could keep it as a member if we want to delete later)
}

void ImageEdgeFlag::draw() {
    if (!program || !texture || !vao) return;

    glUseProgram(program);
    glBindVertexArray(vao);

    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLint texLoc = glGetUniformLocation(program, "uTex");
    glUniform1i(texLoc, 0);

    // texel size for Sobel
    GLint tsLoc = glGetUniformLocation(program, "uTexelSize");
    if (tsLoc >= 0) {
        glUniform2f(tsLoc,
                    1.0f / static_cast<float>(imgW),
                    1.0f / static_cast<float>(imgH));
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ImageEdgeFlag::cleanup() {
    if (texture) {
        glDeleteTextures(1, &texture);
        texture = 0;
    }
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
}
