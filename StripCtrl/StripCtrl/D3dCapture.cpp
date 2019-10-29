#include "stdafx.h"
#include "D3dCapture.h"

#pragma comment(lib, "d3d9.lib")

void D3dCapture::init() {
    d3d_ = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d_)
        throw("Unable to Create Direct3D");

    D3DDISPLAYMODE ddm;
    if (FAILED(d3d_->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &ddm)))
        throw("Unable to Get Adapter Display Mode");

    screen_height_ = ddm.Height;
    screen_width_ = ddm.Width;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

    HWND hWnd = GetDesktopWindow();

    d3dpp.Windowed = TRUE;//WINDOW_MODE;
    d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    d3dpp.BackBufferFormat = ddm.Format;
    d3dpp.BackBufferHeight = ddm.Height;
    d3dpp.BackBufferWidth = ddm.Width;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    //if (FAILED(d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3d_device_)))
    if (FAILED(d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &d3d_device_)))
        throw("Unable to Create Device");

    if (FAILED(d3d_device_->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &d3d_surface_, NULL)))
        throw("Unable to Create Surface");

    const auto buf_size = screen_width_ * screen_height_ * BITS_PER_PIXEL / 8;
    data_.resize(buf_size);
}

void D3dCapture::reinitialize() {
    cleanup();
    init();
}
QSize D3dCapture::screen_size() const {
    return QSize(screen_width_, screen_height_);
}
void D3dCapture::capture() {
    // Check resolution		
    D3DDISPLAYMODE ddm;
    if (FAILED(d3d_->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &ddm)))
        throw("Unable to get adapter display mode");

    if (ddm.Height != screen_height_ || ddm.Width != screen_width_)
        reinitialize();

    // Capture screen
    if (FAILED(d3d_device_->GetFrontBufferData(0, d3d_surface_)))
        throw("Unable to get buffer data");

    D3DLOCKED_RECT lockedRect;
    d3d_surface_->LockRect(&lockedRect, NULL, D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);
    memcpy(data_.data(), lockedRect.pBits, data_.size());
    d3d_surface_->UnlockRect();

}

