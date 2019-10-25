#pragma once
#include <vector>
#include "LedColors.h"

class ColorCalculator {
public:
    virtual ~ColorCalculator() {}
    virtual LedColors calc(const std::vector<unsigned char>& data, const QSize& screen_size) = 0;
};
