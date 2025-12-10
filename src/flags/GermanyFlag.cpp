// src/flags/GermanyFlag.cpp
// تعريف علم ألمانيا باستخدام OpenGL + Mesh

#include "../../include/flags/GermanyFlag.hpp"
#include <glad/glad.h>
#include <iostream>

using namespace std;

GermanyFlag::GermanyFlag()
    : Flag("Germany")
{
}

/*
    init:
        - ثلاث شرائط أفقية:
            * أسود - أحمر - ذهبي
*/
void GermanyFlag::init() {
    mesh.vertices.clear();
    mesh.indices.clear();

    float width          = 0.8f;
    float height_top     = 0.75f;
    float height_middle1 = 0.25f;
    float height_middle2 = -0.25f;
    float height_bottom  = -0.75f;

    // ألوان
    float r_black = 0.0f, g_black = 0.0f, b_black = 0.0f;
    float r_red   = 0.8f, g_red   = 0.0f, b_red   = 0.0f;
    float r_gold  = 1.0f, g_gold  = 0.8f, b_gold  = 0.0f;

    float localVertices[] = {
        // === 1) الشريط العلوي (أسود) ===
        -width, height_middle1, 0.0f,   r_black, g_black, b_black,
         width, height_middle1, 0.0f,   r_black, g_black, b_black,
        -width, height_top,     0.0f,   r_black, g_black, b_black,

         width, height_top,     0.0f,   r_black, g_black, b_black,
        -width, height_top,     0.0f,   r_black, g_black, b_black,
         width, height_middle1, 0.0f,   r_black, g_black, b_black,

        // === 2) الشريط الأوسط (أحمر) ===
        -width, height_middle2, 0.0f,   r_red, g_red, b_red,
         width, height_middle2, 0.0f,   r_red, g_red, b_red,
        -width, height_middle1, 0.0f,   r_red, g_red, b_red,

         width, height_middle1, 0.0f,   r_red, g_red, b_red,
        -width, height_middle1, 0.0f,   r_red, g_red, b_red,
         width, height_middle2, 0.0f,   r_red, g_red, b_red,

        // === 3) الشريط السفلي (ذهبي) ===
        -width, height_bottom,  0.0f,   r_gold, g_gold, b_gold,
         width, height_bottom,  0.0f,   r_gold, g_gold, b_gold,
        -width, height_middle2, 0.0f,   r_gold, g_gold, b_gold,

         width, height_middle2, 0.0f,   r_gold, g_gold, b_gold,
        -width, height_middle2, 0.0f,   r_gold, g_gold, b_gold,
         width, height_bottom,  0.0f,   r_gold, g_gold, b_gold,
    };

    mesh.vertices.assign(localVertices, localVertices + sizeof(localVertices)/sizeof(float));

    for (unsigned int i = 0; i < 18; ++i) {
        mesh.indices.push_back(i);
    }

    mesh.upload();
}

void GermanyFlag::draw() {
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
