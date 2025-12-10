#pragma once

#include "FlagsCore.hpp"

// --------- EgyptEagleFlag: علم مصر بالنسر مرسوم بالـ OpenGL ---------
class EgyptEagleFlag : public Flag {
public:
    EgyptEagleFlag(const std::string &n) : Flag(n) {}

    void init() override {
        mesh.vertices.clear();
        mesh.indices.clear();

        std::vector<float> verts;

        auto addTri = [&](float x1,float y1,
                          float x2,float y2,
                          float x3,float y3,
                          float r,float g,float b){
            // v1
            verts.push_back(x1); verts.push_back(y1); verts.push_back(0.0f);
            verts.push_back(r);  verts.push_back(g);  verts.push_back(b);
            // v2
            verts.push_back(x2); verts.push_back(y2); verts.push_back(0.0f);
            verts.push_back(r);  verts.push_back(g);  verts.push_back(b);
            // v3
            verts.push_back(x3); verts.push_back(y3); verts.push_back(0.0f);
            verts.push_back(r);  verts.push_back(g);  verts.push_back(b);
        };

        auto addRect = [&](float lx,float by,float rx,float ty,
                           float r,float g,float b){
            // 2 مثلثات = مستطيل
            addTri(lx,by, rx,by, lx,ty, r,g,b);
            addTri(rx,by, rx,ty, lx,ty, r,g,b);
        };

        // ألوان الأشرطة
        const float RR = 0.70f, RG = 0.0f, RB = 0.0f;  // أحمر
        const float WR = 1.0f,  WG = 1.0f, WB = 1.0f;  // أبيض
        const float BR = 0.0f,  BG = 0.0f, BB = 0.0f;  // أسود

        float left   = -0.8f;
        float right  =  0.8f;
        float top    =  0.55f;
        float midTop =  0.15f;
        float midBot = -0.15f;
        float bottom = -0.55f;

        // 1) الخلفية: ثلاث شرائط أفقية
        addRect(left,  midTop, right, top,    RR,RG,RB); // أحمر
        addRect(left,  midBot, right, midTop, WR,WG,WB); // أبيض
        addRect(left,  bottom, right, midBot, BR,BG,BB); // أسود

        // 2) النسر الذهبي في المنتصف
        const float GR  = 0.95f, GG  = 0.85f, GB  = 0.25f;
        const float GR2 = 0.92f, GG2 = 0.80f, GB2 = 0.20f;

        // جسد النسر
        float bxL = -0.12f, bxR = 0.12f, byT = 0.16f, byB = -0.06f;
        addRect(bxL, byB, bxR, byT, GR,GG,GB);

        // ريش الصدر (3 شرائط)
        float stripeW = 0.02f;
        addRect(bxL + stripeW*0.5f,  byB, bxL + stripeW*1.5f, byT, GR2,GG2,GB2);
        addRect(-stripeW*0.5f,       byB, stripeW*0.5f,       byT, GR2,GG2,GB2);
        addRect(bxR - stripeW*1.5f,  byB, bxR - stripeW*0.5f, byT, GR2,GG2,GB2);

        // الجناح الأيسر
        float wlYTop = 0.18f, wlYBot = -0.04f;
        addTri(bxL, wlYTop,  bxL-0.26f, wlYTop-0.03f,  bxL, wlYBot, GR,GG,GB);
        addTri(bxL, wlYBot,  bxL-0.26f, wlYTop-0.03f, bxL-0.22f, wlYBot-0.06f, GR,GG,GB);

        // الجناح الأيمن
        float wrYTop = wlYTop, wrYBot = wlYBot;
        addTri(bxR, wrYTop,  bxR+0.26f, wrYTop-0.03f,  bxR, wrYBot, GR,GG,GB);
        addTri(bxR, wrYBot,  bxR+0.26f, wrYTop-0.03f, bxR+0.22f, wrYBot-0.06f, GR,GG,GB);

        // الذيل
        float txL = -0.09f, txR = 0.09f, tyT = byB-0.02f, tyB = byB-0.16f;
        addRect(txL, tyB, txR, tyT, GR,GG,GB);

        float seg = (txR - txL) / 3.0f;
        addRect(txL,          tyB, txL+seg*0.8f, tyT, GR2,GG2,GB2);
        addRect(txL+seg*1.1f, tyB, txL+seg*1.9f, tyT, GR2,GG2,GB2);
        addRect(txL+seg*2.2f, tyB, txR,         tyT, GR2,GG2,GB2);

        // الرقبة
        float nxL = -0.03f, nxR = 0.03f, nyT = 0.28f, nyB = 0.16f;
        addRect(nxL, nyB, nxR, nyT, GR,GG,GB);

        // الرأس
        float hxL = 0.00f, hxR = 0.09f, hyT = 0.33f, hyB = 0.26f;
        addRect(hxL, hyB, hxR, hyT, GR,GG,GB);

        // عين
        float exL = 0.06f, exR = 0.075f, eyB = 0.30f, eyT = 0.315f;
        addRect(exL, eyB, exR, eyT, 0.15f,0.12f,0.05f);

        // منقار
        addTri(hxR, (hyT+hyB)*0.5f,
               hxR+0.06f, hyB+0.01f,
               hxR, hyB, GR,GG,GB);

        // درع/قاعدة تحت الجسم
        float sxL = -0.07f, sxR = 0.07f, syT = -0.02f, syB = -0.14f;
        addRect(sxL, syB, sxR, syT, GR2,GG2,GB2);
        addTri(sxL, syB, sxR, syB, 0.0f, syB-0.06f, GR2,GG2,GB2);

        // ظل بسيط عند اتصال الأجنحة بالجسد
        addTri(bxL, byT, bxL-0.08f, byT-0.02f, bxL, byT-0.06f, GR2,GG2,GB2);
        addTri(bxR, byT, bxR+0.08f, byT-0.02f, bxR, byT-0.06f, GR2,GG2,GB2);

        // تحويل للـ mesh
        mesh.vertices = verts;
        size_t vCount = verts.size() / 6;
        mesh.indices.resize(vCount);
        for(size_t i=0; i<vCount; ++i)
            mesh.indices[i] = (unsigned int)i;

        mesh.upload(false);
    }
};
