#ifndef PIRATE_FLAG_HPP
#define PIRATE_FLAG_HPP

#include "../Core.hpp"

class PirateFlag : public Flag {
public:
    PirateFlag():Flag("Pirates"){}
    void init() override;
};

#endif
