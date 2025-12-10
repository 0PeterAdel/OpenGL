// src/flags/PirateFlag.cpp
// تعريف علم القراصنة  باستخدام OpenGL + Mesh   //  بعد عناء شاق


#include "../../include/flags/PirateFlag.hpp"
#include <glad/glad.h>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

PirateFlag::PirateFlag()
    : Flag("Pirates")
{
}

/*
    Strategy:
      - background rectangle (black)
      - skull:
         * head: circle approx (triangle fan) in white
         * eye sockets: black circles (we cut over by drawing smaller black discs)
         * nose: black triangle
         * jaw: polygon made of triangles
         * teeth: small rectangles/triangles
         * bandana: triangular strips (colored slightly grey/white)
      - crossed cutlasses:
         * approximate blades by thin curved strips (series of quads)
         * handles as small rectangles
      - edges: a few triangular "rips" at right border for tattered effect
*/
void PirateFlag::init() {
    mesh.vertices.clear();
    mesh.indices.clear();

    // colors
    float black[] = {0.01f,0.01f,0.01f};
    float white[] = {1.0f,1.0f,1.0f};
    float grey[]  = {0.9f,0.9f,0.9f};
    float bone[]  = {0.98f,0.97f,0.95f};

    // 1) Background big rectangle (black)
    float left=-0.9f, right=0.9f, top=0.7f, bottom=-0.7f;
    float bg[] = {
        left, bottom, 0.0f,  black[0], black[1], black[2],
        right, bottom,0.0f,  black[0], black[1], black[2],
        left, top,    0.0f,  black[0], black[1], black[2],

        right, top,   0.0f,  black[0], black[1], black[2],
        left, top,    0.0f,  black[0], black[1], black[2],
        right, bottom,0.0f,  black[0], black[1], black[2],
    };
    mesh.vertices.assign(bg, bg + sizeof(bg)/sizeof(float));

    // helper lambdas to add shapes
    // helper struct to add shapes (supports overloading)
    struct PushTriangleHelper {
        std::vector<float>& vertices;

        // 18 args: per-vertex color
        void operator()(float x1,float y1,float z1, float r1,float g1,float b1,
                        float x2,float y2,float z2, float r2,float g2,float b2,
                        float x3,float y3,float z3, float r3,float g3,float b3) {
            vertices.push_back(x1); vertices.push_back(y1); vertices.push_back(z1);
            vertices.push_back(r1); vertices.push_back(g1); vertices.push_back(b1);
            vertices.push_back(x2); vertices.push_back(y2); vertices.push_back(z2);
            vertices.push_back(r2); vertices.push_back(g2); vertices.push_back(b2);
            vertices.push_back(x3); vertices.push_back(y3); vertices.push_back(z3);
            vertices.push_back(r3); vertices.push_back(g3); vertices.push_back(b3);
        }

        // 12 args: uniform color (delegates to 18-arg version)
        // usage: x1,y1,z1, r,g,b, x2,y2,z2, x3,y3,z3
        // Note: The calls in the file seem to be:
        // pushTriangle(v1_x, v1_y, v1_z, r, g, b, 
        //              v2_x, v2_y, v2_z, 
        //              v3_x, v3_y, v3_z);
        void operator()(float x1,float y1,float z1, float r,float g,float b,
                        float x2,float y2,float z2,
                        float x3,float y3,float z3) {
            (*this)(x1,y1,z1, r,g,b,
                    x2,y2,z2, r,g,b,
                    x3,y3,z3, r,g,b);
        }
    };
    PushTriangleHelper pushTriangle{mesh.vertices};

    // 2) Skull head (triangle fan)
    float cx = 0.0f, cy = 0.12f;
    float headR = 0.18f;
    int segs = 48;
    // center of fan (one vertex)
    mesh.vertices.push_back(cx); mesh.vertices.push_back(cy); mesh.vertices.push_back(0.02f);
    mesh.vertices.push_back(white[0]); mesh.vertices.push_back(white[1]); mesh.vertices.push_back(white[2]);
    for(int i=0;i<=segs;i++){
        float a = (float)i/segs * 2.0f * 3.14159265f;
        float x = cx + cosf(a) * headR;
        float y = cy + sinf(a) * headR * 0.95f; // slightly flattened vertically
        mesh.vertices.push_back(x); mesh.vertices.push_back(y); mesh.vertices.push_back(0.02f);
        mesh.vertices.push_back(white[0]); mesh.vertices.push_back(white[1]); mesh.vertices.push_back(white[2]);
    }

    // 3) Eye sockets (draw black discs on top of head) => add small black fan (this will overlay visually)
    auto addDisc = [&](float sx,float sy,float r, int segsLocal, float cr, float cg, float cb){
        // center
        mesh.vertices.push_back(sx); mesh.vertices.push_back(sy); mesh.vertices.push_back(0.03f);
        mesh.vertices.push_back(cr); mesh.vertices.push_back(cg); mesh.vertices.push_back(cb);
        for(int i=0;i<=segsLocal;i++){
            float a = (float)i/segsLocal * 2.0f * 3.14159265f;
            float x = sx + cosf(a) * r;
            float y = sy + sinf(a) * r;
            mesh.vertices.push_back(x); mesh.vertices.push_back(y); mesh.vertices.push_back(0.03f);
            mesh.vertices.push_back(cr); mesh.vertices.push_back(cg); mesh.vertices.push_back(cb);
        }
    };
    addDisc(-0.06f, cy+0.03f, 0.035f, 24, black[0], black[1], black[2]); // left eye hole
    addDisc( 0.06f, cy+0.03f, 0.035f, 24, black[0], black[1], black[2]); // right eye hole

    // 4) nose (small inverted triangle)
    pushTriangle(0.0f, cy-0.03f, 0.02f, white[0],white[1],white[2],
                 -0.02f, cy+0.01f, 0.02f,
                  0.02f, cy+0.01f, 0.02f);

    // 5) jaw (rectangle-ish) + teeth
    // jaw rectangle bottom
    float jawTop = cy - 0.06f;
    float jawBottom = cy - 0.16f;
    float jawL = -0.12f, jawR = 0.12f;
    // two triangles for jaw
    pushTriangle(jawL, jawBottom, 0.02f, white[0],white[1],white[2],
                 jawR, jawBottom, 0.02f,
                 jawL, jawTop, 0.02f);
    pushTriangle(jawR, jawBottom, 0.02f, white[0],white[1],white[2],
                 jawR, jawTop, 0.02f,
                 jawL, jawTop, 0.02f);

    // teeth: small vertical rectangles (approx with triangles)
    float tx = -0.06f;
    for(int i=0;i<5;i++){
        float w = 0.02f;
        float h = 0.03f;
        float x0 = tx + i*0.03f;
        pushTriangle(x0, jawTop, 0.03f, black[0],black[1],black[2],
                    x0+w, jawTop, 0.03f,
                    x0, jawTop+h, 0.03f);
        pushTriangle(x0+w, jawTop, 0.03f, black[0],black[1],black[2],
                    x0+w, jawTop+h, 0.03f,
                    x0, jawTop+h, 0.03f);
    }

    // 6) bandana (a triangular strip above forehead)
    pushTriangle(-0.18f, cy+0.12f, 0.02f, grey[0],grey[1],grey[2],
                 0.18f, cy+0.12f, 0.02f,
                 0.00f, cy+0.06f, 0.02f);
    // knot tail (two small triangles)
    pushTriangle(0.15f, cy+0.10f, 0.02f, grey[0],grey[1],grey[2],
                 0.22f, cy+0.08f, 0.02f,
                 0.18f, cy+0.04f, 0.02f);

    // 7) Crossed cutlasses (approx curved blades using multiple quads)
    // We'll add two blade strips using several small quads to suggest curvature.
    auto addBlade = [&](float cx1,float cy1,float angle,float len,float thickness,float r,float g,float b){
        // create N segments
        int N=8;
        for(int i=0;i<N;i++){
            float t0 = (float)i/(float)N;
            float t1 = (float)(i+1)/(float)N;
            // base along blade centerline
            float x0 = cx1 + cosf(angle) * t0 * len - sinf(angle) * 0.05f * sinf(t0*3.14f);
            float y0 = cy1 + sinf(angle) * t0 * len + cosf(angle) * 0.05f * sinf(t0*3.14f);
            float x1 = cx1 + cosf(angle) * t1 * len - sinf(angle) * 0.05f * sinf(t1*3.14f);
            float y1 = cy1 + sinf(angle) * t1 * len + cosf(angle) * 0.05f * sinf(t1*3.14f);
            // perpendicular
            float nx = -sinf(angle);
            float ny = cosf(angle);
            float w = thickness * (1.0f - t0*0.6f);
            // quad as two triangles
            pushTriangle(x0 + nx*w, y0 + ny*w, 0.015f, r,g,b,
                        x1 + nx*w, y1 + ny*w, 0.015f,
                        x0 - nx*w, y0 - ny*w, 0.015f);
            pushTriangle(x1 + nx*w, y1 + ny*w, 0.015f, r,g,b,
                        x1 - nx*w, y1 - ny*w, 0.015f,
                        x0 - nx*w, y0 - ny*w, 0.015f);
        }
        // handle (small rectangle near start)
        float hx = cx1, hy = cy1;
        pushTriangle(hx - 0.04f, hy - 0.02f, 0.015f, 0.18f,0.12f,0.06f,
                    hx + 0.04f, hy - 0.02f, 0.015f,
                    hx - 0.04f, hy + 0.02f, 0.015f);
        pushTriangle(hx + 0.04f, hy - 0.02f, 0.015f, 0.18f,0.12f,0.06f,
                    hx + 0.04f, hy + 0.02f, 0.015f,
                    hx - 0.04f, hy + 0.02f, 0.015f);
    };
    // add two blades crossing under the jaw
    addBlade(-0.35f, -0.07f, 0.78f, 0.9f, 0.02f, bone[0],bone[1],bone[2]); // left-to-right
    addBlade( 0.35f, -0.07f, -0.78f,0.9f, 0.02f, bone[0],bone[1],bone[2]); // right-to-left

    // 8) tattered right edge: small triangular cuts
    for(int i=0;i<6;i++){
        float y0 = top - i*0.25f;
        float y1 = y0 - 0.12f;
        float x0 = right - 0.02f;
        float x1 = right - 0.14f - 0.03f * (i%2);
        pushTriangle(x0,y0,0.01f, black[0],black[1],black[2],
                    x1,y1,0.01f, black[0],black[1],black[2],
                    x0,y1,0.01f, black[0],black[1],black[2]);
    }

    // build indices sequentially
    unsigned int totalVerts = (unsigned int)(mesh.vertices.size()/6);
    for(unsigned int i=0;i<totalVerts;i++) mesh.indices.push_back(i);

    mesh.upload();
}

void PirateFlag::draw() {
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
