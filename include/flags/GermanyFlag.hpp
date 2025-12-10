// include/flags/GermanyFlag.hpp
// تعريف كلاس علم ألمانيا (GermanyFlag)

#pragma once

#include "../Core.hpp"

/*
    GermanyFlag:
        - يرسم علم ألمانيا:
            * ثلاث شرائط أفقية:
              أسود (أعلى) - أحمر (وسط) - ذهبي (أسفل)
*/

class GermanyFlag : public Flag {
public:
    GermanyFlag();

    void init() override;
    void draw() override;
};
