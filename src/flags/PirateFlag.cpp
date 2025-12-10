#include "../../include/flags/PirateFlag.hpp"

void PirateFlag::init(){
    float w = 0.8f;

    float v[] = {
        -w,-0.6f,0, 0,0,0,  w,-0.6f,0,0,0,0,  -w,0.6f,0,0,0,0,
         w,-0.6f,0, 0,0,0, -w,0.6f,0,0,0,0,   w,0.6f,0,0,0,0,

        // Skull
        -0.1f,0.0f,0.1f, 1,1,1,
         0.1f,0.0f,0.1f, 1,1,1,
         0.0f,0.2f,0.1f, 1,1,1,
    };

    mesh.vertices.assign(v, v+sizeof(v)/sizeof(float));

    for(unsigned int i=0;i<mesh.vertices.size()/6;i++)
        mesh.indices.push_back(i);

    mesh.upload();
}
