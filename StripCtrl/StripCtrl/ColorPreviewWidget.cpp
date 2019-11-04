#include "stdafx.h"
#include "ColorPreviewWidget.h"

#include <QPainter>
#include <QBrush>

namespace {
    const double hor_segment_height = 0.2;
    const double vert_segment_width = 0.1;
}

ColorPreviewWidget::ColorPreviewWidget(QWidget* parent)
    : QWidget(parent) {
}

ColorPreviewWidget::SegmentsSize ColorPreviewWidget::calcSegmentsSize() const {
    const auto w = width();
    const auto h = height();
    const int segment_w = w / colors_.top_strip.size();
    const int segment_h = (h - 2 * h * hor_segment_height) / colors_.left_strip.size();

    return std::make_pair(QSize(segment_w, h * hor_segment_height),
                          QSize(w * vert_segment_width, segment_h));
}

void ColorPreviewWidget::resizeToSegmentsSize(const SegmentsSize& segments_size) {
    const auto hor_margin = (width() - segments_size.first.width() * colors_.top_strip.size()) / 2;
    const auto vert_margin = (segments_size.first.height() * 2 + segments_size.second.height() * colors_.left_strip.size()) / 2;
    const QMargins margins(hor_margin, vert_margin, hor_margin, vert_margin);

    setContentsMargins(margins);
}

void ColorPreviewWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);

    if (colors_.empty())
        return;

    const auto segments_size = calcSegmentsSize();
    resizeToSegmentsSize(segments_size);

    QPainter painter(this);

    const HorSegmentSize& hor_segment_size = segments_size.first;
    const auto h = height();
    QBrush brush(Qt::SolidPattern);
    const auto hor_size = colors_.top_strip.size();
    for (size_t i = 0; i < hor_size; ++i) {
        const auto x = i * hor_segment_size.width();

        brush.setColor(colors_.top_strip[i]);
        painter.fillRect(x, 0, x + hor_segment_size.width(), hor_segment_size.height(), brush);

        brush.setColor(colors_.bottom_strip[hor_size - i - 1]);
        painter.fillRect(x, h - hor_segment_size.height(), x + hor_segment_size.width(), h, brush);
    }

    const VertSegmentSize& vert_segment_size = segments_size.second;
    const auto w = width();
    const auto vert_size = colors_.left_strip.size();
    for (size_t i = 0; i < vert_size; ++i) {
        const auto y = hor_segment_size.height() + i * vert_segment_size.height();

        brush.setColor(colors_.left_strip[vert_size - i - 1]);
        painter.fillRect(0, y, vert_segment_size.width(), y + vert_segment_size.height(), brush);

        brush.setColor(colors_.right_strip[i]);
        painter.fillRect(w - vert_segment_size.width(), y, w, y +vert_segment_size.height(), brush);
    }

    const QTextOption text_option(Qt::AlignCenter);
    const auto fps_string = QString("%1").arg(fps_, 0, 'f', 3, QChar('0'));
    painter.drawText(rect(), fps_string, text_option);
}

void ColorPreviewWidget::setPreviewData(LedColors colors, double fps) {
    colors_ = std::move(colors);
    fps_ = fps;
    update();
}
