#pragma once
#include <QSize>
#include <vector>

class CaptureStrategy {
public:
    virtual ~CaptureStrategy() = default;
    virtual void init() {}
    virtual void capture() = 0;
    virtual void cleanup() {}

    const std::vector<unsigned char>& data() const;
    virtual QSize screenSize() const = 0;

protected:
    std::vector<unsigned char> data_;
};
