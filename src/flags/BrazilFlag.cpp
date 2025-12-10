// src/flags/BrazilFlag.cpp
// تعريف علم البرازيل باستخدام OpenGL + Mesh

#include "../../include/flags/BrazilFlag.hpp"
#include <glad/glad.h>
#include <cmath>
#include <iostream>

using namespace std;

BrazilFlag::BrazilFlag()
    : Flag("Brazil")
{
}

/*
    init:
        - 1) مستطيل أخضر كامل (خلفية)
        - 2) ماسة صفراء (rhombus) مركزية
        - 3) دائرة زرقاء مركزية (تقريب بدائرة مكونة من مثلثات)
        - 4) شريط أبيض تقريبي (مستطيل مائل) على الدائرة
        - 5) نجوم مبسطة (ثلاث نقاط مثلثية) داخل الدائرة (تأثير تجميلي)
*/
void BrazilFlag::init() {
    mesh.vertices.clear();
    mesh.indices.clear();

    // base extents
    float l = -0.9f, r = 0.9f, t = 0.7f, b = -0.7f;

    // colors
    float greenR = 0.0f, greenG = 0.45f, greenB = 0.2f;
    float yellowR = 1.0f, yellowG = 0.85f, yellowB = 0.0f;
    float blueR = 0.06f, blueG = 0.39f, blueB = 0.64f;
    float whiteR = 1.0f, whiteG = 1.0f, whiteB = 1.0f;
    float starR = 1.0f, starG = 1.0f, starB = 1.0f;

    // 1) green background (two triangles)
    float verts_bg[] = {
        l, b, 0.0f,   greenR, greenG, greenB,
        r, b, 0.0f,   greenR, greenG, greenB,
        l, t, 0.0f,   greenR, greenG, greenB,

        r, t, 0.0f,   greenR, greenG, greenB,
        l, t, 0.0f,   greenR, greenG, greenB,
        r, b, 0.0f,   greenR, greenG, greenB,
    };
    mesh.vertices.assign(verts_bg, verts_bg + sizeof(verts_bg)/sizeof(float));

    // 2) yellow rhombus (diamond) centered
    float cx = 0.0f, cy = 0.0f;
    float rx = 0.5f, ry = 0.35f;
    // diamond as two triangles (but better to do two triangles twice to form full diamond)
    float diamond[] = {
        cx - rx, cy, 0.0f,   yellowR, yellowG, yellowB, // left
        cx + rx, cy, 0.0f,   yellowR, yellowG, yellowB, // right
        cx,      cy + ry,0.0f, yellowR, yellowG, yellowB, // top

        cx + rx, cy, 0.0f,   yellowR, yellowG, yellowB, // right
        cx,      cy - ry,0.0f, yellowR, yellowG, yellowB, // bottom
        cx,      cy + ry,0.0f, yellowR, yellowG, yellowB, // top
    };
    mesh.vertices.insert(mesh.vertices.end(), diamond, diamond + sizeof(diamond)/sizeof(float));

    // 3) blue circle (approx using triangle fan)
    const int circleSegs = 48;
    vector<float> circleVerts;
    // center
    circleVerts.push_back(cx); circleVerts.push_back(cy); circleVerts.push_back(0.01f);
    circleVerts.push_back(blueR); circleVerts.push_back(blueG); circleVerts.push_back(blueB);
    float radius = 0.25f;
    for(int i=0;i<=circleSegs;i++){
        float a = (float)i / circleSegs * 2.0f * 3.14159265f;
        float x = cx + cosf(a) * radius;
        float y = cy + sinf(a) * radius;
        circleVerts.push_back(x); circleVerts.push_back(y); circleVerts.push_back(0.01f);
        circleVerts.push_back(blueR); circleVerts.push_back(blueG); circleVerts.push_back(blueB);
    }
    mesh.vertices.insert(mesh.vertices.end(), circleVerts.begin(), circleVerts.end());

    // 4) white band (approximated by a wide quad rotated slightly)
    // we will add a rectangle centered at cy + small offset and rotated by -15 degrees
    float bandW = 0.9f, bandH = 0.08f;
    float angle = -15.0f * 3.14159265f / 180.0f;
    float ca = cosf(angle), sa = sinf(angle);
    float bw = bandW*0.5f, bh = bandH*0.5f;
    // local corners of axis-aligned rect then transform
    float local[4][2] = {{-bw,-bh},{bw,-bh},{bw,bh},{-bw,bh}};
    float bandVerts[24]; int bi=0;
    for(int i=0;i<4;i++){
        float lx = local[i][0], ly = local[i][1] + 0.06f; // small vertical shift
        float tx = lx*ca - ly*sa + cx;
        float ty = lx*sa + ly*ca + cy;
        bandVerts[bi++] = tx; bandVerts[bi++] = ty; bandVerts[bi++] = 0.02f;
        bandVerts[bi++] = whiteR; bandVerts[bi++] = whiteG; bandVerts[bi++] = whiteB;
    }
    // two triangles for band
    mesh.vertices.insert(mesh.vertices.end(), bandVerts, bandVerts + 24);

    // 5) small stars (very simplified): three white small triangles inside circle
    auto addStarTri = [&](float sx, float sy, float s) {
        float tri[18] = {
            sx, sy - s, 0.03f,  starR, starG, starB,
            sx - s, sy + s, 0.03f, starR, starG, starB,
            sx + s, sy + s, 0.03f, starR, starG, starB
        };
        mesh.vertices.insert(mesh.vertices.end(), tri, tri + 18);
    };
    addStarTri(-0.05f, 0.05f, 0.02f);
    addStarTri(0.12f, 0.0f, 0.02f);
    addStarTri(-0.12f, -0.05f, 0.02f);

    // build indices sequentially
    unsigned int totalVerts = (unsigned int)(mesh.vertices.size()/6);
    for(unsigned int i=0;i<totalVerts;i++) mesh.indices.push_back(i);

    mesh.upload();
}

void BrazilFlag::draw() {
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
