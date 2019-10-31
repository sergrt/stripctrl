#pragma once
#include "LedColors.h"

#include <ctpl_stl.h>

class Settings;

class ColorCalculator {
public:
    ColorCalculator(const Settings& settings);
    LedColors calc(const std::vector<unsigned char>& data, const QSize& screen_size);

private:
    void applyGamma(LedColors& led_colors) const;
    const Settings& settings_;
    ctpl::thread_pool thread_pool_;
};