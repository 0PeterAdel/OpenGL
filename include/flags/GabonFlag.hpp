// include/flags/GabonFlag.hpp
// علم الجابون: 3 شرائط أفقية مرسومة بالكامل بـ OpenGL 

#pragma once

#include "../Core.hpp"   // يحتوي علي الكلاس الأساسي Flag و FlagManager

/*
    GabonFlag:
        - يرث من Flag
        - مسئول عن:
            * بناء الـ vertices الخاصة بالعلم
            * تجهيز الـ VAO / VBO
            * رسم العلم باستخدام glDrawArrays
*/
class GabonFlag : public Flag {
public:
    // الكونستركتور: يرسل اسم العلم للكلاس الأساسي
    GabonFlag();

    // هنا بنبني الـ geometry (نقاط العلم) و بنجهز الـ VAO / VBO
    void init() override;

    // دالة الرسم: تستدعي أمر الرسم للعلم
    void draw() override;
};
