// main.cpp
// Pixel Pioneers Team - OpenGL + ImGui flags viewer
// تصميم OOP لعرض مجموعة أعلام بشكل تفاعلي
// Landing Page: خلفية متحركة مستقبلية باستخدام Fragment Shader

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <cmath>  // للـ sin / cos في الـ C++

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Dear ImGui (مكتبة الواجهة الرسومية)
#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_glfw.h"
#include "../imgui/backends/imgui_impl_opengl3.h"

// هيدرز المشروع
#include "FlagsCore.hpp"
#include "EgyptEagleFlag.hpp"

using namespace std;

// مقاس النافذة الافتراضي
const int DWIDTH  = 1000;
const int DHEIGHT = 700;

// ====================== 1) Shaders ======================
// Shader الخاص بالأعلام (موجة بسيطة علشان العلم يتحرك)

const char* flag_vertex_src = R"(
#version 330 core
layout(location = 0) in vec3 aPos;    // مكان النقطة
layout(location = 1) in vec3 aColor;  // لون النقطة
out vec3 vColor;

uniform float uTime;   // وقت - نستخدمه في الأنيميشن
uniform float uWidth;  // نصف عرض العلم (لتحكم في شدة الموجة)

void main() {
    // عمل موجة بسيطة علي محور X حسب مكان Y والزمن
    float freq = 10.0;
    float amp  = 0.03;
    float attenuation = 1.0 - abs(aPos.x)/uWidth;  // تقل كل ما نقرب من طرف العلم
    float wave = sin(aPos.y*freq + uTime*3.0) * amp * attenuation;

    vec3 pos = aPos;
    pos.x += wave;

    gl_Position = vec4(pos, 1.0);
    vColor = aColor;
}
)";

const char* flag_frag_src = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main() {
    // اللون النهائي لكل pixel
    FragColor = vec4(vColor, 1.0);
}
)";

// Shader خاص بالـ Landing Page (خلفية متحركة ملونة)
const char* landing_vertex_src = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
out vec2 vUv;

void main() {
    // نحول من [-1,1] الي [0,1]
    vUv = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* landing_frag_src = R"(
#version 330 core
out vec4 FragColor;
in vec2 vUv;
uniform float uTime;

void main(){
    float t = uTime * 0.2;
    vec2 uv = vUv;
    vec2 p = uv - 0.5;
    float len = length(p);

    // تدرج لوني أساسي
    vec3 col = mix(vec3(0.02,0.04,0.10),
                   vec3(0.10,0.05,0.25),
                   uv.y + 0.2*sin(t*1.3));

    // دوائر لونية متحركة (blobs)
    float b1 = 0.15 / (0.001 + length(uv - vec2(0.3+0.1*sin(t*1.7), 0.5+0.12*cos(t*1.3))));
    float b2 = 0.12 / (0.001 + length(uv - vec2(0.7+0.12*cos(t*1.1), 0.45+0.1*sin(t*1.5))));
    col += vec3(0.9,0.4,0.6) * b1;
    col += vec3(0.2,0.6,1.0) * b2 * 0.8;

    // دوائر خفيفة حوالين المركز
    col += 0.02 * sin(20.0*(len - 0.1*sin(t*0.7)));

    // تصحيح إضاءة بسيط
    col = pow(col, vec3(0.8));
    FragColor = vec4(col, 1.0);
}
)";

// ====================== 2) Helper لتجميع الـ Shader ======================
GLuint compileShader(GLenum type, const char* src){
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);

    GLint ok;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if(!ok){
        char buf[1024];
        glGetShaderInfoLog(sh, 1024, nullptr, buf);
        cerr << "Shader compile error: " << buf << endl;
    }
    return sh;
}

// ====================== 3) أعلام إضافية (Stripe / Circle / Pirates) ======================

// --------- StripeFlag: أعلام أفقية بأشرطة ملونة (زي جابون / فرنسا) ---------
class StripeFlag : public Flag {
public:
    vector<array<float,3>> stripes; // من فوق لتحت
    float halfW, halfH;
    int cols, rows;

    StripeFlag(const string &n, const vector<array<float,3>>& s,
               float w=0.6f, float h=0.4f, int c=60, int r=30)
    : Flag(n), stripes(s), halfW(w), halfH(h), cols(c), rows(r) {}

