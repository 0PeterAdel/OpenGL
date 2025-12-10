// src/flags/GabonFlag.cpp
// تعريف علم الجابون باستخدام OpenGL + Mesh (VAO/VBO + indices)

#include "../../include/flags/GabonFlag.hpp"
#include <glad/glad.h>
#include <iostream>

using namespace std;

// الكونستركتور: نمرر اسم العلم للكلاس الأساسي
GabonFlag::GabonFlag()
    : Flag("Gabon")   // نفس الاسم اللي هنستخدمه في الـ menu في main.cpp
{
}

/*
    init:
        - هنا بنبني نقاط العلم (vertices)
        - بنجهز الـ Mesh:
            * mesh.vertices => إحداثيات + ألوان
            * mesh.indices  => ترتيب الرسم
        - بنرفع البيانات على الـ GPU عن طريق mesh.upload()
*/
void GabonFlag::init() {
    mesh.vertices.clear();
    mesh.indices.clear();

    float width          = 0.8f;
    float height_top     = 0.75f;
    float height_middle1 = 0.25f;
    float height_middle2 = -0.25f;
    float height_bottom  = -0.75f;

    float localVertices[] = {
        // === 1) Green stripe (top) ===
        -width, height_middle1, 0.0f,   0.0f, 0.6f, 0.3f,
         width, height_middle1, 0.0f,   0.0f, 0.6f, 0.3f,
        -width, height_top,     0.0f,   0.0f, 0.6f, 0.3f,
        
         width, height_top,     0.0f,   0.0f, 0.6f, 0.3f,
        -width, height_top,     0.0f,   0.0f, 0.6f, 0.3f,
         width, height_middle1, 0.0f,   0.0f, 0.6f, 0.3f,

        // === 2) Yellow stripe (middle) ===
        -width, height_middle2, 0.0f,   1.0f, 0.8f, 0.0f,
         width, height_middle2, 0.0f,   1.0f, 0.8f, 0.0f,
        -width, height_middle1, 0.0f,   1.0f, 0.8f, 0.0f,
        
         width, height_middle1, 0.0f,   1.0f, 0.8f, 0.0f,
        -width, height_middle1, 0.0f,   1.0f, 0.8f, 0.0f,
         width, height_middle2, 0.0f,   1.0f, 0.8f, 0.0f,

        // === 3) Blue stripe (bottom) ===
        -width, height_bottom,  0.0f,   0.0f, 0.3f, 0.7f,
         width, height_bottom,  0.0f,   0.0f, 0.3f, 0.7f,
        -width, height_middle2, 0.0f,   0.0f, 0.3f, 0.7f,
        
         width, height_middle2, 0.0f,   0.0f, 0.3f, 0.7f,
        -width, height_middle2, 0.0f,   0.0f, 0.3f, 0.7f,
         width, height_bottom,  0.0f,   0.0f, 0.3f, 0.7f,
    };

    mesh.vertices.assign(localVertices, localVertices + sizeof(localVertices)/sizeof(float));

    for(unsigned int i = 0; i < 18; i++) {
        mesh.indices.push_back(i);
    }

    mesh.upload();
}

void GabonFlag::draw() {
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
