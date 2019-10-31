#include "stdafx.h"
#include "GdiCapture.h"

GdiCapture::~GdiCapture() {
    cleanup();
}

void GdiCapture::init() {
    hDesktopWnd = GetDesktopWindow();
    hDesktopDC = GetDC(hDesktopWnd);
    hCaptureDC = CreateCompatibleDC(hDesktopDC);

    screen_width_ = GetSystemMetrics(SM_CXSCREEN);
    screen_height_ = GetSystemMetrics(SM_CYSCREEN);

    captureBitmap = CreateCompatibleBitmap(hDesktopDC, screen_width_, screen_height_);
    const auto buf_size = screen_width_ * screen_height_ * BITS_PER_PIXEL / 8;
    data_.resize(buf_size);

    SelectObject(hCaptureDC, captureBitmap);
}


void GdiCapture::cleanup() {
    if (captureBitmap)
        DeleteObject(captureBitmap);

    if (hDesktopDC)
        ReleaseDC(hDesktopWnd, hDesktopDC);

    if (hCaptureDC)
        DeleteDC(hCaptureDC);
}

void GdiCapture::reinitialize() {
    cleanup();
    init();
}

void GdiCapture::capture() {
    // Check for resolution changes
    const int cur_screen_width = GetSystemMetrics(SM_CXSCREEN);
    const int cur_screen_height = GetSystemMetrics(SM_CYSCREEN);

    if (cur_screen_width != screen_width_ || cur_screen_height != screen_height_)
        reinitialize();
    
    BitBlt(hCaptureDC, 0, 0, screen_width_, screen_height_, hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT);
    GetBitmapBits(captureBitmap, data_.size(), data_.data());
}

QSize GdiCapture::screenSize() const {
    return QSize(screen_width_, screen_height_);
}

