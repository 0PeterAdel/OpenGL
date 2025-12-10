// src/main.cpp
// Pixel Pioneers Team - Landing + Flags Viewer (OpenGL + ImGui)

#include <iostream>
#include <string>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Core and Flags
#include "../include/Core.hpp"
#include "../include/flags/EgyptEagleFlag.hpp"
#include "../include/flags/PirateFlag.hpp"
#include "../include/flags/GabonFlag.hpp"
#include "../include/flags/JapanFlag.hpp"
#include "../include/flags/AustriaFlag.hpp"
#include "../include/flags/RomaniaFlag.hpp"
#include "../include/flags/UAEFlag.hpp"
#include "../include/flags/GermanyFlag.hpp"
#include "../include/flags/BahamasFlag.hpp"
#include "../include/flags/BrazilFlag.hpp"

#include "../include/flags/ImageEdgeFlag.hpp"


// Dear ImGui
#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_glfw.h"
#include "../imgui/backends/imgui_impl_opengl3.h"

using namespace std;

const int W = 1280;
const int H = 800;

// ---------- helpers: compile / link shaders ----------
GLuint compileShader(GLenum type, const char* src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(sh, 1024, nullptr, log);
        cerr << "Shader compile error:\n" << log << endl;
    }
    return sh;
}

GLuint createProgram(const char* vsSrc, const char* fsSrc) {
    GLuint vs = compileShader(GL_VERTEX_SHADER,   vsSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSrc);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(prog, 1024, nullptr, log);
        cerr << "Program link error:\n" << log << endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// ---------- landing shaders ----------
const char* landing_vs = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
out vec2 vUv;
void main() {
    vUv = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* landing_fs = R"(
#version 330 core
out vec4 FragColor;
in vec2 vUv;
uniform float uTime;

void main(){
    float t = uTime * 0.2;
    vec2 uv = vUv;
    vec2 p = uv - 0.5;
    float len = length(p);

    vec3 col = mix(vec3(0.02,0.04,0.10),
                   vec3(0.10,0.05,0.25),
                   uv.y + 0.2*sin(t*1.3));

    float b1 = 0.15 / (0.001 + length(uv - vec2(0.3+0.1*sin(t*1.7), 0.5+0.12*cos(t*1.3))));
    float b2 = 0.12 / (0.001 + length(uv - vec2(0.7+0.12*cos(t*1.1), 0.45+0.1*sin(t*1.5))));
    col += vec3(0.9,0.4,0.6) * b1;
    col += vec3(0.2,0.6,1.0) * b2 * 0.8;

    col += 0.02 * sin(20.0*(len - 0.1*sin(t*0.7)));

    col = pow(col, vec3(0.8));
    FragColor = vec4(col, 1.0);
}
)";

// ---------- flag shaders (positions + color) ----------
const char* flag_vs = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
out vec3 vColor;
void main() {
    gl_Position = vec4(aPos, 1.0);
    vColor = aColor;
}
)";

const char* flag_fs = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vColor, 1.0);
}
)";



