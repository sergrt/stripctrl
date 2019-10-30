#pragma once

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

    CaptureEngine capture_engine_;
    ColorCalculation color_calculation_;
};