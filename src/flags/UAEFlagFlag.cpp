// src/flags/UAEFlag.cpp
// تعريف علم الإمارات باستخدام OpenGL + Mesh

#include "../../include/flags/UAEFlag.hpp"
#include <glad/glad.h>
#include <iostream>

using namespace std;

UAEFlag::UAEFlag()
    : Flag("UAE")
{
}

/*
    init:
        - شريط رأسي أحمر (على اليسار)
        - ثلاثة شرائط أفقية (أخضر - أبيض - أسود) في باقي العرض
*/
void UAEFlag::init() {
    mesh.vertices.clear();
    mesh.indices.clear();

    float left   = -0.8f;
    float right  =  0.8f;
    float top    =  0.75f;
    float bottom = -0.75f;

    // عرض شريط الهواية الأحمر
    float redRight = -0.3f;

    float height_top     = top;
    float height_middle1 = 0.25f;
    float height_middle2 = -0.25f;
    float height_bottom  = bottom;

    // ألوان
    float r_red   = 0.8f, g_red   = 0.0f, b_red   = 0.0f;
    float r_green = 0.0f, g_green = 0.6f, b_green = 0.3f;
    float r_white = 1.0f, g_white = 1.0f, b_white = 1.0f;
    float r_black = 0.0f, g_black = 0.0f, b_black = 0.0f;

    float localVertices[] = {
        // === 1) الشريط الرأسي الأحمر (يسار) ===
        left,  bottom, 0.0f,   r_red, g_red, b_red,
        redRight, bottom, 0.0f,   r_red, g_red, b_red,
        left,  top,    0.0f,   r_red, g_red, b_red,

        redRight, top,    0.0f,   r_red, g_red, b_red,
        left,     top,    0.0f,   r_red, g_red, b_red,
        redRight, bottom, 0.0f,   r_red, g_red, b_red,

        // === 2) الشريط العلوي (أخضر) على اليمين ===
        redRight, height_middle1, 0.0f,   r_green, g_green, b_green,
        right,    height_middle1, 0.0f,   r_green, g_green, b_green,
        redRight, height_top,     0.0f,   r_green, g_green, b_green,

        right,    height_top,     0.0f,   r_green, g_green, b_green,
        redRight, height_top,     0.0f,   r_green, g_green, b_green,
        right,    height_middle1, 0.0f,   r_green, g_green, b_green,

        // === 3) الشريط الأوسط (أبيض) على اليمين ===
        redRight, height_middle2, 0.0f,   r_white, g_white, b_white,
        right,    height_middle2, 0.0f,   r_white, g_white, b_white,
        redRight, height_middle1, 0.0f,   r_white, g_white, b_white,

        right,    height_middle1, 0.0f,   r_white, g_white, b_white,
        redRight, height_middle1, 0.0f,   r_white, g_white, b_white,
        right,    height_middle2, 0.0f,   r_white, g_white, b_white,

        // === 4) الشريط السفلي (أسود) على اليمين ===
        redRight, height_bottom,  0.0f,   r_black, g_black, b_black,
        right,    height_bottom,  0.0f,   r_black, g_black, b_black,
        redRight, height_middle2, 0.0f,   r_black, g_black, b_black,

        right,    height_middle2, 0.0f,   r_black, g_black, b_black,
        redRight, height_middle2, 0.0f,   r_black, g_black, b_black,
        right,    height_bottom,  0.0f,   r_black, g_black, b_black,
    };

    mesh.vertices.assign(localVertices, localVertices + sizeof(localVertices)/sizeof(float));

    // 4 rectangles × 6 points = 24 points
    for (unsigned int i = 0; i < 24; ++i) {
        mesh.indices.push_back(i);
    }

    mesh.upload();
}

void UAEFlag::draw() {
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