int main() {
    // 1) GLFW + Window
    if (!glfwInit()) {
        cerr << "Failed to init GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_STENCIL_BITS, 8);

    GLFWwindow* window = glfwCreateWindow(W, H, "Pixel Pioneers Team - Flags", nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // 2) GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to init GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, W, H);

    // 3) ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;


    // Load font with Unicode support
    ImFont* mainFont = io.Fonts->AddFontFromFileTTF(
        "fonts/NotoSansArabic-Regular.ttf",   // Font file path
        20.0f,                                // Font size
        nullptr,
        io.Fonts->GetGlyphRangesCyrillic()    // Glyph ranges for Unicode support
    );
    if (!mainFont) {
        std::cerr << "Failed to load Arabic font\n";
    }


    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 4) Shader programs
    GLuint landingProg = createProgram(landing_vs, landing_fs);
    GLuint flagProg    = createProgram(flag_vs,    flag_fs);

    // 5) Landing full-screen quad (VAO/VBO/EBO)
    GLuint landVAO = 0, landVBO = 0, landEBO = 0;
    {
        float quadVerts[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
             1.0f,  1.0f,
            -1.0f,  1.0f
        };
        unsigned int quadIndices[] = { 0,1,2, 2,3,0 };

        glGenVertexArrays(1, &landVAO);
        glGenBuffers(1, &landVBO);
        glGenBuffers(1, &landEBO);

        glBindVertexArray(landVAO);
        glBindBuffer(GL_ARRAY_BUFFER, landVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, landEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    // 6) FlagManager + flags
    FlagManager manager;
    manager.registerFlag(new EgyptEagleFlag());
    manager.registerFlag(new PirateFlag());
    manager.registerFlag(new GabonFlag()); 
    manager.registerFlag(new JapanFlag());
    manager.registerFlag(new AustriaFlag());
    manager.registerFlag(new RomaniaFlag());
    manager.registerFlag(new UAEFlag());
    manager.registerFlag(new GermanyFlag());
    manager.registerFlag(new BahamasFlag());
    manager.registerFlag(new BrazilFlag());

    manager.registerFlag(new ImageEdgeFlag("assets/eg.png"));
    string selected = "Landing";
    vector<string> menuItems = {
        "Landing",
        "Egypt",
        "Pirates",
        "Gabon",
        "Japan",
        "Austria",
        "Romania",
        "UAE",
        "Germany",
        "Bahamas",
        "Brazil",
        "ImageEdges"
    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    float startTime = (float)glfwGetTime();

    // ImGui style tweak
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding  = 6.0f;

    // 7) main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // ---------- ImGui frame ----------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float tNow = (float)glfwGetTime();
        float dt   = tNow - startTime;

        // TopBar: Team name display
        ImGui::SetNextWindowPos(ImVec2(0,0));
        ImGui::SetNextWindowSize(ImVec2(W,80));
        ImGuiWindowFlags topFlags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGui::Begin("TopBar", nullptr, topFlags);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p0 = ImGui::GetWindowPos();
        ImVec2 p1 = ImVec2(p0.x + W, p0.y + 80.0f);

        ImU32 colL = ImColor(0.04f, 0.04f, 0.10f, 1.0f);
        ImU32 colR = ImColor(0.18f + 0.10f*std::sin(dt*0.6f), 0.35f, 0.75f, 1.0f);
        dl->AddRectFilledMultiColor(p0, p1, colL, colR, colR, colL);
        dl->AddRectFilled(
            ImVec2(p0.x, p1.y-2.0f),
            ImVec2(p1.x, p1.y),
            ImColor(0.9f, 0.8f, 0.3f, 0.9f)
        );

        ImGui::SetWindowFontScale(1.9f);

        const char* part1 = "Pixel";
        const char* part2 = " Pioneers ";
        const char* part3 = "Team";

        ImVec4 c1(1.0f, 0.55f + 0.15f*std::sin(dt*1.3f), 0.25f, 1.0f);
        ImVec4 c2(1.0f, 1.0f,  1.0f, 1.0f);
        ImVec4 c3(0.35f,0.90f, 1.0f, 1.0f);

        ImVec2 size1 = ImGui::CalcTextSize(part1);
        ImVec2 size2 = ImGui::CalcTextSize(part2);
        ImVec2 size3 = ImGui::CalcTextSize(part3);
        float totalWidth = size1.x + size2.x + size3.x;
        float startX = (W - totalWidth) * 0.5f;

        ImGui::SetCursorPosY(18);
        ImGui::SetCursorPosX(startX);

        ImGui::TextColored(c1, "%s", part1); ImGui::SameLine(0,0);
        ImGui::TextColored(c2, "%s", part2); ImGui::SameLine(0,0);
        ImGui::TextColored(c3, "%s", part3);

        ImGui::SetWindowFontScale(1.0f);
        ImGui::SetCursorPosY(52);
        ImGui::SetCursorPosX(W*0.5f - 190);
        ImGui::TextColored(
            ImVec4(0.85f,0.85f,0.95f,0.9f),
            "Interactive Flags Viewer · OpenGL + ImGui · OOP Design"
        );
        ImGui::End();

        // Sidebar menu
        ImGui::SetNextWindowPos(ImVec2(10,84));
        ImGui::SetNextWindowSize(ImVec2(260, H-94));
        ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("Select Mode / Flag:");
        ImGui::Separator();
        for (auto& name : menuItems) {
            if (ImGui::Selectable(name.c_str(), selected == name)) {
                selected = name;
            }
        }
        ImGui::End();

        // Info panel
        ImGui::SetNextWindowPos(ImVec2(W-280,84));
        ImGui::SetNextWindowSize(ImVec2(270, 220));
        ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("Selected: %s", selected.c_str());
        ImGui::Separator();
        if (selected == "Landing") {
            ImGui::TextWrapped(
                "Welcome to Pixel Pioneers Team\n"
                "Colorful animated Landing Page using Fragment Shader."
            );
        } else if (selected == "Egypt") {
            ImGui::TextWrapped("Egypt Flag with Eagle emblem (OpenGL polygons).");
        } else if (selected == "Pirates") {
            ImGui::TextWrapped("Pirate Flag: skull & crossbones drawn with triangles.");
        } else if (selected == "Gabon") {
            ImGui::TextWrapped("Gabon Flag: three horizontal stripes (Green, Yellow, Blue) drawn with OpenGL.");
        }
        ImGui::End();

        // ---------- Render OpenGL scene ----------
        // ---------- Render OpenGL scene ----------
        ImGui::Render();

        // ================= خلفية الـ Landing: تملأ الشاشة كلها =================
        glViewport(0, 0, W, H);  // نرجّع الـ viewport على كامل النافذة
        glClearColor(0.02f, 0.02f, 0.03f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        glUseProgram(landingProg);
        GLint tLoc = glGetUniformLocation(landingProg, "uTime");
        if (tLoc >= 0) glUniform1f(tLoc, dt);   // نستخدم dt اللي فوق

        glBindVertexArray(landVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // ================= العلم: في جزء صغير مخصص =================
        if (selected != "Landing") {
            const int topBarH      = 84;   // بعد الـ TopBar
            const int leftPanelW   = 280;  // منيو
            const int rightPanelW  = 280;  // Info
            const int bottomMargin = 10;

            int vpX = leftPanelW;
            int vpY = topBarH;
            int vpW = W - leftPanelW - rightPanelW;
            int vpH = H - topBarH - bottomMargin;

            glViewport(vpX, vpY, vpW, vpH);

            glUseProgram(flagProg);
            Flag* f = manager.get(selected);
            if (f) f->draw();
        }

        // ---------- ImGui فوق كل حاجة ----------
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);


        
    }

    // cleanup
    manager.cleanup();
    glDeleteProgram(landingProg);
    glDeleteProgram(flagProg);
    glDeleteVertexArrays(1, &landVAO);
    glDeleteBuffers(1, &landVBO);
    glDeleteBuffers(1, &landEBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
