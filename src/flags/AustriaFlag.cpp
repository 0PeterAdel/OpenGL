// src/flags/AustriaFlag.cpp
// تعريف علم النمسا باستخدام OpenGL + Mesh (VAO/VBO + indices)

#include "../../include/flags/AustriaFlag.hpp"
#include <glad/glad.h>
#include <iostream>

using namespace std;

// الكونستركتور: نمرر اسم العلم للكلاس الأساسي
AustriaFlag::AustriaFlag()
    : Flag("Austria")   // نفس الاسم اللي هنستخدمه في الـ menu في main.cpp
{
}

/*
    init:
        - هنا بنبني نقاط العلم (vertices)
        - ثلاث شرائط أفقية:
            * أحمر - أبيض - أحمر
        - بنجهز الـ Mesh:
            * mesh.vertices => إحداثيات + ألوان
            * mesh.indices  => ترتيب الرسم
        - بنرفع البيانات على الـ GPU عن طريق mesh.upload()
*/
void AustriaFlag::init() {
    mesh.vertices.clear();
    mesh.indices.clear();

    float width          = 0.8f;
    float height_top     = 0.75f;
    float height_middle1 = 0.25f;
    float height_middle2 = -0.25f;
    float height_bottom  = -0.75f;

    // أحمر
    float r_red = 0.85f, g_red = 0.0f, b_red = 0.0f;
    // أبيض
    float r_white = 1.0f, g_white = 1.0f, b_white = 1.0f;

    float localVertices[] = {
        // === 1) الشريط العلوي (أحمر) ===
        -width, height_middle1, 0.0f,   r_red, g_red, b_red,
         width, height_middle1, 0.0f,   r_red, g_red, b_red,
        -width, height_top,     0.0f,   r_red, g_red, b_red,

         width, height_top,     0.0f,   r_red, g_red, b_red,
        -width, height_top,     0.0f,   r_red, g_red, b_red,
         width, height_middle1, 0.0f,   r_red, g_red, b_red,

        // === 2) الشريط الأوسط (أبيض) ===
        -width, height_middle2, 0.0f,   r_white, g_white, b_white,
         width, height_middle2, 0.0f,   r_white, g_white, b_white,
        -width, height_middle1, 0.0f,   r_white, g_white, b_white,

         width, height_middle1, 0.0f,   r_white, g_white, b_white,
        -width, height_middle1, 0.0f,   r_white, g_white, b_white,
         width, height_middle2, 0.0f,   r_white, g_white, b_white,

        // === 3) الشريط السفلي (أحمر) ===
        -width, height_bottom,  0.0f,   r_red, g_red, b_red,
         width, height_bottom,  0.0f,   r_red, g_red, b_red,
        -width, height_middle2, 0.0f,   r_red, g_red, b_red,

         width, height_middle2, 0.0f,   r_red, g_red, b_red,
        -width, height_middle2, 0.0f,   r_red, g_red, b_red,
         width, height_bottom,  0.0f,   r_red, g_red, b_red,
    };

    mesh.vertices.assign(localVertices, localVertices + sizeof(localVertices)/sizeof(float));

    for (unsigned int i = 0; i < 18; ++i) {
        mesh.indices.push_back(i);
    }

    mesh.upload();
}

void AustriaFlag::draw() {
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
