#pragma once
#include <QColor>

class ColorMean {
public:
    static QColor calcMean(const std::vector<unsigned char>& data, int x, int y, int w, int h);
    static QColor calcMonte(const std::vector<unsigned char>& data, int x, int y, int w, int h);
    static QColor calcMonteN(const std::vector<unsigned char>& data, int x, int y, int w, int h);
};