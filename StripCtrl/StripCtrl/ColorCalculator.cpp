#include "stdafx.h"
#include "ColorCalculator.h"
#include "Settings.h"
#include <QColor>
#include <QRect>
#include <future>
#include <random>

ColorCalculator::ColorCalculator(const Settings& settings)
    : settings_{ settings } {

    thread_pool_.resize(settings.color_threads_);
}

QColor threadFuncMonteCarlo(int id, const std::vector<unsigned char>& data, QRect rect, int points_count, int screen_width) {
    std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> w_dist(rect.left(), rect.right());
    std::uniform_int_distribution<> h_dist(rect.top(), rect.bottom());

    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    double a = 0.0;

    static const int pixel_size = BITS_PER_PIXEL / 8;

    for (auto i = 0; i < points_count; ++i) {
        const auto random_x = w_dist(generator);
        const auto random_y = h_dist(generator);

        const auto pos = random_y * screen_width * pixel_size + random_x * pixel_size;

        const QColor color(
            data[pos + 2],
            data[pos + 1],
            data[pos + 0],
            data[pos + 3]);

        r += color.redF();
        g += color.greenF();
        b += color.blueF();
        a += color.alphaF();

    }
    const double scale = 1.0 / points_count;
    return QColor::fromRgbF(r*scale, g*scale, b*scale, a*scale);
}

QColor threadFuncFullSegment(int id, const std::vector<unsigned char>& data, QRect rect, int not_used, int screen_width) {
    unsigned int B_avg = data[0];
    unsigned int G_avg = data[1];
    unsigned int R_avg = data[2];

    const int x = rect.left();
    const int y = rect.top();
    const int w = rect.width();
    const int h = rect.height();

    static const int pixel_size = BITS_PER_PIXEL / 8;

    for (int i = x; i < x + w; ++i) {
        for (int j = y; j < y + h; ++j) {
            const auto pos = j * screen_width * pixel_size + i * pixel_size;

            B_avg += data[pos + 0];
            G_avg += data[pos + 1];
            R_avg += data[pos + 2];
        }
    }

    R_avg = R_avg / (h * w);
    G_avg = G_avg / (h * w);
    B_avg = B_avg / (h * w);
    return QColor(R_avg, G_avg, B_avg);
}

QColor threadFuncInterleaved(int id, const std::vector<unsigned char>& data, QRect rect, int line_use, int screen_width) {
    unsigned int B_avg = data[0];
    unsigned int G_avg = data[1];
    unsigned int R_avg = data[2];
    unsigned int count = 1;

    const int x = rect.left();
    const int y = rect.top();
    const int w = rect.width();
    const int h = rect.height();

    static const int pixel_size = BITS_PER_PIXEL / 8;

    for (int i = x; i < x + w; ++i) {
        if (i % line_use == 0) {
            for (int j = y; j < y + h; ++j) {
                const auto pos = j * screen_width * pixel_size + i * pixel_size;

                B_avg += data[pos + 0];
                G_avg += data[pos + 1];
                R_avg += data[pos + 2];

                ++count;
            }
        }
    }

    R_avg = R_avg / count;
    G_avg = G_avg / count;
    B_avg = B_avg / count;
    return QColor(R_avg, G_avg, B_avg);
}

/*
QColor threadFuncFullSegment_dbl(int id, const std::vector<unsigned char>& data, QRect rect, int points_count, int screen_width) {
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    double a = 0.0;

    const int x = rect.left();
    const int y = rect.top();
    const int w = rect.width();
    const int h = rect.height();

    for (int i = x; i < x + w; ++i) {
        for (int j = y; j < y + h; ++j) {
            const auto pos = j * screen_width * 4 + i * 4;

            const QColor color(
                data[pos + 2],
                data[pos + 1],
                data[pos + 0],
                data[pos + 3]);

            r += color.redF();
            g += color.greenF();
            b += color.blueF();
            a += color.alphaF();
        }
    }

    const double scale = 1.0 / points_count;
    return QColor::fromRgbF(r*scale, g*scale, b*scale, a*scale);
}
*/

