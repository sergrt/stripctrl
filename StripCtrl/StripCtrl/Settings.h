#pragma once

class Settings final {
public:
    enum class CaptureEngine {
        D3D,
        Gdi
    };

    Settings();
    ~Settings();

    int leds_h_ = 16;
    int leds_v_ = 8;

    int vertical_segment_width_ = 100;
    int horizontal_segment_height_ = 100;

    CaptureEngine capture_engine_;
};