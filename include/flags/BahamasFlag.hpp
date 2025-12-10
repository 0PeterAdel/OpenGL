// include/flags/BahamasFlag.hpp
// تعريف كلاس علم جزر البهاما (BahamasFlag)

#pragma once

#include "../Core.hpp"

/*
    BahamasFlag:
        - علم جزر البهاما:
            * مثلث أسود عند الجهة اليسرى (hoist)
            * شريط علوي ووسط و سفلي: لون تركوازي - أصفر - تركوازي
        - يستخدم Mesh الموجود في الكلاس الأساسي Flag
*/

class BahamasFlag : public Flag {
public:
    BahamasFlag();
    void init() override;
    void draw() override;
};
