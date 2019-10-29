#include "stdafx.h"
#include "Settings.h"
#include <QString>
#include <QSettings>
#include <stdexcept>

namespace {
    const auto settings_file_name = QString("settings.ini");

    const auto capture_engine_d3d = QString("Direct3D");
    const auto capture_engine_gdi = QString("GDI");
}

struct IniFile {
    static const QString General;
    struct General {
        static const QString LedsH;
        static const QString LedsV;
        static const QString VertSegmentWidth;
        static const QString HorSegmentHeight;
        static const QString CaptureEngine;
    };
};

const QString IniFile::General = "General";

const QString IniFile::General::LedsH = "LedsH";
const QString IniFile::General::LedsV = "LedsV";
const QString IniFile::General::VertSegmentWidth = "VertSegmentWidth";
const QString IniFile::General::HorSegmentHeight = "HorSegmentHeight";
const QString IniFile::General::CaptureEngine = "CaptureEngine";

Settings::Settings() {
    QSettings settings(settings_file_name, QSettings::IniFormat);
    settings.beginGroup(IniFile::General);
    leds_h_ = settings.value(IniFile::General::LedsH, 16).toInt();
    leds_v_ = settings.value(IniFile::General::LedsV, 8).toInt();
    horizontal_segment_height_ = settings.value(IniFile::General::HorSegmentHeight, 100).toInt();
    vertical_segment_width_ = settings.value(IniFile::General::VertSegmentWidth, 100).toInt();
    auto capture_engine_str = settings.value(IniFile::General::CaptureEngine, "").toString();
    if (capture_engine_str == capture_engine_d3d)
        capture_engine_ = CaptureEngine::D3D;
    else if (capture_engine_str == capture_engine_gdi)
        capture_engine_ = CaptureEngine::Gdi;
    else
        throw(std::runtime_error("Invalid capture engine"));

    settings.endGroup();
}

Settings::~Settings() {
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

    settings.endGroup();
}