#pragma once
#include "ColorCalculator.h"
#include <ctpl_stl.h>

class Settings;

class ColorCalcMonteCarlo : public ColorCalculator {
public:
    ColorCalcMonteCarlo(const Settings& settings);
    LedColors calc(const std::vector<unsigned char>& data, const QSize& screen_size) override;
private:

    const Settings& settings_;
    ctpl::thread_pool thread_pool_;
};