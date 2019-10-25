#pragma once

#include "CaptureStrategy.h"

//#include <QSize>

#include <d3d9.h>
#include <atlbase.h>

class D3dCapture : public CaptureStrategy {
public:
    void init() override;
    void capture() override;
    QSize screen_size() const override;

private:
    void reinitialize();

    CComPtr<IDirect3D9> d3d_;
    CComPtr<IDirect3DDevice9> d3d_device_;
    CComPtr<IDirect3DSurface9> d3d_surface_;

    int screen_width_ = 1920;
    int screen_height_ = 1080; // Last detected screen resolution
};
