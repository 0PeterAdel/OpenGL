// src/flags/RomaniaFlag.cpp
// تعريف علم رومانيا باستخدام OpenGL + Mesh

#include "../../include/flags/RomaniaFlag.hpp"
#include <glad/glad.h>
#include <iostream>

using namespace std;

RomaniaFlag::RomaniaFlag()
    : Flag("Romania")
{
}

/*
    init:
        - ثلاث شرائط رأسية:
            * أزرق - أصفر - أحمر
*/
void RomaniaFlag::init() {
    mesh.vertices.clear();
    mesh.indices.clear();

    float left   = -0.8f;
    float right  =  0.8f;
    float top    =  0.75f;
    float bottom = -0.75f;

    // تقسيم العرض إلى 3 أجزاء
    float width  = right - left;
    float w3     = width / 3.0f;
    float x1     = left;
    float x2     = left + w3;
    float x3     = left + 2.0f * w3;
    float x4     = right;

    // ألوان
    float r_blue = 0.0f, g_blue = 0.0f, b_blue = 0.6f;
    float r_yel  = 1.0f, g_yel  = 0.8f, b_yel  = 0.0f;
    float r_red  = 0.8f, g_red  = 0.0f, b_red  = 0.0f;

    float localVertices[] = {
        // === 1) الشريط الأيسر (أزرق) ===
        x1, bottom, 0.0f,   r_blue, g_blue, b_blue,
        x2, bottom, 0.0f,   r_blue, g_blue, b_blue,
        x1, top,    0.0f,   r_blue, g_blue, b_blue,

        x2, top,    0.0f,   r_blue, g_blue, b_blue,
        x1, top,    0.0f,   r_blue, g_blue, b_blue,
        x2, bottom, 0.0f,   r_blue, g_blue, b_blue,

        // === 2) الشريط الأوسط (أصفر) ===
        x2, bottom, 0.0f,   r_yel, g_yel, b_yel,
        x3, bottom, 0.0f,   r_yel, g_yel, b_yel,
        x2, top,    0.0f,   r_yel, g_yel, b_yel,

        x3, top,    0.0f,   r_yel, g_yel, b_yel,
        x2, top,    0.0f,   r_yel, g_yel, b_yel,
        x3, bottom, 0.0f,   r_yel, g_yel, b_yel,

        // === 3) الشريط الأيمن (أحمر) ===
        x3, bottom, 0.0f,   r_red, g_red, b_red,
        x4, bottom, 0.0f,   r_red, g_red, b_red,
        x3, top,    0.0f,   r_red, g_red, b_red,

        x4, top,    0.0f,   r_red, g_red, b_red,
        x3, top,    0.0f,   r_red, g_red, b_red,
        x4, bottom, 0.0f,   r_red, g_red, b_red,
    };

    mesh.vertices.assign(localVertices, localVertices + sizeof(localVertices)/sizeof(float));

    for (unsigned int i = 0; i < 18; ++i) {
        mesh.indices.push_back(i);
    }

    mesh.upload();
}

void RomaniaFlag::draw() {
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
