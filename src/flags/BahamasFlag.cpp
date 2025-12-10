// src/flags/BahamasFlag.cpp
// تعريف علم جزر البهاما باستخدام OpenGL + Mesh

#include "../../include/flags/BahamasFlag.hpp"
#include <glad/glad.h>
#include <iostream>

using namespace std;

BahamasFlag::BahamasFlag()
    : Flag("Bahamas")
{
}

/*
    init:
        - نرسم:
          1) خلفية مقسمة: تركوازي - أصفر - تركوازي (ثلاثة مستطيلات أفقية)
          2) مثلث أسود على اليسار (hoist triangle)
*/
void BahamasFlag::init() {
    mesh.vertices.clear();
    mesh.indices.clear();

    float left   = -0.8f;
    float right  =  0.8f;
    float top    =  0.6f;
    float bottom = -0.6f;

    // تقسيم عمودي (ثلاث شرائط متساوية)
    float hTop = top;
    float hMid1 = 0.2f;
    float hMid2 = -0.2f;
    float hBot = bottom;

    // ألوان
    float tealR = 0.0f, tealG = 0.73f, tealB = 0.67f; // تركوازي
    float goldR = 1.0f, goldG = 0.84f, goldB = 0.0f;  // أصفر ذهبي
    float blackR = 0.02f, blackG = 0.02f, blackB = 0.02f;

    // top stripe (teal)
    float verts[] = {
        // top stripe (two triangles)
        left, hMid1, 0.0f,   tealR, tealG, tealB,
        right,hMid1, 0.0f,   tealR, tealG, tealB,
        left, hTop,  0.0f,   tealR, tealG, tealB,

        right,hTop,  0.0f,   tealR, tealG, tealB,
        left, hTop,  0.0f,   tealR, tealG, tealB,
        right,hMid1, 0.0f,   tealR, tealG, tealB,

        // middle stripe (gold)
        left, hMid2, 0.0f,   goldR, goldG, goldB,
        right,hMid2, 0.0f,   goldR, goldG, goldB,
        left, hMid1, 0.0f,   goldR, goldG, goldB,

        right,hMid1, 0.0f,   goldR, goldG, goldB,
        left, hMid1, 0.0f,   goldR, goldG, goldB,
        right,hMid2, 0.0f,   goldR, goldG, goldB,

        // bottom stripe (teal)
        left, hBot,  0.0f,   tealR, tealG, tealB,
        right,hBot,  0.0f,   tealR, tealG, tealB,
        left, hMid2, 0.0f,   tealR, tealG, tealB,

        right,hMid2, 0.0f,   tealR, tealG, tealB,
        left, hMid2, 0.0f,   tealR, tealG, tealB,
        right,hBot,  0.0f,   tealR, tealG, tealB,
    };

    // load rectangle stripes
    int vcount = sizeof(verts)/sizeof(float);
    mesh.vertices.assign(verts, verts + vcount);

    // add black hoist triangle (as two triangles forming a triangle fan shape)
    // triangle vertices (left-most)
    float tri[] = {
        left, hBot, 0.0f,  blackR, blackG, blackB,
        left, hTop, 0.0f,  blackR, blackG, blackB,
        left+0.45f, 0.0f, 0.0f,  blackR, blackG, blackB,
    };
    int triStart = mesh.vertices.size();
    mesh.vertices.insert(mesh.vertices.end(), tri, tri + sizeof(tri)/sizeof(float));

    // indices sequential (we use glDrawElements with the same order)
    unsigned int totalVerts = (unsigned int)(mesh.vertices.size()/6);
    for(unsigned int i=0;i<totalVerts;i++) mesh.indices.push_back(i);

    mesh.upload();
}

void BahamasFlag::draw() {
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
