// src/flags/PirateFlag.cpp
// علم القراصنة - المرحلة 2:
// إطار أسود + فتحات مثلثية مفرّغة + سيفين متقاطعين باللون الأبيض

#include "../../include/flags/PirateFlag.hpp"
#include <glad/glad.h>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

PirateFlag::PirateFlag()
    : Flag("Pirates")
{
}

/*
    الفكرة:
      - init():
          * نبني Mesh يحتوي فقط المثلثات اللي هتكون فتحات (شفافة).
          * نخزن حدود المستطيل الأسود (left/right/top/bottom).
      - draw():
          1) نستخدم الـ stencil لرسم المثلثات كـ "ثقوب".
          2) نرسم المستطيل الأسود في الأماكن اللي مفيهاش ثقوب.
          3) مع بقاء نفس الـ stencil (clip)، نرسم السيفين المتقاطعين
             باللون الأبيض داخل مساحة العلم فقط (بدون ملمس الإطار).
*/

void PirateFlag::init() {
    mesh.vertices.clear();
    mesh.indices.clear();

    // helper: إضافة vertex واحد
    auto pushVertex = [&](float x, float y, float z,
                          float r, float g, float b) {
        mesh.vertices.push_back(x);
        mesh.vertices.push_back(y);
        mesh.vertices.push_back(z);
        mesh.vertices.push_back(r);
        mesh.vertices.push_back(g);
        mesh.vertices.push_back(b);
    };

    // helper: إضافة مثلث واحد بلون ثابت
    auto pushTriangle = [&](float x1,float y1,float z1,
                            float x2,float y2,float z2,
                            float x3,float y3,float z3,
                            float r,float g,float b) {
        pushVertex(x1,y1,z1,r,g,b);
        pushVertex(x2,y2,z2,r,g,b);
        pushVertex(x3,y3,z3,r,g,b);
    };

    // مش مهم اللون، لأننا بنستخدم المثلثات في الـ stencil فقط
    const float dummyR = 1.0f, dummyG = 1.0f, dummyB = 1.0f;

    // حدود العلم (مستطيل داخلي) - هنستخدمها في draw()
    float left   = -0.9f;
    float right  =  0.9f;
    float top    =  0.6f;
    float bottom = -0.6f;

    // ================
    // فتحات أعلى العلم
    // ================
    struct TopCut { float x0, x1, depth; };
    std::vector<TopCut> topCuts = {
        { -0.65f, -0.45f, 0.10f },
        { -0.15f,  0.05f, 0.06f },
        {  0.30f,  0.55f, 0.08f }
    };
    for (auto &c : topCuts) {
        float mid = 0.5f*(c.x0 + c.x1);
        float apexY = top - c.depth;        // داخل العلم
        pushTriangle(c.x0, top,   0.0f,
                     c.x1, top,   0.0f,
                     mid,  apexY, 0.0f,
                     dummyR,dummyG,dummyB);
    }

    // ================
    // فتحات أسفل العلم
    // ================
    struct BottomCut { float x0, x1, depth; };
    std::vector<BottomCut> bottomCuts = {
        { -0.55f, -0.30f, 0.08f },
        {  0.00f,  0.25f, 0.06f },
        {  0.50f,  0.75f, 0.10f }
    };
    for (auto &c : bottomCuts) {
        float mid = 0.5f*(c.x0 + c.x1);
        float apexY = bottom + c.depth;     // داخل العلم
        pushTriangle(c.x0, bottom, 0.0f,
                     c.x1, bottom, 0.0f,
                     mid,  apexY,  0.0f,
                     dummyR,dummyG,dummyB);
    }

    // ================
    // فتحات على اليمين
    // ================
    struct SideCut { float y0, y1, depth; };
    float xr = right;
    std::vector<SideCut> rightCuts = {
        {  0.45f,  0.25f, 0.10f },
        {  0.05f, -0.10f, 0.07f },
        { -0.30f, -0.50f, 0.12f }
    };
    for (auto &c : rightCuts) {
        float ym = 0.5f*(c.y0 + c.y1);
        float apexX = xr - c.depth;         // داخل العلم
        pushTriangle(xr,    c.y0, 0.0f,
                     xr,    c.y1, 0.0f,
                     apexX, ym,   0.0f,
                     dummyR,dummyG,dummyB);
    }

    // ================
    // فتحات على الشمال
    // ================
    float xl = left;
    std::vector<SideCut> leftCuts = {
        {  0.35f,  0.15f, 0.08f },
        { -0.05f, -0.25f, 0.06f }
    };
    for (auto &c : leftCuts) {
        float ym = 0.5f*(c.y0 + c.y1);
        float apexX = xl + c.depth;         // داخل العلم
        pushTriangle(xl,    c.y0, 0.0f,
                     xl,    c.y1, 0.0f,
                     apexX, ym,   0.0f,
                     dummyR,dummyG,dummyB);
    }

    // indices: كل 3 أرقام = vertex واحد (هنا نرسم triangles قائمة بدون إعادة استخدام)
    unsigned int totalVerts = static_cast<unsigned int>(mesh.vertices.size() / 6);
    for (unsigned int i = 0; i < totalVerts; ++i)
        mesh.indices.push_back(i);

    // نخزن الحدود في الكلاس
    this->left   = left;
    this->right  = right;
    this->top    = top;
    this->bottom = bottom;

    mesh.upload();
}

