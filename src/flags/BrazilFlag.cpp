// src/flags/BrazilFlag.cpp
// تعريف علم البرازيل باستخدام OpenGL + Mesh (VAO / VBO + indices)

#include "../../include/flags/BrazilFlag.hpp"
#include <glad/glad.h>
#include <cmath>
#include <iostream>

using namespace std;

// الكونستركتور: نمرر اسم العلم للكلاس الأساسي
BrazilFlag::BrazilFlag()
    : Flag("Brazil")    // نفس الاسم اللي بيظهر في الـ menu
{
}

/*
    init:
        - 1) مستطيل أخضر للخلفية
        - 2) ماسة صفراء (diamond) في المنتصف
        - 3) دائرة زرقاء في المنتصف (تقريب بعدد كبير من المثلثات)
        - 4) شريط أبيض مائل داخل حدود الدائرة (عرض أقل)
        - 5) نجوم صغيرة كثيرة في النصف السفلي من الدائرة
*/
void BrazilFlag::init() {
    mesh.vertices.clear();
    mesh.indices.clear();

    // ---------- ألوان العلم ----------
    const float greenR  = 0.00f, greenG  = 0.45f, greenB  = 0.20f;
    const float yellowR = 1.00f, yellowG = 0.85f, yellowB = 0.00f;
    const float blueR   = 0.06f, blueG   = 0.39f, blueB   = 0.64f;
    const float whiteR  = 1.00f, whiteG  = 1.00f, whiteB = 1.00f;

    // ---------- helper صغير لإضافة مثلث ----------
    auto addTri = [&](float x1, float y1,
                      float x2, float y2,
                      float x3, float y3,
                      float z,
                      float r, float g, float b)
    {
        // V1
        mesh.vertices.push_back(x1);
        mesh.vertices.push_back(y1);
        mesh.vertices.push_back(z);
        mesh.vertices.push_back(r);
        mesh.vertices.push_back(g);
        mesh.vertices.push_back(b);

        // V2
        mesh.vertices.push_back(x2);
        mesh.vertices.push_back(y2);
        mesh.vertices.push_back(z);
        mesh.vertices.push_back(r);
        mesh.vertices.push_back(g);
        mesh.vertices.push_back(b);

        // V3
        mesh.vertices.push_back(x3);
        mesh.vertices.push_back(y3);
        mesh.vertices.push_back(z);
        mesh.vertices.push_back(r);
        mesh.vertices.push_back(g);
        mesh.vertices.push_back(b);
    };

    // ---------- 1) الخلفية الخضراء (مستطيل كامل) ----------
    float l = -0.9f, r = 0.9f, t = 0.7f, b = -0.7f;
    addTri(l, b,  r, b,  l, t,  0.0f,  greenR, greenG, greenB);
    addTri(r, b,  r, t,  l, t,  0.0f,  greenR, greenG, greenB);

    // ---------- 2) الماسة الصفراء (diamond) ----------
    float cx = 0.0f, cy = 0.0f;
    float rx = 0.55f;   // نصف العرض
    float ry = 0.38f;   // نصف الارتفاع

    float leftX   = cx - rx;
    float rightX  = cx + rx;
    float topY    = cy + ry;
    float bottomY = cy - ry;

    addTri(leftX,  cy,      rightX, cy,       cx,     topY,    0.01f, yellowR, yellowG, yellowB); // أعلى
    addTri(leftX,  cy,      cx,     bottomY,  rightX, cy,      0.01f, yellowR, yellowG, yellowB); // أسفل

    // ---------- 3) الدائرة الزرقاء ----------
    const int segments = 64;
    float radius  = 0.28f;
    float circleZ = 0.02f;

    for (int i = 0; i < segments; ++i) {
        float a0 = (float)i       / segments * 2.0f * 3.14159265f;
        float a1 = (float)(i + 1) / segments * 2.0f * 3.14159265f;

        float x0 = cx + cosf(a0) * radius;
        float y0 = cy + sinf(a0) * radius;
        float x1 = cx + cosf(a1) * radius;
        float y1 = cy + sinf(a1) * radius;

        addTri(cx, cy,  x0, y0,  x1, y1,  circleZ,  blueR, blueG, blueB);
    }

    // ---------- 4) الشريط الأبيض المائل داخل حدود الدائرة (عرض أقل) ----------
    /*
        نظام إحداثيات محلي (x', y'):
            - x' موازي لاتجاه الشريط.
            - y' عمودي عليه.
        نرسم مستطيل من x' = -radius إلى x' = +radius
        بحيث أطرافه تلمس حدود الدائرة تقريباً.
    */
    float bandZ  = 0.03f;
    float bandH  = 0.06f;      // 👈 تم تقليل السمك (كان 0.10f)
    float halfH  = bandH * 0.5f;
    float bandRadius = radius * 0.98f;

    float angle  = -15.0f * 3.14159265f / 180.0f;  // زاوية الشريط
    float ca = cosf(angle);
    float sa = sinf(angle);

    // إزاحة بسيطة لأسفل داخل الدائرة
    float centerShiftYPrime = -0.03f;

    auto rotFromLocal = [&](float xp, float yp, float& outX, float& outY) {
        float yShifted = yp + centerShiftYPrime;
        outX = cx + (xp * ca - yShifted * sa);
        outY = cy + (xp * sa + yShifted * ca);
    };

    // أربع زوايا المستطيل في (x', y')
    float Axp = -bandRadius, Ayp = -halfH;
    float Bxp =  bandRadius, Byp = -halfH;
    float Cxp =  bandRadius, Cyp =  halfH;
    float Dxp = -bandRadius, Dyp =  halfH;

    float Ax, Ay, Bx, By, Cx, Cy, Dx, Dy;
    rotFromLocal(Axp, Ayp, Ax, Ay);
    rotFromLocal(Bxp, Byp, Bx, By);
    rotFromLocal(Cxp, Cyp, Cx, Cy);
    rotFromLocal(Dxp, Dyp, Dx, Dy);

    // المستطيل = مثلثين
    addTri(Ax, Ay,  Bx, By,  Cx, Cy,  bandZ, whiteR, whiteG, whiteB);
    addTri(Ax, Ay,  Cx, Cy,  Dx, Dy,  bandZ, whiteR, whiteG, whiteB);

    // ---------- 5) نجوم صغيرة كثيرة في النصف السفلي ----------
    auto addStar = [&](float sx, float sy, float size) {
        float sz    = 0.012f * size;   // نجمة صغيرة جداً
        float starZ = 0.04f;

        // مثلث بسيط صغير كـ star
        addTri(sx, sy + sz,
               sx - sz, sy - sz,
               sx + sz, sy - sz,
               starZ, whiteR, whiteG, whiteB);
    };

    // توزيع نجوم أكثر في النصف السفلي من الدائرة (y < 0 تقريباً)
    addStar(-0.11f, -0.02f, 1.0f);
    addStar(-0.06f, -0.03f, 0.9f);
    addStar(-0.02f, -0.04f, 0.8f);
    addStar( 0.02f, -0.05f, 0.9f);
    addStar( 0.06f, -0.06f, 0.8f);
    addStar( 0.10f, -0.07f, 0.9f);

    addStar(-0.09f, -0.08f, 0.7f);
    addStar(-0.04f, -0.09f, 0.8f);
    addStar( 0.00f, -0.10f, 0.7f);
    addStar( 0.05f, -0.11f, 0.9f);
    addStar( 0.09f, -0.09f, 0.8f);
    addStar( 0.13f, -0.06f, 0.7f);

    // ---------- بناء الـ indices بالتسلسل ----------
    unsigned int totalVerts = static_cast<unsigned int>(mesh.vertices.size() / 6); // كل vertex = 6 float
    for (unsigned int i = 0; i < totalVerts; ++i) {
        mesh.indices.push_back(i);
    }

    // رفع البيانات على الـ GPU
    mesh.upload();
}

void BrazilFlag::draw() {
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(mesh.indices.size()),
                   GL_UNSIGNED_INT,
                   0);
    glBindVertexArray(0);
}
