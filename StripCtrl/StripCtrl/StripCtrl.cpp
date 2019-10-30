#include "stdafx.h"
#include "StripCtrl.h"
#include "CaptureStrategy.h"
#include <memory>
#include "D3dCapture.h"
#include "GdiCapture.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <thread>
#include "ColorCalculator.h"
#include "Settings.h"

#include <atomic>
#include <mutex>
#include <QSpinBox>

namespace {
    std::atomic<bool> stop_capture(true);
    std::atomic<bool> preview_active(true);

    int sum_fps = 0;
    int cnt = 0;
    const int max_cnt = 256;
    void initFps() {
        cnt = 0;
        sum_fps = 0;
    }

    std::mutex fps_mutex;
    std::condition_variable fps_cond;
}

std::unique_ptr<CaptureStrategy> makeCaptureStrategy(Settings::CaptureEngine capture_engine) {
    if (capture_engine == Settings::CaptureEngine::D3D)
        return std::make_unique<D3dCapture>();
    else if (capture_engine == Settings::CaptureEngine::Gdi)
        return std::make_unique<GdiCapture>();
    else
        throw std::runtime_error("Unhandled capture strategy");
}


void captureFunc(LedColors& colors, const Settings& settings) {
    std::cout << "Capture thread start" << "\n";
    initFps();

    ColorCalculator color_calculator(settings);
    auto capture = makeCaptureStrategy(settings.capture_engine_);
    capture->init();
    while(!stop_capture) {
        const auto now = std::chrono::system_clock::now();
        capture->capture();
        // Calculate regions
        {
            std::lock_guard<std::mutex> lock(fps_mutex);
            colors = color_calculator.calc(capture->data(), capture->screenSize());

            auto diff = std::chrono::system_clock::now() - now;
            if (cnt == max_cnt) {
                cnt = 1;
                sum_fps = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
            } else {
                ++cnt;
                sum_fps += std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
            }

            std::cout << sum_fps / (double)cnt << "\n";
            fps_cond.notify_all();
        }

        // Send colors to strip here
    }
    capture->cleanup();
    std::cout << "Capture thread stop" << "\n";
}

void UiUpdateThread::run() {
    using namespace std::chrono_literals;

    auto old_counter_val = cnt;
    while (preview_active) {
        old_counter_val = cnt;
        std::unique_lock<std::mutex> lock(fps_mutex);
        fps_cond.wait(lock, [old_counter_val]() {return old_counter_val != cnt || !preview_active;});
        emit updateUi();
    }
}

void StripCtrl::startCaptureThread() {
    stop_capture = false;
    capture_thread_ = std::thread(captureFunc, std::ref(colors_), std::cref(settings_));
}

void StripCtrl::stopCaptureThread() {
    stop_capture = true;
    if (capture_thread_.joinable())
        capture_thread_.join();
}

void StripCtrl::restartCaptureThread() {
    stopCaptureThread();
    startCaptureThread();
}

StripCtrl::StripCtrl(QWidget *parent)
    : QMainWindow(parent) {
    ui.setupUi(this);
    settingsToUi();

    connect(ui.capture_d3d, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            uiToSettings();
            if (!stop_capture)
                restartCaptureThread();
        }
    });

    connect(ui.capture_gdi, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            uiToSettings();
            if (!stop_capture)
                restartCaptureThread();
        }
    });

    connect(ui.leds_h, &QLineEdit::textChanged, this, [this]() {
        if (ui.leds_h->text().toInt() > 0)
            uiToSettings();
    });

    connect(ui.leds_v, &QLineEdit::textChanged, this, [this]() {
        if (ui.leds_v->text().toInt() > 0)
            uiToSettings();
    });

    connect(ui.hor_segment_height, &QLineEdit::textChanged, this, [this]() {
        if (ui.hor_segment_height->text().toInt() > 0)
            uiToSettings();
    });

    connect(ui.vert_segment_width, &QLineEdit::textChanged, this, [this]() {
        if (ui.vert_segment_width->text().toInt() > 0)
            uiToSettings();
    });

    connect(ui.color_full, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            uiToSettings();
            if (!stop_capture)
                restartCaptureThread();
        }
    });

    connect(ui.color_monte_carlo, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            uiToSettings();
            if (!stop_capture)
                restartCaptureThread();
        }
    });

    connect(ui.monte_carlo_points, &QLineEdit::textChanged, this, [this]() {
        if (ui.monte_carlo_points->text().toInt() > 0)
            uiToSettings();
    });

    connect(ui.color_threads, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
        uiToSettings();
        if (!stop_capture)
            restartCaptureThread();
    });



    connect(ui.activate_, &QCheckBox::stateChanged, this, [this]() {
        if (ui.activate_->isChecked()) {
            startCaptureThread();
        } else {
            stopCaptureThread();
        }
    });

    connect(ui.previewBox, &QGroupBox::clicked, this, [this](bool checked) {
        if (checked) {
            startPreviewThread();
        } else {
            stopPreviewThread();
        }
    });

    if (preview_active)
        ui_update_thread_.start();

    connect(&ui_update_thread_, &UiUpdateThread::updateUi, this, [this]() {
        this->ui.color_widget_->setPreviewData(std::move(colors_), sum_fps / static_cast<double>(cnt));
    }, Qt::ConnectionType::DirectConnection);
}

void StripCtrl::startPreviewThread() {
    preview_active = true;
    ui_update_thread_.start();
}

void StripCtrl::stopPreviewThread() {
    using namespace std::chrono_literals;

    preview_active = false;
    fps_cond.notify_all();
    while (!ui_update_thread_.isFinished())
        std::this_thread::sleep_for(100ms);
}

StripCtrl::~StripCtrl() {
    stopPreviewThread();
    stopCaptureThread();
}

void StripCtrl::settingsToUi() {
    ui.capture_d3d->setChecked(settings_.capture_engine_ == Settings::CaptureEngine::D3D);
    ui.capture_gdi->setChecked(settings_.capture_engine_ == Settings::CaptureEngine::Gdi);

    ui.leds_h->setText(QString("%1").arg(settings_.leds_h_));
    ui.leds_v->setText(QString("%1").arg(settings_.leds_v_));

    ui.hor_segment_height->setText(QString("%1").arg(settings_.horizontal_segment_height_));
    ui.vert_segment_width->setText(QString("%1").arg(settings_.vertical_segment_width_));

    ui.color_full->setChecked(settings_.color_calculation_ == Settings::ColorCalculation::FullSegment);
    ui.color_monte_carlo->setChecked(settings_.color_calculation_ == Settings::ColorCalculation::MonteCarlo);

    ui.monte_carlo_points->setText(QString("%1").arg(settings_.monte_carlo_points_));
    ui.color_threads->setValue(settings_.color_threads_);
}

void StripCtrl::uiToSettings() {
    settings_.capture_engine_ = ui.capture_d3d->isChecked() ? Settings::CaptureEngine::D3D : Settings::CaptureEngine::Gdi;

    settings_.leds_h_ = ui.leds_h->text().toInt();
    settings_.leds_v_ = ui.leds_v->text().toInt();

    settings_.horizontal_segment_height_ = ui.hor_segment_height->text().toInt();
    settings_.vertical_segment_width_ = ui.vert_segment_width->text().toInt();

    settings_.color_calculation_ = ui.color_full->isChecked() ? Settings::ColorCalculation::FullSegment : Settings::ColorCalculation::MonteCarlo;
    
    settings_.monte_carlo_points_ = ui.monte_carlo_points->text().toInt();
    settings_.color_threads_ = ui.color_threads->value();

    settings_.save();
}
