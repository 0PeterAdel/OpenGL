// include/flags/BrazilFlag.hpp
// تعريف كلاس علم البرازيل (BrazilFlag)

#pragma once

#include "../Core.hpp"

/*
    BrazilFlag:
        - يرسم علم البرازيل:
            * خلفية خضراء
            * ماسة صفراء (rhombus)
            * دائرة زرقاء في المنتصف (approx. triangle fan)
            * شريط أبيض تقريبي عبر الدائرة
            * (نجوم مبسطة نستعمل نقاط ثلاثية لتقريب التأثير)
*/
class BrazilFlag : public Flag {
public:
    BrazilFlag();
    void init() override;
    void draw() override;
};
