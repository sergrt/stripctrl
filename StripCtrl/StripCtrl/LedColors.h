#pragma once
#include <vector>
#include <QColor>

struct LedColors {
    LedColors() = default;
    LedColors(LedColors&& other) noexcept {
        left_strip = std::move(other.left_strip);
        top_strip = std::move(other.top_strip);
        right_strip = std::move(other.right_strip);
        bottom_strip = std::move(other.bottom_strip);
    }

    LedColors& operator=(LedColors&& other) noexcept {
        left_strip = std::move(other.left_strip);
        top_strip = std::move(other.top_strip);
        right_strip = std::move(other.right_strip);
        bottom_strip = std::move(other.bottom_strip);
        return *this;
    }
    std::vector<QColor> left_strip;
    std::vector<QColor> top_strip;
    std::vector<QColor> right_strip;
    std::vector<QColor> bottom_strip;
};