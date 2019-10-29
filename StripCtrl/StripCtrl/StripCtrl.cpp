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
#include "ColorCalcMonteCarlo.h"
#include "Settings.h"
#include "ColorWidget.h"
#include <atomic>
#include <mutex>

std::atomic<bool> stop(false);
std::atomic<bool> preview_active(true);
long long sum_fps = 0;
long long cnt = 0;
std::mutex fps_mutex;
std::condition_variable fps_cond;

void timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << std::ctime(&now_c) << std::endl;
}

void captureFunc(LedColors& colors, const Settings& settings) {
    std::unique_ptr<ColorCalculator> color_calculator_ = std::make_unique<ColorCalcMonteCarlo>(settings);
    std::unique_ptr<CaptureStrategy> capture;
    if (settings.capture_engine_ == Settings::CaptureEngine::D3D)
        capture = std::make_unique<D3dCapture>();
    else
        capture = std::make_unique<GdiCapture>();
    if (!capture)
        return;
    capture->init();
    while(!stop) {
        const auto now = std::chrono::system_clock::now();
        capture->capture();
        // Calculate regions
        {
            std::lock_guard<std::mutex> lock(fps_mutex);
            colors = color_calculator_->calc(capture->data(), capture->screen_size());

            auto diff = std::chrono::system_clock::now() - now;
            sum_fps += std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
            cnt += 1;
            std::cout << sum_fps / (double)cnt << "\n";
            fps_cond.notify_all();
        }


        // Send colors to strip
                

        //std::cout << sum_fps / (double)cnt << "\n";
    }
    capture->cleanup();
}

void UiUpdateThread::run() {
    using namespace std::chrono_literals;
    //int i = 0;
    auto cnt_ = cnt;
    while (preview_active) {
        //std::this_thread::sleep_for(100ms);
        {
            cnt_ = cnt;
            std::unique_lock<std::mutex> lock(fps_mutex);
            fps_cond.wait(lock, [cnt_]() {return cnt_ != cnt;});
//            std::cout << sum_fps / (double)cnt << "\n";
            emit updateUi();
        }
    }
}

void StripCtrl::restartCaptureThread() {
    stop = true;
    if (capture_thread_.joinable())
        capture_thread_.join();

    stop = false;
    capture_thread_ = std::thread(captureFunc, std::ref(colors_), std::ref(settings_));

}

StripCtrl::StripCtrl(QWidget *parent)
    : QMainWindow(parent) {
    ui.setupUi(this);
    settingsToUi();

    connect(ui.capture_d3d, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            uiToSettings();
            restartCaptureThread();
        }
    });
    connect(ui.capture_gdi, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            uiToSettings();
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


    connect(ui.activate_, &QCheckBox::stateChanged, this, [this]() {
        if (ui.activate_->isChecked()) {
            stop = false;
            capture_thread_ = std::thread(captureFunc, std::ref(colors_), std::ref(settings_));
            //ui_update_thread_.start();
        } else {
            stop = true;
            capture_thread_.join();
        }
    });

    connect(ui.previewBox, &QGroupBox::clicked, this, [this](bool checked) {
        if (checked) {
            preview_active = true;
            ui_update_thread_.start();
        } else {
            preview_active = false;
        }
    });

    if (preview_active)
        ui_update_thread_.start();


    

    connect(&ui_update_thread_, &UiUpdateThread::updateUi, this, [this]() {
        //ui.labelFps->setText(QString("%1").arg(sum_fps / (double)cnt));
        this->ui.color_widget_->setColors(std::move(colors_));

        //this->update();
            }, //Qt::ConnectionType::BlockingQueuedConnection);//, Qt::ConnectionType::DirectConnection);
            Qt::ConnectionType::DirectConnection);
}

StripCtrl::~StripCtrl() {
    uiToSettings();

    using namespace std::chrono_literals;

    preview_active = false;
    ++cnt;
    fps_cond.notify_all();
    while (!ui_update_thread_.isFinished())
        std::this_thread::sleep_for(100ms);

    stop = true;
    if (capture_thread_.joinable())
        capture_thread_.join();
}

void StripCtrl::settingsToUi() {
    ui.capture_d3d->setChecked(settings_.capture_engine_ == Settings::CaptureEngine::D3D);
    ui.capture_gdi->setChecked(settings_.capture_engine_ == Settings::CaptureEngine::Gdi);

    ui.leds_h->setText(QString("%1").arg(settings_.leds_h_));
    ui.leds_v->setText(QString("%1").arg(settings_.leds_v_));

    ui.hor_segment_height->setText(QString("%1").arg(settings_.horizontal_segment_height_));
    ui.vert_segment_width->setText(QString("%1").arg(settings_.vertical_segment_width_));
}

void StripCtrl::uiToSettings() {
    settings_.capture_engine_ = ui.capture_d3d->isChecked() ? Settings::CaptureEngine::D3D : Settings::CaptureEngine::Gdi;

    settings_.leds_h_ = ui.leds_h->text().toInt();
    settings_.leds_v_ = ui.leds_v->text().toInt();

    settings_.horizontal_segment_height_ = ui.hor_segment_height->text().toInt();
    settings_.vertical_segment_width_ = ui.vert_segment_width->text().toInt();
}



