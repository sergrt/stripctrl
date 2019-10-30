#pragma once
#include "LedColors.h"

#include <QWidget>
#include <utility>

class ColorPreviewWidget : public QWidget {
public:
    ColorPreviewWidget(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent* event) override;
    void setPreviewData(LedColors colors, double fps);

private:
    LedColors colors_;
    double fps_ = 0.0;

    using HorSegmentSize = QSize;
    using VertSegmentSize = QSize;
    using SegmentsSize = std::pair<HorSegmentSize, VertSegmentSize>;
    SegmentsSize calcSegmentsSize() const;
    void resizeToSegmentsSize(const SegmentsSize& segments_size);
};
