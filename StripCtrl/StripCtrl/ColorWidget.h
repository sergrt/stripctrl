#pragma once
#include <vector>
#include <QWidget>
#include <QPainter>
#include "LedColors.h"

class ColorWidget : public QWidget {
public:
    ColorWidget(QWidget* parent = nullptr) : QWidget(parent) {}
    void paintEvent(QPaintEvent* event) override {
       
        QWidget::paintEvent(event);

        
        if (colors_.left_strip.empty())
            return;

        {
            int segment_w = width() / colors_.top_strip.size();
            int segment_h = (height() - 20 - 20) / colors_.left_strip.size();
            setFixedSize(segment_w * colors_.top_strip.size(),
                         40 + segment_h * colors_.left_strip.size());
        }

        QPainter p(this);

        int segment_w = width() / colors_.top_strip.size();

        for (int i = 0; i < colors_.top_strip.size(); ++i) {
            QBrush t(colors_.top_strip[i]);
            p.fillRect(i * segment_w, 0, segment_w, 20, t);

            QBrush b(colors_.bottom_strip[i]);
            p.fillRect(i * segment_w, height() - 20, segment_w, height(), b);
        }

        int segment_h = (height() - 20 - 20)/ colors_.left_strip.size();
        for (int i = 0; i < colors_.left_strip.size(); ++i) {
            QBrush t(colors_.left_strip[i]);
            p.fillRect(0, 20 + i * segment_h, segment_w, segment_h, t);

            QBrush b(colors_.right_strip[i]);
            p.fillRect(width() - segment_w, 20 + i * segment_h, segment_w, segment_h, b);
        }
        
    }
    void setColors(LedColors colors) {
        colors_ = std::move(colors);

        
        update();
    }
private:
    LedColors colors_;
};
