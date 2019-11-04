#pragma once
#include <QSerialPort>

class Settings final {
public:
    enum class CaptureEngine {
        D3D,
        Gdi
    };
    enum class ColorCalculation {
        FullSegment,
        MonteCarlo,
        Interleaved
    };

    Settings();
    ~Settings();

    void save() const;

    int leds_h_ = 16;
    int leds_v_ = 8;

    int vertical_segment_width_ = 100;
    int horizontal_segment_height_ = 100;

    int monte_carlo_points_ = 128;
    int interleaved_lines_ = 2;
    int color_threads_ = 4;

    QString serial_port_name_;
    QSerialPort::BaudRate baud_rate_;

    CaptureEngine capture_engine_;
    ColorCalculation color_calculation_;

    bool use_gamma_correction_ = false;
    int gamma_red_ = 0;
    int gamma_green_ = 0;
    int gamma_blue_ = 0;

    bool use_limits_ = false;
    int threshold_ = 0;
    int limit_ = 255;

    int dim_value_ = 0;
};
