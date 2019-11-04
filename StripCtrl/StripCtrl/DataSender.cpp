#include "stdafx.h"
#include "DataSender.h"
#include "Settings.h"

DataSender::DataSender(const Settings& settings)
    : settings_{ settings } {
    serial_port_.setPortName(settings_.serial_port_name_);
    if (!serial_port_.setBaudRate(settings_.baud_rate_))
        throw std::runtime_error("Unable to set baud rate");
    /*
    serial_port_.setDataBits(QSerialPort::DataBits::Data8);
    serial_port_.setParity(QSerialPort::Parity::NoParity);
    serial_port_.setStopBits(QSerialPort::StopBits::OneStop);
    */

    if (!serial_port_.open(QIODevice::ReadWrite))
        throw std::runtime_error("Unable to open port");
}

DataSender::~DataSender() {
    if (serial_port_.isOpen())
        serial_port_.close();
}

void DataSender::send(const LedColors& data) {
    if (!serial_port_.isOpen())
        return;

    QByteArray _ = serial_port_.readAll();
    if (_.isEmpty()) {
        if (!serial_port_.waitForReadyRead(1000))
            return;
        _ = serial_port_.readAll();
    }

    if (_.isEmpty())
        return;

    const unsigned short led_num = data.left_strip.size() * 2 + data.top_strip.size() * 2;
    std::vector<char> raw_data(2 + 1 + led_num * 3);
    size_t idx = 0;
    raw_data[idx++] = *(reinterpret_cast<const char*>(&led_num));
    raw_data[idx++] = *(reinterpret_cast<const char*>(&led_num) + 1);

    raw_data[idx++] = static_cast<unsigned char>(settings_.dim_value_);

    for (const auto& v : { data.left_strip, data.top_strip, data.right_strip, data.bottom_strip }) {
        for (const auto& c : v) {
            raw_data[idx++] = c.red();
            raw_data[idx++] = c.green();
            raw_data[idx++] = c.blue();
        }
    }

    serial_port_.write(raw_data.data(), raw_data.size());
    serial_port_.waitForBytesWritten();
}
