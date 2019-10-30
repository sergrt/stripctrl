#pragma once

#include "CaptureStrategy.h"

#include "windows.h"


class GdiCapture : public CaptureStrategy {
public:
    ~GdiCapture();
    void init() override;
    void capture() override;
    void cleanup() override;
    QSize screenSize() const override;

private:
    void reinitialize();

    HWND hDesktopWnd = NULL;
    HDC hDesktopDC = NULL;
    HDC hCaptureDC = NULL;

    int screen_width_ = 1920;
    int screen_height_ = 1080;

    HBITMAP captureBitmap = NULL;
};
