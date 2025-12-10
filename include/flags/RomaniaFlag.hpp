// include/flags/RomaniaFlag.hpp
// تعريف كلاس علم رومانيا (RomaniaFlag)

#pragma once

#include "../Core.hpp"

/*
    RomaniaFlag:
        - يرسم علم رومانيا:
            * ثلاث شرائط رأسية متساوية
            * أزرق (يسار) - أصفر (وسط) - أحمر (يمين)
*/

class RomaniaFlag : public Flag {
public:
    RomaniaFlag();

    void init() override;
    void draw() override;
};
