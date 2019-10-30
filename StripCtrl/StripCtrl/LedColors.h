#pragma once
#include <QColor>
#include <vector>

struct LedColors {
    LedColors() = default;
    LedColors(int w, int h);
    LedColors(LedColors&& other) noexcept;
    LedColors& operator=(LedColors&& other) noexcept;
    bool empty() const;

    std::vector<QColor> left_strip;
    std::vector<QColor> top_strip;
    std::vector<QColor> right_strip;
    std::vector<QColor> bottom_strip;
};