// include/flags/AustriaFlag.hpp
// تعريف كلاس علم النمسا (AustriaFlag)

#pragma once

#include "../Core.hpp"

/*
    AustriaFlag:
        - يرسم علم النمسا:
            * ثلاث شرائط أفقية متساوية
            * أحمر (أعلى) - أبيض (وسط) - أحمر (أسفل)
        - يستخدم نفس الـ Mesh الموجود في الكلاس الأساسي Flag
*/

class AustriaFlag : public Flag {
public:
    AustriaFlag();

    // تجهيز بيانات العلم ورفعها للـ GPU
    void init() override;

    // رسم العلم (باستخدام الـ VAO / indices اللي في الـ mesh)
    void draw() override;
};
