#include "../../include/flags/EgyptEagleFlag.hpp"

void EgyptEagleFlag::init(){
    float w = 0.8f;
    float h1 = 0.3f, h2 = 0.0f, h3 = -0.3f;

    // أحمر / أبيض / أسود
    float v[] = {
        // Red
        -w,h1,0, 1,0,0,  w,h1,0,1,0,0,  -w,h2,0,1,0,0,
         w,h1,0,1,0,0,  -w,h2,0,1,0,0,  w,h2,0,1,0,0,

        // White
        -w,h2,0, 1,1,1,  w,h2,0,1,1,1,  -w,h3,0,1,1,1,
         w,h2,0,1,1,1,  -w,h3,0,1,1,1,  w,h3,0,1,1,1,

        // Black
        -w,h3,0, 0,0,0,  w,h3,0,0,0,0,  -w,-0.6f,0,0,0,0,
         w,h3,0,0,0,0,  -w,-0.6f,0,0,0,0,  w,-0.6f,0,0,0,0,

        // === Eagle (Gold Simple Shape) ===
        -0.1f,-0.05f,0.1f, 1,0.8,0.2,
         0.1f,-0.05f,0.1f, 1,0.8,0.2,
         0.0f, 0.15f,0.1f, 1,0.8,0.2,
    };

    mesh.vertices.assign(v, v+sizeof(v)/sizeof(float));

    for(unsigned int i=0;i<mesh.vertices.size()/6;i++)
        mesh.indices.push_back(i);

    mesh.upload();
}
