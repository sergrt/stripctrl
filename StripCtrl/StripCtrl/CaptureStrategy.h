#pragma once
#include "LedColors.h"

class CaptureStrategy {
public:
    virtual ~CaptureStrategy() = default;
    virtual void init() {}
    virtual void capture() = 0;
    virtual void cleanup() {}

    const std::vector<unsigned char>& data() const {
        return data_;
    }

    virtual QSize screen_size() const = 0;
protected:
    std::vector<unsigned char> data_;
};
