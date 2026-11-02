#pragma once

#include <QColor>
#include <QImage>
#include <QPointF>
#include <QRegion>

class BrushTool
{
public:
    BrushTool();

    void setSize(double size);
    void setOpacity(double opacity);
    void setHardness(double hardness);
    void setFlow(double flow);
    void setSpacing(double spacing);
    void setColor(const QColor& color);
    void setClipRegion(const QRegion& region);

    double size() const;
    double opacity() const;
    double hardness() const;
    double flow() const;
    double spacing() const;

    QColor color() const;

    void setEraser(bool enabled);
    bool isEraser() const;

    void beginStroke(
        QImage& image,
        const QPointF& position
    );

    void continueStroke(
        QImage& image,
        const QPointF& position
    );

    void endStroke();

private:
    void drawPoint(
        QImage& image,
        const QPointF& position
    );

    void drawLine(
        QImage& image,
        const QPointF& from,
        const QPointF& to
    );

    void drawBrushDab(
        QImage& image,
        const QPointF& position
    );

    void eraseBrushDab(
        QImage& image,
        const QPointF& position
    );

    double m_size = 40.0;

    double m_opacity = 1.0;

    double m_hardness = 1.0;

    double m_flow = 1.0;

    double m_spacing = 0.15;

    QColor m_color = Qt::black;
    QRegion m_clipRegion;

    QPointF m_lastPosition;

    double m_distanceSinceLastDab = 0.0;

    bool m_drawing = false;

    bool m_eraser = false;
};
