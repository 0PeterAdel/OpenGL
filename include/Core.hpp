#ifndef CORE_HPP
#define CORE_HPP

#include <string>
#include <vector>
#include <map>
#include <glad/glad.h>

struct Mesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO=0, VBO=0, EBO=0;

    void upload(){
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glGenBuffers(1,&EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float),
                     vertices.data(),GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned int),
                     indices.data(),GL_STATIC_DRAW);

        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void cleanup(){
        glDeleteBuffers(1,&VBO);
        glDeleteBuffers(1,&EBO);
        glDeleteVertexArrays(1,&VAO);
    }
};

class Flag {
public:
    std::string name;
    Mesh mesh;

    Flag(const std::string& n):name(n){}
    virtual ~Flag(){}

    virtual void init() = 0;

    virtual void draw(){
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    virtual void cleanup(){ mesh.cleanup(); }
};

class FlagManager {
    std::map<std::string, Flag*> flags;

public:
    void registerFlag(Flag* f){
        flags[f->name] = f;
        f->init();
    }

    Flag* get(const std::string& n){
        if(flags.count(n)) return flags[n];
        return nullptr;
    }

    void cleanup(){
        for(auto &f:flags){
            f.second->cleanup();
            delete f.second;
        }
        flags.clear();
    }
};

#endif
