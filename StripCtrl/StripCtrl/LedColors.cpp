#include "stdafx.h"
#include "LedColors.h"

LedColors::LedColors(LedColors&& other) noexcept {
    left_strip = std::move(other.left_strip);
    top_strip = std::move(other.top_strip);
    right_strip = std::move(other.right_strip);
    bottom_strip = std::move(other.bottom_strip);
}

LedColors& LedColors::operator=(LedColors&& other) noexcept {
    left_strip = std::move(other.left_strip);
    top_strip = std::move(other.top_strip);
    right_strip = std::move(other.right_strip);
    bottom_strip = std::move(other.bottom_strip);
    return *this;
}

bool LedColors::empty() const {
    return left_strip.empty();
}