    void init() override {
        mesh.vertices.clear();
        mesh.indices.clear();

        // تقسيم العلم ل grid (rows x cols) و تحديد اللون حسب stripe
        for(int r=0; r<=rows; ++r){
            float ty = (float)r / rows;
            float y  = halfH - ty*(2.0f*halfH);
            for(int c=0; c<=cols; ++c){
                float tx = (float)c / cols;
                float x  = -halfW + tx*(2.0f*halfW);

                int idx = (int)(ty * stripes.size());
                if(idx < 0) idx = 0;
                if(idx >= (int)stripes.size()) idx = (int)stripes.size()-1;
                const auto &col = stripes[idx];

                mesh.vertices.push_back(x);
                mesh.vertices.push_back(y);
                mesh.vertices.push_back(0.0f);
                mesh.vertices.push_back(col[0]);
                mesh.vertices.push_back(col[1]);
                mesh.vertices.push_back(col[2]);
            }
        }

        // بناء الـ indices (كل مربع = مثلثين)
        for(int r=0;r<rows;++r){
            for(int c=0;c<cols;++c){
                unsigned int a = r*(cols+1) + c;
                unsigned int b = a+1;
                unsigned int cidx = a + (cols+1);
                unsigned int d = cidx + 1;
                mesh.indices.push_back(a); mesh.indices.push_back(cidx); mesh.indices.push_back(b);
                mesh.indices.push_back(b); mesh.indices.push_back(cidx); mesh.indices.push_back(d);
            }
        }

        mesh.upload(false);
    }
};

// --------- CircleFlag: زي اليابان (خلفية + دائرة) ---------
class CircleFlag : public Flag {
public:
    array<float,3> bg;   // لون الخلفية

    CircleFlag(const string &n, const array<float,3>& background)
    : Flag(n), bg(background) {}

    void init() override {
        // مستطيل بسيط يمثل العلم
        mesh.vertices = {
            -0.6f, -0.4f, 0.0f, bg[0], bg[1], bg[2],
             0.6f, -0.4f, 0.0f, bg[0], bg[1], bg[2],
             0.6f,  0.4f, 0.0f, bg[0], bg[1], bg[2],
            -0.6f,  0.4f, 0.0f, bg[0], bg[1], bg[2]
        };
        mesh.indices = {0,1,2, 2,3,0};
        mesh.upload(false);
    }

    void draw(GLuint shaderProgram, float time) override {
        // 1) نرسم الخلفية
        Flag::draw(shaderProgram, time);

        // 2) نرسم دائرة حمراء في النص (الشمس) كـ mesh مؤقت
        const int segments = 80;
        vector<float> verts;
        vector<unsigned int> idx;

        // مركز الدائرة
        verts.push_back(0.0f);
        verts.push_back(0.0f);
        verts.push_back(0.01f);
        verts.push_back(0.85f);
        verts.push_back(0.1f);
        verts.push_back(0.15f);

        float radius = 0.18f;
        for(int s=0; s<=segments; ++s){
            float a = (float)s / segments * 2.0f * 3.1415926f;
            float x = std::cos(a) * radius;
            float y = std::sin(a) * radius;
            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(0.01f);
            verts.push_back(0.85f);
            verts.push_back(0.1f);
            verts.push_back(0.15f);
        }

        for(int i=1;i<=segments;i++){
            idx.push_back(0);
            idx.push_back(i);
            idx.push_back(i+1);
        }

        GLuint VAO,VBO,EBO;
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glGenBuffers(1,&EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);

        GLsizei stride = 6*sizeof(float);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,stride,(void*)0);                 glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,stride,(void*)(3*sizeof(float))); glEnableVertexAttribArray(1);

        glUseProgram(shaderProgram);
        GLint timeLoc = glGetUniformLocation(shaderProgram, "uTime");
        if(timeLoc>=0) glUniform1f(timeLoc, time);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)idx.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDeleteBuffers(1,&VBO);
        glDeleteBuffers(1,&EBO);
        glDeleteVertexArrays(1,&VAO);
    }
};

// --------- PirateFlag: علم القراصنة (Skull & Crossbones) ---------
class PirateFlag : public Flag {
public:
    PirateFlag(const string &n) : Flag(n) {}