LedColors ColorCalculator::calc(const std::vector<unsigned char>& data, const QSize& screen_size) {
    auto thread_func = threadFuncFullSegment;
    int thread_func_param = 0;

    if (settings_.color_calculation_ == Settings::ColorCalculation::Interleaved) {
        thread_func = threadFuncInterleaved;
        thread_func_param = settings_.interleaved_lines_;
    } else if (settings_.color_calculation_ == Settings::ColorCalculation::MonteCarlo) {
        thread_func = threadFuncMonteCarlo;
        thread_func_param = settings_.monte_carlo_points_;
    }

    

    const auto vertical_segment_height = screen_size.height() / settings_.leds_v_;
    std::vector<std::future<QColor>> results_left_strip(settings_.leds_v_);
    std::vector<std::future<QColor>> results_right_strip(settings_.leds_v_);

    for (int vert = 0; vert < settings_.leds_v_; ++vert) {
        const auto y = vert * vertical_segment_height;
        results_left_strip[vert] = thread_pool_.push(thread_func, std::cref(data),
                                                     QRect(0, y, settings_.vertical_segment_width_, vertical_segment_height),
                                                     thread_func_param,
                                                     screen_size.width());

        results_right_strip[vert] = thread_pool_.push(thread_func, std::cref(data),
                                                      QRect(screen_size.width() - settings_.vertical_segment_width_, y,
                                                            settings_.vertical_segment_width_, vertical_segment_height),
                                                      thread_func_param,
                                                      screen_size.width());
    }

    const auto horizontal_segment_width = screen_size.width() / settings_.leds_h_;
    std::vector<std::future<QColor>> results_top_strip(settings_.leds_h_);
    std::vector<std::future<QColor>> results_bottom_strip(settings_.leds_h_);

    for (int hor = 0; hor < settings_.leds_h_; ++hor) {
        const auto x = hor * horizontal_segment_width;
        results_top_strip[hor] = thread_pool_.push(thread_func, std::cref(data),
                                                   QRect(x, 0, horizontal_segment_width, settings_.horizontal_segment_height_),
                                                   thread_func_param,
                                                   screen_size.width());

        results_bottom_strip[hor] = thread_pool_.push(thread_func, std::cref(data),
                                                      QRect(x, screen_size.height() - settings_.horizontal_segment_height_,
                                                            horizontal_segment_width, settings_.horizontal_segment_height_),
                                                      thread_func_param,
                                                      screen_size.width());
    }

    LedColors led_colors(settings_.leds_h_, settings_.leds_v_);
    for (int i = 0; i < settings_.leds_v_; ++i) {
        led_colors.left_strip[i] = results_left_strip[i].get();
        led_colors.right_strip[i] = results_right_strip[i].get();
    }

    for (int i = 0; i < settings_.leds_h_; ++i) {
        led_colors.top_strip[i] = results_top_strip[i].get();
        led_colors.bottom_strip[i] = results_bottom_strip[i].get();
    }

    applyGamma(led_colors);
    return led_colors;
}

void ColorCalculator::applyGamma(LedColors& led_colors) const {
    if (!settings_.use_gamma_correction_ || (settings_.gamma_red_ == 0 && settings_.gamma_green_ == 0 && settings_.gamma_blue_ == 0))
        return;

    for (auto v : std::vector<std::vector<QColor>*>{ &led_colors.left_strip, &led_colors.top_strip, &led_colors.right_strip, &led_colors.bottom_strip }) {
        for (auto& c : *v) {
            c.setRed(c.red() + settings_.gamma_red_);
            c.setGreen(c.green() + settings_.gamma_green_);
            c.setBlue(c.blue() + settings_.gamma_blue_);
        }
    }
}
