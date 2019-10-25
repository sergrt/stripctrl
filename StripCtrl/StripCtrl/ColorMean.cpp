#include "stdafx.h"
#include "ColorMean.h"

int screen_width_ = 1920;

QColor ColorMean::calcMean(const std::vector<unsigned char>& data, int x, int y, int w, int h) {


    unsigned B_avg = data[0];
    unsigned G_avg = data[1];
    unsigned R_avg = data[2];


    for (int i = x; i < x + w; ++i) {
        for (int j = y; j < y + h; ++j) {
            auto pos = j * screen_width_ * 4 + i * 4;

            B_avg += data[pos + 0];
            G_avg += data[pos + 1];
            R_avg += data[pos + 2];
        }
    }

    R_avg = R_avg / (h * w);
    G_avg = G_avg / (h * w);
    B_avg = B_avg / (h * w);
    QColor resRGB(R_avg, G_avg, B_avg);
    return resRGB;
}

const int pt_count = 200;
QColor ColorMean::calcMonte(const std::vector<unsigned char>& data, int x, int y, int w, int h) {
    std::mt19937 gen;
    gen.seed(time(0));
    std::uniform_int_distribution<> w_dist(x, x+w);
    std::uniform_int_distribution<> h_dist(y, y+h);


    unsigned B_avg = data[0];
    unsigned G_avg = data[1];
    unsigned R_avg = data[2];

    

    //int pt_count = 100;
    for (int i = 0; i < pt_count; ++i) {
        auto random_x = w_dist(gen);
        auto random_y = h_dist(gen);

        auto pos = random_y * screen_width_ * 4 + random_x * 4;

        B_avg += data[pos + 0];
        G_avg += data[pos + 1];
        R_avg += data[pos + 2];

    }

    R_avg = R_avg / pt_count;
    G_avg = G_avg / pt_count;
    B_avg = B_avg / pt_count;
    QColor resRGB(R_avg, G_avg, B_avg);
    return resRGB;
}

QColor ColorMean::calcMonteN(const std::vector<unsigned char>& data, int x, int y, int w, int h) {
    std::mt19937 gen;
    gen.seed(time(0));
    std::uniform_int_distribution<> w_dist(x, x + w);
    std::uniform_int_distribution<> h_dist(y, y + h);

    double r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
        
    for (int i = 0; i < pt_count; ++i) {
        const auto random_x = w_dist(gen);
        const auto random_y = h_dist(gen);

        const auto pos = random_y * screen_width_ * 4 + random_x * 4;

        const QColor color(data[pos + 0], data[pos + 1], data[pos + 2], data[pos + 3]);
        r += color.redF();
        g += color.greenF();
        b += color.blueF();
        a += color.alphaF();

    }
    double scale = 1.0f / pt_count;
    return QColor::fromRgbF(r*scale, g*scale, b*scale, a*scale).rgba();
}
