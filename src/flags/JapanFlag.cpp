// src/flags/JapanFlag.cpp
// تعريف علم اليابان باستخدام OpenGL + Mesh (VAO/VBO + indices)

#include "../../include/flags/JapanFlag.hpp"
#include <glad/glad.h>
#include <cmath>
#include <iostream>

using namespace std;

// الكونستركتور: نمرر اسم العلم للكلاس الأساسي
JapanFlag::JapanFlag()
    : Flag("Japan")   // نفس الاسم اللي هنستخدمه في الـ menu في main.cpp
{
}

/*
    init:
        - هنا بنبني نقاط العلم (vertices)
        - بنرسم:
            * مستطيل أبيض يمثل خلفية العلم
            * دائرة حمراء في منتصف العلم (الشمس) كمجموعة مثلثات
        - بنجهز الـ Mesh:
            * mesh.vertices => إحداثيات + ألوان
            * mesh.indices  => ترتيب الرسم (كل 3 نقاط = Triangle)
        - بنرفع البيانات على الـ GPU عن طريق mesh.upload()
*/
void JapanFlag::init() {
    mesh.vertices.clear();
    mesh.indices.clear();

    // --- 1) مستطيل أبيض كخلفية للعلم ---
    // هنخلي العلم في منتصف الشاشة تقريباً
    float left   = -0.8f;
    float right  =  0.8f;
    float top    =  0.5f;
    float bottom = -0.5f;

    // لون الخلفية: أبيض
    float r_bg = 1.0f, g_bg = 1.0f, b_bg = 1.0f;

    // مستطيل = 2 مثلث = 6 نقاط
    float rectVertices[] = {
        // Triangle 1
        left,  bottom, 0.0f,   r_bg, g_bg, b_bg,
        right, bottom, 0.0f,   r_bg, g_bg, b_bg,
        left,  top,    0.0f,   r_bg, g_bg, b_bg,

        // Triangle 2
        right, top,    0.0f,   r_bg, g_bg, b_bg,
        left,  top,    0.0f,   r_bg, g_bg, b_bg,
        right, bottom, 0.0f,   r_bg, g_bg, b_bg,
    };

    // نضيف نقاط المستطيل إلى الـ mesh.vertices
    for (size_t i = 0; i < sizeof(rectVertices)/sizeof(float); ++i) {
        mesh.vertices.push_back(rectVertices[i]);
    }

    // --- 2) دائرة حمراء في المنتصف (الشمس) ---
    // هنستخدم Triangle List (كل مثلث = 3 نقاط: مركز + نقطتين على المحيط)
    const float PI = 3.14159265f;

    // مركز الدائرة
    float cx = 0.0f;
    float cy = 0.0f;
    float radius = 0.25f;

    // لون الدائرة: أحمر
    float r_c = 0.85f, g_c = 0.1f, b_c = 0.15f;

    int segments = 64; // كل ما زودنا العدد كل ما الدائرة بقت أنعم

    for (int i = 0; i < segments; ++i) {
        float angle1 = 2.0f * PI * (float)i / (float)segments;
        float angle2 = 2.0f * PI * (float)(i + 1) / (float)segments;

        // نقطة المركز
        mesh.vertices.push_back(cx);
        mesh.vertices.push_back(cy);
        mesh.vertices.push_back(0.0f);
        mesh.vertices.push_back(r_c);
        mesh.vertices.push_back(g_c);
        mesh.vertices.push_back(b_c);

        // نقطة على المحيط (الزاوية الأولى)
        mesh.vertices.push_back(cx + radius * cosf(angle1));
        mesh.vertices.push_back(cy + radius * sinf(angle1));
        mesh.vertices.push_back(0.0f);
        mesh.vertices.push_back(r_c);
        mesh.vertices.push_back(g_c);
        mesh.vertices.push_back(b_c);

        // نقطة على المحيط (الزاوية الثانية)
        mesh.vertices.push_back(cx + radius * cosf(angle2));
        mesh.vertices.push_back(cy + radius * sinf(angle2));
        mesh.vertices.push_back(0.0f);
        mesh.vertices.push_back(r_c);
        mesh.vertices.push_back(g_c);
        mesh.vertices.push_back(b_c);
    }

    // --- 3) بناء الـ indices ---
    // كل 3 قيم في vertices بيمثلوا Vertex واحد، وكل 3 Vertices = Triangle
    unsigned int vertexCount = static_cast<unsigned int>(mesh.vertices.size() / 6); // (x,y,z,r,g,b)

    for (unsigned int i = 0; i < vertexCount; ++i) {
        mesh.indices.push_back(i);
    }

    // Upload mesh data to GPU
    mesh.upload();
}

/*
    draw:
        - تستخدم الـ VAO اللي جوه mesh
        - تستدعي glDrawElements بعدد العناصر اللي في mesh.indices
        - الـ Shader المستخدم بيتحدد من برة (في main.cpp)
*/
void JapanFlag::draw() {
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
