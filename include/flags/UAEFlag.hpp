// include/flags/UAEFlag.hpp
// تعريف كلاس علم الإمارات (UAEFlag)

#pragma once

#include "../Core.hpp"

/*
    UAEFlag:
        - يرسم علم الإمارات:
            * شريط رأسي أحمر على اليسار
            * ثلاثة شرائط أفقية (أخضر - أبيض - أسود) على اليمين
*/

class UAEFlag : public Flag {
public:
    UAEFlag();

    void init() override;
    void draw() override;
};
