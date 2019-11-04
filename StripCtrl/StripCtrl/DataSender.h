#pragma once
#include "LedColors.h"

#include <QSerialPort>

class Settings;

class DataSender {
public:
    DataSender(const Settings& settings);
    ~DataSender();

    void send(const LedColors& data);
private:
    QSerialPort serial_port_;
    const Settings& settings_;
};
