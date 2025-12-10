#ifndef EGYPT_EAGLE_FLAG_HPP
#define EGYPT_EAGLE_FLAG_HPP

#include "../Core.hpp"

class EgyptEagleFlag : public Flag {
public:
    EgyptEagleFlag():Flag("Egypt"){}
    void init() override;
};

#endif
