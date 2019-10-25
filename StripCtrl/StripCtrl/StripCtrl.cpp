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

void captureFunc(LedColors& colors) {
    Settings settings_;
    std::unique_ptr<ColorCalculator> color_calculator_ = std::make_unique<ColorCalcMonteCarlo>(settings_);
    //auto capture = std::make_unique<D3dCapture>();
    auto capture = std::make_unique<GdiCapture>();
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

StripCtrl::StripCtrl(QWidget *parent)
    : QMainWindow(parent) {
    ui.setupUi(this);


    connect(ui.activate_, &QCheckBox::stateChanged, this, [this]() {
        if (ui.activate_->isChecked()) {
            stop = false;
            capture_thread_ = std::thread(captureFunc, std::ref(colors_));
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