    void init() override {
        mesh.vertices.clear();
        mesh.indices.clear();

        vector<float> verts;

        auto addTri = [&](float x1,float y1,
                          float x2,float y2,
                          float x3,float y3,
                          float r,float g,float b){
            // vertex 1
            verts.push_back(x1); verts.push_back(y1); verts.push_back(0.0f);
            verts.push_back(r);  verts.push_back(g);  verts.push_back(b);
            // vertex 2
            verts.push_back(x2); verts.push_back(y2); verts.push_back(0.0f);
            verts.push_back(r);  verts.push_back(g);  verts.push_back(b);
            // vertex 3
            verts.push_back(x3); verts.push_back(y3); verts.push_back(0.0f);
            verts.push_back(r);  verts.push_back(g);  verts.push_back(b);
        };

        // ألوان
        const float BR = 0.0f, BG = 0.0f, BB = 0.0f; // أسود
        const float WR = 1.0f, WG = 1.0f, WB = 1.0f; // أبيض

        // 1) خلفية العلم (مستطيل اسود يغطي العلم)
        float left   = -0.8f;
        float right  =  0.8f;
        float top    =  0.75f;
        float bottom = -0.75f;

        addTri(left, bottom, right, bottom, left, top, BR,BG,BB);
        addTri(right,bottom, right, top,    left, top, BR,BG,BB);

        // 2) رأس الجمجمة (مستطيل أبيض)
        float hxL = -0.20f, hxR = 0.20f, hyT = 0.35f, hyB = 0.05f;
        addTri(hxL, hyB, hxR, hyB, hxL, hyT, WR,WG,WB);
        addTri(hxR, hyB, hxR, hyT, hxL, hyT, WR,WG,WB);

        // 3) الفك (مستطيل أبيض أصغر تحت الرأس)
        float jxL = -0.16f, jxR = 0.16f, jyT = 0.05f, jyB = -0.10f;
        addTri(jxL, jyB, jxR, jyB, jxL, jyT, WR,WG,WB);
        addTri(jxR, jyB, jxR, jyT, jxL, jyT, WR,WG,WB);

        // 4) العين اليسرى (مستطيل أسود صغير)
        float exL1 = -0.125f, exR1 = -0.055f, eyB1 = 0.20f, eyT1 = 0.26f;
        addTri(exL1, eyB1, exR1, eyB1, exL1, eyT1, BR,BG,BB);
        addTri(exR1, eyB1, exR1, eyT1, exL1, eyT1, BR,BG,BB);

        // 5) العين اليمنى
        float exL2 = 0.055f, exR2 = 0.125f, eyB2 = 0.20f, eyT2 = 0.26f;
        addTri(exL2, eyB2, exR2, eyB2, exL2, eyT2, BR,BG,BB);
        addTri(exR2, eyB2, exR2, eyT2, exL2, eyT2, BR,BG,BB);

        // 6) الأنف (مثلث اسود)
        addTri(0.0f, 0.11f, -0.03f, 0.18f, 0.03f, 0.18f, BR,BG,BB);

        // 7) عظمة مائلة من اليسار تحت لليمين فوق
        addTri(-0.60f, -0.45f, -0.45f, -0.60f, 0.60f, 0.45f, WR,WG,WB);
        addTri(-0.45f, -0.60f, 0.60f,  0.45f, 0.45f, 0.60f, WR,WG,WB);

        // 8) عظمة مائلة عكسية من اليسار فوق لليمين تحت
        addTri(-0.60f, 0.45f, -0.45f, 0.60f, 0.60f, -0.45f, WR,WG,WB);
        addTri(-0.45f, 0.60f, 0.60f, -0.45f, 0.45f, -0.60f, WR,WG,WB);

        // تحويل الـ verts الي vertices و بناء indices متسلسلة
        mesh.vertices = verts;
        size_t vCount = verts.size() / 6;   // كل vertex = 6 float
        mesh.indices.resize(vCount);
        for(size_t i=0;i<vCount;++i) mesh.indices[i] = (unsigned int)i;

        mesh.upload(false);
    }
};

// ====================== 4) Landing Quad ======================
Mesh createLandingQuad(){
    Mesh m;
    // مستطيل يغطي الشاشة كلها (NDC)
    m.vertices = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };
    m.indices = {0,1,2, 2,3,0};
    m.upload(true);
    m.prim = GL_TRIANGLES;
    return m;
}

