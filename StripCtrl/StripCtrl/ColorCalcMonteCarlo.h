#pragma once
#include "ColorCalculator.h"
#include <ctpl_stl.h>

class Settings;

class ColorCalcMonteCarlo : public ColorCalculator {
public:
    ColorCalcMonteCarlo(const Settings& settings);
    LedColors calc(const std::vector<unsigned char>& data, const QSize& screen_size) override;
private:
    
    const int points_count_ = 70;//256;
    const Settings& settings_;

    const int threads_count_ = 5;
    ctpl::thread_pool thread_pool_;
};