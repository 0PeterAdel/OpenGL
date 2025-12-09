// اول حاجة نضيف المكتبات اللي عندنا

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


using namespace std;

/*
    هنا احنا عرفنا source code الخاص بال vertexshader
    اللغة المستخدمة هيا GLSL

    vertext_shader: 
        هوا طريقة معالجة البينات المدخلة و تحويلها من صيغة الي صيغة يفهما المعالج الرسومي
    
    fragment_shader:
        هوا كود يطبق علي كل pixel او fragment
*/

// ده المسئول عن معالج الاحداثيات
const char *vertex_shader_source_code = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 vColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    vColor = aColor;
}
)";

const char *fragment_shader_source_code = R"(
#version 330 core
out vec4 FragColor;
in vec3 vColor;

void main()
{
    FragColor = vec4(vColor, 1.0);
}
)";

// المقاسات الافتراضية عندنا للنافذة
const int DWIDTH = 800;
const int DHEIGHT = 600;


int main(void)
{
    // لو هنكبر المشروع زي ما خططنا مفروض هنا نبدأ نعرض لستة للمستخدم و نهندل المدخلات الخاصة بيه
    // منها تحديد حجم الويندو و تحديد اسم العلم المستخدم
    // هل عايز يرسم اكتر من علم





    // هنا بنبدأ نحمل المتكبة و نعمل اعداد للبيئة و بنعمل تشك
    // هل تم تحميلها بشكل صحيح نكمل او لأ
    if (!glfwInit())
    {
        cerr << "Failed to Initialize GLFW \n";
        return -1;
    }


    // هنا بقي بنبدأ نعمل اعداد لاصدار المكتبة اللي هنستخدمها و للبروفايل اللي هنستخدمه
    // Default version: 3.3, Default profile: coreProfile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // هنا هنبدأ ننشئ الويندو او النافذة اللي هنشتغل عليها
    GLFWwindow *window1 = glfwCreateWindow(DWIDTH, DHEIGHT, "Project Name", nullptr, nullptr);

    // هنعمل تست نشوف هل تم انشاء النافذة بواسطة البرنامج
    if (!window1)
    {
        // بنطبع رسالة
        cerr << "Failed to Create a window\n";

        // بنعمل تدمير للمكتبة في الميموري
        glfwTerminate();

        return -1;
    }


    // بعد كده هنعمل اي لو عندنا اكتر من نافذة او ويندو لازم نختار منهم واحدة نشتغل عليها
    // عن طريق الخطوة دي بنحدد النافذة اللي هنشتغل عليها في الوقت الحالي
    glfwMakeContextCurrent(window1);




    // هنعمل استدعاء لوظائف المكتبةة و تحميلها في الميموري عن طريق ال glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cerr << "Can't Load functions using glad\n";
        glfwDestroyWindow(window1);
        glfwTerminate();
        return -1;
    }





    // النافذة الواحدة ممكن تتقسم لاكتر من viewport
    //جزء نعرض فيه رسوماتنا 
    // بس في حالتنا هنعتبر ان كل الويندو النافذة بتاعتنا

    glViewport(
        // نقطة البداية من اقصي اليسار في الاسفل
        0,
        0, 
        // هنرسم في نطاق قد اي بندي ال height , width
        DWIDTH,
        DHEIGHT
    );


    // نبدأ انشاء البرنامج الخاص بال shaders
        // ننشئ ال vert shader
        // نربطة بال source code
        // compile vertex shader
        // نعمل تشك هل كل حاجة تم ترجمتها بنجاح

    // هنا انشئنا ال vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source_code, nullptr); // link source code above to this object
    glCompileShader(vertex_shader);

    GLint success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char error_message[512];
        glGetShaderInfoLog(vertex_shader, 512, nullptr, error_message);
        cerr << "Vertex Shader Compilation failed:\n" << error_message << endl;
    }

    // هننشئ ال fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader, 1, &fragment_shader_source_code, nullptr); // link source code above to this object
    glCompileShader(fragment_shader);// بنترجم ال fragment shader

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        // هنا عملنا مكان نخزن فيه رسالة الخطا اللي هترجعها المكتبة
        char error_message[512];

        // بنقراء رسالة الخطاء دي و نخزنها عشان نطبعها
        glGetShaderInfoLog(fragment_shader, 512, nullptr, error_message);
        cerr << "Vertex Shader Compilation failed:\n" << error_message << endl;
    }


    // انشئ البرنامج عشان نبدأ نشغله
    // نجمع البرامج الصغيرة و نربطها بالبرنامج الاساسى
    GLuint ShaderProgram = glCreateProgram();
    glAttachShader(ShaderProgram, vertex_shader);
    glAttachShader(ShaderProgram, fragment_shader);
    glLinkProgram(ShaderProgram);


    // دلوقتي احنا مش محتاجين الاجزاء الصغيرة ال shaders 
    // بقي عندنا حاجة اكبر بتحتوي علي الاجزاء الصغيرة و هتشغلها
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);


    // USING AI
    // --- Geometry Data (The Flag) ---
    // علم الغابون: 3 مستطيلات أفقية
    // الترتيب: X, Y, Z (Position) | R, G, B (Color)
    
    // الألوان التقريبية:
    // الأخضر: 0.0, 0.6, 0.3
    // الأصفر: 1.0, 0.8, 0.0
    // الأزرق: 0.0, 0.3, 0.7


    float width = 0.8f; // عرض العلم
    float height_top = 0.75f; // حدود المستطيل العلوي
    float height_middle1 = 0.25f; // الحد الفاصل الأول
    float height_middle2 = -0.25f; // الحد الفاصل الثاني
    float height_bottom = -0.75f; // حدود المستطيل السفلي

    GLfloat vertices[] = {
        // === 1. الشريط العلوي (أخضر) ===
        // Triangle 1
        -width, height_middle1, 0.0f,   0.0f, 0.6f, 0.3f, // BL
         width, height_middle1, 0.0f,   0.0f, 0.6f, 0.3f, // BR
        -width, height_top,  0.0f,   0.0f, 0.6f, 0.3f, // TL
        // Triangle 2
         width, height_top,  0.0f,   0.0f, 0.6f, 0.3f, // TR
        -width, height_top,  0.0f,   0.0f, 0.6f, 0.3f, // TL
         width, height_middle1, 0.0f,   0.0f, 0.6f, 0.3f, // BR

        // === 2. الشريط الأوسط (أصفر) ===
        // Triangle 1
        -width, height_middle2, 0.0f,   1.0f, 0.8f, 0.0f, // BL
         width, height_middle2, 0.0f,   1.0f, 0.8f, 0.0f, // BR
        -width, height_middle1, 0.0f,   1.0f, 0.8f, 0.0f, // TL
        // Triangle 2
         width, height_middle1, 0.0f,   1.0f, 0.8f, 0.0f, // TR
        -width, height_middle1, 0.0f,   1.0f, 0.8f, 0.0f, // TL
         width, height_middle2, 0.0f,   1.0f, 0.8f, 0.0f, // BR

        // === 3. الشريط السفلي (أزرق) ===
        // Triangle 1
        -width, height_bottom,  0.0f,   0.0f, 0.3f, 0.7f, // BL
         width, height_bottom,  0.0f,   0.0f, 0.3f, 0.7f, // BR
        -width, height_middle2, 0.0f,   0.0f, 0.3f, 0.7f, // TL
        // Triangle 2
         width, height_middle2, 0.0f,   0.0f, 0.3f, 0.7f, // TR
        -width, height_middle2, 0.0f,   0.0f, 0.3f, 0.7f, // TL
         width, height_bottom,  0.0f,   0.0f, 0.3f, 0.7f, // BR
    };

    // هنبدا نجهز الميموري الخاص بينا 
    // VAO, VBO
    // هننشئ المكانين
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // هنحدد انهي VAO, VBO 
    // هنشتغل عليه عن طريق اننا نفعله زي موضوع الويندو فوق بالظبط
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // هنبدا نملي البيانت في ال BUFFER DATA VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configure vertex attribute
    GLsizei stride = 6 * sizeof(float);

    glVertexAttribPointer(
        0,                // attribute index
        3,                // size (x,y,z)
        GL_FLOAT,         // data type
        GL_FALSE,         // normalize
        stride,// stride
        (void*)0          // offset
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Point again to the start of the VBO, VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // هنبدأ نرسم بقا
    while (!glfwWindowShouldClose(window1))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(ShaderProgram);
        glBindVertexArray(VAO);

        // نرسم 18 نقطة (3 مستطيلات * 6 نقاط لكل مستطيل)
        glDrawArrays(GL_TRIANGLES, 0, 18);

        glfwSwapBuffers(window1);
        glfwPollEvents();
    }
    
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(ShaderProgram);
    glfwDestroyWindow(window1);
    glfwTerminate();

    return 0;
}