// ====================== 5) MAIN ======================
int main(){
    // 1) تهيئة GLFW و إنشاء النافذة
    if(!glfwInit()){
        cerr<<"GLFW init failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(DWIDTH, DHEIGHT,
        "Pixel Pioneers Team - Flags", nullptr, nullptr);
    if(!window){
        cerr<<"Window create failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // 2) تحميل دوال OpenGL عن طريق GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cerr << "GLAD load failed\n";
        return -1;
    }
    glViewport(0,0,DWIDTH,DHEIGHT);

    // 3) تهيئة ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 4) تجميع ال Shaders
    GLuint flag_v   = compileShader(GL_VERTEX_SHADER,   flag_vertex_src);
    GLuint flag_f   = compileShader(GL_FRAGMENT_SHADER, flag_frag_src);
    GLuint flag_prog = glCreateProgram();
    glAttachShader(flag_prog, flag_v);
    glAttachShader(flag_prog, flag_f);
    glLinkProgram(flag_prog);
    glDeleteShader(flag_v);
    glDeleteShader(flag_f);

    GLuint land_v   = compileShader(GL_VERTEX_SHADER,   landing_vertex_src);
    GLuint land_f   = compileShader(GL_FRAGMENT_SHADER, landing_frag_src);
    GLuint land_prog = glCreateProgram();
    glAttachShader(land_prog, land_v);
    glAttachShader(land_prog, land_f);
    glLinkProgram(land_prog);
    glDeleteShader(land_v);
    glDeleteShader(land_f);

    // 5) إنشاء Mesh خاص بالـ Landing Page
    Mesh landingMesh = createLandingQuad();

    // 6) تسجيل الأعلام في FlagManager
    FlagManager fm;

    // Egypt: علم مصر بالنسر (من الهيدر الخارجي)
    fm.registerFlag(new EgyptEagleFlag("Egypt"));

    // Pirates: الجمجمة و العظام
    fm.registerFlag(new PirateFlag("Pirates"));

    // Gabon
    vector<array<float,3>> gabon = {
        {0.0f, 0.6f, 0.3f},
        {1.0f, 0.8f, 0.0f},
        {0.0f, 0.3f, 0.7f}
    };
    fm.registerFlag(new StripeFlag("Gabon", gabon));

    // France (placeholder horizontal stripes)
    vector<array<float,3>> france_h = {
        {0.0f, 0.0f, 0.6f},
        {1.0f, 1.0f, 1.0f},
        {0.8f, 0.0f, 0.0f}
    };
    fm.registerFlag(new StripeFlag("France", france_h));

    // Japan
    fm.registerFlag(new CircleFlag("Japan", {1.0f,1.0f,1.0f}));

    // 7) المتغير المسئول عن الاختيار الحالي من المنيو
    string selected = "Landing";

    // ترتيب العناصر في القائمة
    vector<string> menuItems = {
        "Egypt",
        "Pirates",
        "Gabon",
        "France",
        "Japan",
        "Mother of world",
        "Sopngbob",
        "isreal boom"
    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float startTime = (float)glfwGetTime();

    // تعديل بسيط في شكل ImGui (زوايا ناعمة)
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding  = 6.0f;

    // ====================== LOOP الرئيسي ======================
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        // بدء Frame جديد لـ ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --------- TopBar: عنوان Pixel Pioneers بشكل عريض وجذاب ---------
        ImGui::SetNextWindowPos(ImVec2(0,0));
        ImGui::SetNextWindowSize(ImVec2(DWIDTH,90));
        ImGuiWindowFlags topFlags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGui::Begin("TopBar", nullptr, topFlags);

        float t = (float)glfwGetTime();

        // خلفية متدرجة ديناميكية
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 p0 = ImGui::GetWindowPos();
        ImVec2 p1 = ImVec2(p0.x + DWIDTH, p0.y + 90.0f);
        ImU32 colL = ImColor(0.04f, 0.04f, 0.10f, 1.0f);
        ImU32 colR = ImColor(0.18f + 0.10f*std::sin(t*0.6f), 0.35f, 0.75f, 1.0f);
        draw_list->AddRectFilledMultiColor(p0, p1, colL, colR, colR, colL);

        // خط رفيع لامع أسفل التوب بار
        draw_list->AddRectFilled(
            ImVec2(p0.x, p1.y-2.0f),
            ImVec2(p1.x, p1.y),
            ImColor(0.9f, 0.8f, 0.3f, 0.9f)
        );

        // نكبّر الفونت لعنوان التيم
        ImGui::SetWindowFontScale(1.9f);

        const char* part1 = "Pixel";
        const char* part2 = " Pioneers ";
        const char* part3 = "Team";

        ImVec4 c1(1.0f, 0.55f + 0.15f*std::sin(t*1.3f), 0.25f, 1.0f);
        ImVec4 c2(1.0f, 1.0f,  1.0f, 1.0f);
        ImVec4 c3(0.35f,0.90f, 1.0f, 1.0f);

        // حساب عرض العنوان علشان يكون في النص
        ImVec2 size1 = ImGui::CalcTextSize(part1);
        ImVec2 size2 = ImGui::CalcTextSize(part2);
        ImVec2 size3 = ImGui::CalcTextSize(part3);
        float totalWidth = size1.x + size2.x + size3.x;
        float startX = (DWIDTH - totalWidth) * 0.5f;

        // موضع السطر الأول (العنوان الرئيسي)
        ImGui::SetCursorPosY(18);
        ImGui::SetCursorPosX(startX);

        // النص الملون
        ImGui::TextColored(c1, "%s", part1);  ImGui::SameLine(0,0);
        ImGui::TextColored(c2, "%s", part2);  ImGui::SameLine(0,0);
        ImGui::TextColored(c3, "%s", part3);

        // سطر فرعي صغير تحت العنوان (Tagline)
        ImGui::SetWindowFontScale(1.0f);
        ImGui::SetCursorPosY(52);
        ImGui::SetCursorPosX(DWIDTH*0.5f - 190);
        ImGui::TextColored(
            ImVec4(0.85f,0.85f,0.95f,0.9f),
            "Interactive Flags Viewer · OpenGL + ImGui · OOP Design"
        );

        ImGui::End();

        // --------- القائمة الجانبية (Menu) ---------
        ImGui::SetNextWindowPos(ImVec2(10,74));
        ImGui::SetNextWindowSize(ImVec2(260,DHEIGHT-84));
        ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("اختر Mode / علم:");
        ImGui::Separator();

        for(auto &it : menuItems){
            if(ImGui::Selectable(it.c_str(), selected==it))
                selected = it;
        }

        ImGui::Separator();
        if(ImGui::Button("Back to Landing", ImVec2(-1,0)))
            selected = "Landing";
        ImGui::End();

        // --------- لوحة المعلومات (Info Panel) ---------
        ImGui::SetNextWindowPos(ImVec2(DWIDTH-280,74));
        ImGui::SetNextWindowSize(ImVec2(270,220));
        ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("Selected: %s", selected.c_str());
        ImGui::Separator();

        if(selected=="Landing"){
            ImGui::TextWrapped("Welcome to Pixel Pioneers Team\nLanding Page ملونة متحركة بالـ Fragment Shader.");
        } else if(selected=="Egypt"){
            ImGui::TextWrapped("Egypt Flag with Eagle:\nDrawn fully using OpenGL geometry (no textures).");
        } else if(selected=="Pirates"){
            ImGui::TextWrapped("Pirate Flag:\nBlack background with skull & crossbones.\nDesigned manually using triangles.");
        } else {
            Flag* f = fm.get(selected);
            if(f){
                ImGui::TextWrapped("Showing flag: %s", selected.c_str());
            } else {
                ImGui::TextWrapped("Mode: %s\n(Placeholder / Coming soon)", selected.c_str());
            }
        }
        ImGui::End();

        // تجهيز ImGui للرسم
        ImGui::Render();

        // --------- رسم الخلفية / العلم ---------
        glClearColor(0.02f,0.02f,0.03f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float now = (float)glfwGetTime();
        float dt  = now - startTime;

        if(selected == "Landing"){
            // نرسم الـ Landing Page بالـ shader الخاص بها
            glUseProgram(land_prog);
            GLint tloc = glGetUniformLocation(land_prog, "uTime");
            if(tloc>=0) glUniform1f(tloc, dt);

            glBindVertexArray(landingMesh.VAO);
            glDrawElements(GL_TRIANGLES, landingMesh.drawCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        } else {
            // نرسم العلم المختار لو موجود
            Flag* f = fm.get(selected);
            if(f){
                glUseProgram(flag_prog);
                GLint timeLoc  = glGetUniformLocation(flag_prog, "uTime");
                GLint widthLoc = glGetUniformLocation(flag_prog, "uWidth");
                if(timeLoc>=0)  glUniform1f(timeLoc, dt);
                if(widthLoc>=0) glUniform1f(widthLoc, 0.6f);
                f->draw(flag_prog, dt);
            }
        }

        // رسم ImGui فوق الرسمة
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    fm.cleanup();
    landingMesh.cleanup();
    glDeleteProgram(flag_prog);
    glDeleteProgram(land_prog);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
