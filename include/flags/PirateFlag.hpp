// include/flags/PirateFlag.hpp
// تعريف كلاس علم القراصنة (PirateFlag) 
// تحديــــــــــــــــــــــــــــــــــــــث الساعة 7 صباحاً 20 يونيو 2024
#pragma once

#include "../Core.hpp"

/*
    PirateFlag (detailed):
        - خلفية سوداء
        - جمجمة مفصّلة (رأس دائري، عيون، أنف، فك، اسنان)
        - شوحتين (سيفين قصيرين) متقاطعتين تحت الجمجمة
        - باندانا/رباط على الرأس وتفاصيل بسيطة للحواف الممزقة
        - كل العناصر مرسومة بالكامل باستخدام الـ Mesh (polygons)
*/
class PirateFlag : public Flag {
public:
    PirateFlag();
    void init() override;
    void draw() override;
};
