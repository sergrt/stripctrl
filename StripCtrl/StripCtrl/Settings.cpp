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
        static const QString SerialPortName;
        static const QString BaudRate;
        static const QString UseGammaCorrection;
        static const QString GammaRed;
        static const QString GammaGreen;
        static const QString GammaBlue;
        static const QString UseLimits;
        static const QString Threshold;
        static const QString Limit;
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
const QString IniFile::General::SerialPortName = "SerialPortName";
const QString IniFile::General::BaudRate = "BaudRate";
const QString IniFile::General::UseGammaCorrection = "UseGammaCorrection";
const QString IniFile::General::GammaRed = "GammaRed";
const QString IniFile::General::GammaGreen = "GammaGreen";
const QString IniFile::General::GammaBlue = "GammaBlue";
const QString IniFile::General::UseLimits = "UseLimits";
const QString IniFile::General::Threshold = "Threshold";
const QString IniFile::General::Limit = "Limit";

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

    serial_port_name_ = settings.value(IniFile::General::SerialPortName, "").toString();
    baud_rate_ = static_cast<QSerialPort::BaudRate>(settings.value(IniFile::General::BaudRate, QSerialPort::BaudRate::Baud115200).toInt());

    use_gamma_correction_ = settings.value(IniFile::General::UseGammaCorrection, false).toBool();
    gamma_red_ = settings.value(IniFile::General::GammaRed, 0).toInt();
    gamma_green_ = settings.value(IniFile::General::GammaGreen, 0).toInt();
    gamma_blue_ = settings.value(IniFile::General::GammaBlue, 0).toInt();

    use_limits_ = settings.value(IniFile::General::UseLimits, false).toBool();
    threshold_ = settings.value(IniFile::General::Threshold, 0).toInt();
    limit_ = settings.value(IniFile::General::Limit, 0).toInt();

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

    settings.setValue(IniFile::General::SerialPortName, serial_port_name_);
    settings.setValue(IniFile::General::BaudRate, static_cast<int>(baud_rate_));

    settings.setValue(IniFile::General::UseGammaCorrection, use_gamma_correction_);
    settings.setValue(IniFile::General::GammaRed, gamma_red_);
    settings.setValue(IniFile::General::GammaGreen, gamma_green_);
    settings.setValue(IniFile::General::GammaBlue, gamma_blue_);

    settings.setValue(IniFile::General::UseLimits, use_limits_);
    settings.setValue(IniFile::General::Threshold, threshold_);
    settings.setValue(IniFile::General::Limit, limit_);

    settings.endGroup();
}