// include/flags/JapanFlag.hpp
// تعريف كلاس علم اليابان (JapanFlag)

#pragma once

#include "../Core.hpp"

/*
    JapanFlag:
        - يرسم علم اليابان:
            * خلفية مستطيل أبيض
            * دائرة حمراء في المنتصف (الشمس)
        - يستخدم نفس الـ Mesh الموجود في الكلاس الأساسي Flag
*/

class JapanFlag : public Flag {
public:
    JapanFlag();

    // تجهيز بيانات العلم ورفعها للـ GPU
    void init() override;

    // رسم العلم (باستخدام الـ VAO / indices اللي في الـ mesh)
    void draw() override;
};