void PirateFlag::draw() {
    // =========================
    // 1) ضبط الـ stencil للفتحات
    // =========================
    glEnable(GL_STENCIL_TEST);

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    // لا نكتب ألوان الآن (نكتب فقط في الـ stencil)
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(mesh.indices.size()),
                   GL_UNSIGNED_INT,
                   0);
    glBindVertexArray(0);

    // =========================
    // 2) المستطيل الأسود (جسم العلم) مع احترام الـ stencil
    // =========================
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, 0, 0xFF);          // ارسم فقط حيث stencil == 0
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0x00);                       // لا نغيّر الـ stencil

    float flagR = 0.0f, flagG = 0.0f, flagB = 0.0f;
    float quad[] = {
        left,  bottom, 0.0f,  flagR, flagG, flagB,
        right, bottom, 0.0f,  flagR, flagG, flagB,
        right, top,    0.0f,  flagR, flagG, flagB,
        left,  top,    0.0f,  flagR, flagG, flagB
    };
    unsigned int idx[] = {0,1,2, 2,3,0};

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    GLsizei stride = 6 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);

    // =========================
    // 3) رسم السيوف المتقاطعة (يمين + يسار) - كل واحد معكوس بالنسبة للمحور
    // =========================
    auto drawBlade = [&](float baseStartX, float baseStartY,
                         float angleDeg, float length,
                         float thickness, bool /*mirrorUnused*/) // mirror لم نعد نحتاجه
    {
        const float PI = 3.14159265f;
        float angle = angleDeg * PI / 180.0f;

        // ألوان النصل والقبضة (قبضة فاتحة أكثر لتمييزها على الأسود)
        float bladeR = 1.0f, bladeG = 1.0f, bladeB = 1.0f;
        float handleR = 0.92f, handleG = 0.74f, handleB = 0.45f; // لون مقبض واضح و فاتح

        std::vector<float> verts;
        std::vector<unsigned int> indices;

        auto pushV = [&](float x,float y,float z,float r,float g,float b){
            verts.push_back(x); verts.push_back(y); verts.push_back(z);
            verts.push_back(r); verts.push_back(g); verts.push_back(b);
        };

        auto addQuad = [&](float x0,float y0,
                           float x1,float y1,
                           float x2,float y2,
                           float x3,float y3,
                           float z,float r,float g,float b){
            unsigned int base = static_cast<unsigned int>(verts.size()/6);
            pushV(x0,y0,z,r,g,b);
            pushV(x1,y1,z,r,g,b);
            pushV(x2,y2,z,r,g,b);
            pushV(x3,y3,z,r,g,b);
            indices.push_back(base+0);
            indices.push_back(base+1);
            indices.push_back(base+2);
            indices.push_back(base+2);
            indices.push_back(base+3);
            indices.push_back(base+0);
        };

        // محور السيف: سلسلة من مربعات صغيرة تظهر انحناء
        int N = 18;
        for(int i=0;i<N;i++){
            float t0 = (float)i / (float)N;
            float t1 = (float)(i+1) / (float)N;

            auto centerPoint = [&](float t){
                float baseX = baseStartX + cosf(angle) * t * length;
                float baseY = baseStartY + sinf(angle) * t * length;
                float bend = 0.18f * sinf(t * PI);  // انحناء بسيط
                float nxB = -sinf(angle);
                float nyB =  cosf(angle);
                return std::pair<float,float>(
                    baseX + nxB * bend,
                    baseY + nyB * bend
                );
            };

            auto p0 = centerPoint(t0);
            auto p1 = centerPoint(t1);

            float nx = -sinf(angle);
            float ny =  cosf(angle);

            float w0 = thickness * (1.0f - 0.35f * t0);
            float w1 = thickness * (1.0f - 0.35f * t1);

            float x0 = p0.first  + nx * w0;
            float y0 = p0.second + ny * w0;
            float x1 = p1.first  + nx * w1;
            float y1 = p1.second + ny * w1;
            float x2 = p1.first  - nx * w1;
            float y2 = p1.second - ny * w1;
            float x3 = p0.first  - nx * w0;
            float y3 = p0.second - ny * w0;

            addQuad(x0,y0, x1,y1, x2,y2, x3,y3, 0.02f, bladeR,bladeG,bladeB);
        }

        // قبضة السيف (مستطيل صغير)
        float handleLen  = 0.18f;
        float handleTh   = 0.055f;
        float hx0 = baseStartX - handleLen*0.5f;
        float hx1 = baseStartX + handleLen*0.5f;
        float hy0 = baseStartY - handleTh*0.5f;
        float hy1 = baseStartY + handleTh*0.5f;
        addQuad(hx0,hy0, hx1,hy0, hx1,hy1, hx0,hy1, 0.019f, handleR,handleG,handleB);

        // رفع البيانات مؤقتاً ورسم
        GLuint vaoB, vboB, eboB;
        glGenVertexArrays(1, &vaoB);
        glGenBuffers(1, &vboB);
        glGenBuffers(1, &eboB);

        glBindVertexArray(vaoB);
        glBindBuffer(GL_ARRAY_BUFFER, vboB);
        glBufferData(GL_ARRAY_BUFFER,
                     verts.size()*sizeof(float),
                     verts.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboB);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size()*sizeof(unsigned int),
                     indices.data(),
                     GL_STATIC_DRAW);

        GLsizei strideB = 6 * sizeof(float);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideB, (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideB, (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(vaoB);
        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(indices.size()),
                       GL_UNSIGNED_INT,
                       0);
        glBindVertexArray(0);

        glDeleteBuffers(1, &vboB);
        glDeleteBuffers(1, &eboB);
        glDeleteVertexArrays(1, &vaoB);
    };

    // موضع السيوف: اثنين متماثلين، واحد على +x وواحد على -x مع زاوية معكوسة
    float swordsY   = -0.28f;   // نزلناه شوية لانه طلبت كده
    float swordsX   =  0.33f;   // مكان الاعمدة
    float swordsLen =  0.85f;
    float swordsTh  =  0.030f;
    float swordsAng =  35.0f;   // ميل السيف (سيُعكس بالسالب للنسخة اليسرى)

    // السيف الأيمن
    drawBlade( swordsX,  swordsY,  swordsAng,  swordsLen, swordsTh, false );
    // السيف الأيسر (موقع -x وزاوية معكوسة)
    drawBlade(-swordsX,  swordsY, -swordsAng,  swordsLen, swordsTh, false );

    // =========================
    // 4) إيقاف الـ stencil
    // =========================
    glDisable(GL_STENCIL_TEST);
}
