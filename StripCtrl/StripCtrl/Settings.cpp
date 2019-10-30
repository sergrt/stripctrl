#include "stdafx.h"
#include "Settings.h"

#include <QString>
#include <QSettings>
#include <stdexcept>

namespace {
    const auto settings_file_name = QString("settings.ini");

    const auto capture_engine_d3d = QString("Direct3D");
    const auto capture_engine_gdi = QString("GDI");

    const auto color_calc_full_segment = QString("FullSegment");
    const auto color_calc_monte_carlo = QString("MonteCarlo");
    const auto color_calc_interleaved = QString("Interleaved");
}

struct IniFile {
    static const QString General;
    struct General {
        static const QString LedsH;
        static const QString LedsV;
        static const QString VertSegmentWidth;
        static const QString HorSegmentHeight;
        static const QString CaptureEngine;
        static const QString ColorCalculation;
        static const QString MonteCarloPoints;
        static const QString InterleavedLines;
        static const QString ColorThreads;
    };
};

const QString IniFile::General = "General";

const QString IniFile::General::LedsH = "LedsH";
const QString IniFile::General::LedsV = "LedsV";
const QString IniFile::General::VertSegmentWidth = "VertSegmentWidth";
const QString IniFile::General::HorSegmentHeight = "HorSegmentHeight";
const QString IniFile::General::CaptureEngine = "CaptureEngine";
const QString IniFile::General::ColorCalculation = "ColorCalculation";
const QString IniFile::General::MonteCarloPoints = "MonteCarloPoints";
const QString IniFile::General::InterleavedLines = "InterleavedLines";
const QString IniFile::General::ColorThreads = "ColorThreads";

Settings::Settings() {
    QSettings settings(settings_file_name, QSettings::IniFormat);
    settings.beginGroup(IniFile::General);
    leds_h_ = settings.value(IniFile::General::LedsH, 16).toInt();
    leds_v_ = settings.value(IniFile::General::LedsV, 8).toInt();
    horizontal_segment_height_ = settings.value(IniFile::General::HorSegmentHeight, 100).toInt();
    vertical_segment_width_ = settings.value(IniFile::General::VertSegmentWidth, 100).toInt();
    const auto capture_engine_str = settings.value(IniFile::General::CaptureEngine, "").toString();
    if (capture_engine_str == capture_engine_d3d)
        capture_engine_ = CaptureEngine::D3D;
    else if (capture_engine_str == capture_engine_gdi)
        capture_engine_ = CaptureEngine::Gdi;
    else
        throw std::runtime_error("Invalid capture engine");

    const auto color_calc_str = settings.value(IniFile::General::ColorCalculation, "").toString();
    if (color_calc_str == color_calc_full_segment)
        color_calculation_ = ColorCalculation::FullSegment;
    else if (color_calc_str == color_calc_monte_carlo)
        color_calculation_ = ColorCalculation::MonteCarlo;
    else if (color_calc_str == color_calc_interleaved)
        color_calculation_ = ColorCalculation::Interleaved;
    else
        throw std::runtime_error("Invalid color calc strategy");

    monte_carlo_points_ = settings.value(IniFile::General::MonteCarloPoints, 128).toInt();
    interleaved_lines_ = settings.value(IniFile::General::InterleavedLines, 2).toInt();
    color_threads_ = settings.value(IniFile::General::ColorThreads, 4).toInt();

    settings.endGroup();
}

Settings::~Settings() {
    save();
}

void Settings::save() const {
    QSettings settings(settings_file_name, QSettings::IniFormat);
    settings.beginGroup(IniFile::General);
    settings.setValue(IniFile::General::LedsH, leds_h_);
    settings.setValue(IniFile::General::LedsV, leds_v_);
    settings.setValue(IniFile::General::HorSegmentHeight, horizontal_segment_height_);
    settings.setValue(IniFile::General::VertSegmentWidth, vertical_segment_width_);

    auto capture_engine_str = QString("");
    if (capture_engine_ == CaptureEngine::D3D)
        capture_engine_str = capture_engine_d3d;
    else if (capture_engine_ == CaptureEngine::Gdi)
        capture_engine_str = capture_engine_gdi;

    settings.setValue(IniFile::General::CaptureEngine, capture_engine_str);

    auto color_calc_str = QString("");
    if (color_calculation_ == ColorCalculation::FullSegment)
        color_calc_str = color_calc_full_segment;
    else if (color_calculation_ == ColorCalculation::MonteCarlo)
        color_calc_str = color_calc_monte_carlo;
    else if (color_calculation_ == ColorCalculation::Interleaved)
        color_calc_str = color_calc_interleaved;

    settings.setValue(IniFile::General::ColorCalculation, color_calc_str);

    settings.setValue(IniFile::General::MonteCarloPoints, monte_carlo_points_);
    settings.setValue(IniFile::General::InterleavedLines, interleaved_lines_);
    settings.setValue(IniFile::General::ColorThreads, color_threads_);

    settings.endGroup();
}