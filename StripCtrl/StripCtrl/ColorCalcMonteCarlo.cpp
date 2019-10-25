#include "stdafx.h"
#include "ColorCalcMonteCarlo.h"
#include "Settings.h"
#include <QColor>
#include <QRect>
#include <optional>
#include <chrono>
ColorCalcMonteCarlo::ColorCalcMonteCarlo(const Settings& settings)
    : settings_{ settings } {

    
    thread_pool_.resize(threads_count_);
}

QColor threadFunc(int id, const std::vector<unsigned char>& data, QRect rect, int points_count, int screen_width) {
    std::mt19937 generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> w_dist(rect.left(), rect.right());
    std::uniform_int_distribution<> h_dist(rect.top(), rect.bottom());

    double r = 0.0, g = 0.0, b = 0.0, a = 0.0;

    for (int i = 0; i < points_count; ++i) {
        const auto random_x = w_dist(generator);
        const auto random_y = h_dist(generator);

        const auto pos = random_y * screen_width * 4 + random_x * 4;

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
    double scale = 1.0 / points_count;
    return QColor::fromRgbF(r*scale, g*scale, b*scale, a*scale).rgba();
}
/*
QColor threadFunc(int id, const std::vector<unsigned char>& data, QRect rect, int points_count, int screen_width) {

    double r = 0.0, g = 0.0, b = 0.0, a = 0.0;

    for (int y = rect.top(); y < rect.bottom(); ++y) {
        for (int x = rect.left(); x < rect.right(); ++x) {


            const auto pos = y * screen_width * 4 + x * 4;

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
    double scale = 1.0 / (rect.width() * rect.height());
    return QColor::fromRgbF(r*scale, g*scale, b*scale, a*scale).rgba();
}
*/
LedColors ColorCalcMonteCarlo::calc(const std::vector<unsigned char>& data, const QSize& screen_size) {
    const auto horizontal_segment_width = screen_size.width() / settings_.leds_h_;

    std::vector<std::future<QColor>> results_top_strip(settings_.leds_h_);
    std::vector<std::future<QColor>> results_bottom_strip(settings_.leds_h_);
    for (int hor = 0; hor < settings_.leds_h_; ++hor) {
        const auto x = hor * horizontal_segment_width;
        results_top_strip[hor] = thread_pool_.push(threadFunc, std::cref(data),
                                                      QRect(x, 0,
                                                            horizontal_segment_width, settings_.horizontal_segment_height_),
                                                      points_count_, screen_size.width());
        results_bottom_strip[hor] = thread_pool_.push(threadFunc, std::cref(data),
                                         QRect(x, screen_size.height() - settings_.horizontal_segment_height_,
                                               horizontal_segment_width, settings_.horizontal_segment_height_),
                                         points_count_, screen_size.width());
    }


    const auto vertical_segment_height = screen_size.height() / settings_.leds_v_;
    std::vector<std::future<QColor>> results_left_strip(settings_.leds_v_);
    std::vector<std::future<QColor>> results_right_strip(settings_.leds_v_);
    for (int vert = 0; vert < settings_.leds_v_; ++vert) {
        const auto y = vert * vertical_segment_height;
        results_left_strip[vert] = thread_pool_.push(threadFunc, std::cref(data),
                                                   QRect(0, y,
                                                         settings_.vertical_segment_width_, vertical_segment_height),
                                                   points_count_, screen_size.width());
        results_right_strip[vert] = thread_pool_.push(threadFunc, std::cref(data),
                                                      QRect(screen_size.width() - settings_.vertical_segment_width_, y,
                                                            settings_.vertical_segment_width_, vertical_segment_height),
                                                      points_count_, screen_size.width());
    }
    LedColors led_colors;
    led_colors.left_strip.resize(settings_.leds_v_);
    led_colors.right_strip.resize(settings_.leds_v_);
    for (int i = 0; i < settings_.leds_v_; ++i) {
        led_colors.left_strip[i] = results_left_strip[i].get();
        led_colors.right_strip[i] = results_right_strip[i].get();
    }

    led_colors.top_strip.resize(settings_.leds_h_);
    led_colors.bottom_strip.resize(settings_.leds_h_);
    for (int i = 0; i < settings_.leds_h_; ++i) {
        led_colors.top_strip[i] = results_top_strip[i].get();
        led_colors.bottom_strip[i] = results_bottom_strip[i].get();
    }

    return led_colors;
}