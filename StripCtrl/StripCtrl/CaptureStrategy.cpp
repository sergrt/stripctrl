#include "stdafx.h"
#include "CaptureStrategy.h"

const std::vector<unsigned char>& CaptureStrategy::data() const {
    return data_;
}