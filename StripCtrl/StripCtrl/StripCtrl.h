#pragma once
#include "ui_StripCtrl.h"
#include "Settings.h"

#include <QtWidgets/QMainWindow>
#include <thread>

class UiUpdateThread : public QThread {
    Q_OBJECT

public:
    void run() override;

signals:
    void updateUi();
};

class StripCtrl : public QMainWindow {
    Q_OBJECT

public:
    StripCtrl(QWidget *parent = Q_NULLPTR);
    ~StripCtrl();

private:
    Ui::StripCtrlClass ui;
    void settingsToUi() const;

    void startCaptureThread();
    void stopCaptureThread();
    void restartCaptureThread();
    void startPreviewThread();
    void stopPreviewThread();

    void setUnsafeControlsEnabled(bool enabled) const;

    std::thread capture_thread_;
    UiUpdateThread ui_update_thread_;

    LedColors colors_;
    Settings settings_;

private slots:
    void uiToSettings();
};
