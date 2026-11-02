#include "BrushTool.h"

#include <QPainter>
#include <QRadialGradient>

#include <algorithm>
#include <cmath>


BrushTool::BrushTool()
{
}


// ============================================================
// SETTINGS
// ============================================================

void BrushTool::setSize(
    double size)
{
    m_size =
        std::clamp(
            size,
            1.0,
            2000.0
        );
}


void BrushTool::setOpacity(
    double opacity)
{
    m_opacity =
        std::clamp(
            opacity,
            0.0,
            1.0
        );
}


void BrushTool::setHardness(
    double hardness)
{
    m_hardness =
        std::clamp(
            hardness,
            0.0,
            1.0
        );
}


void BrushTool::setFlow(
    double flow)
{
    m_flow =
        std::clamp(
            flow,
            0.0,
            1.0
        );
}


void BrushTool::setSpacing(
    double spacing)
{
    m_spacing =
        std::clamp(
            spacing,
            0.01,
            1.0
        );
}


void BrushTool::setColor(
    const QColor& color)
{
    m_color =
        color;
}

void BrushTool::setClipRegion(const QRegion& region)
{
    m_clipRegion = region;
}


// ============================================================
// ERASER
// ============================================================

void BrushTool::setEraser(
    bool enabled)
{
    m_eraser =
        enabled;
}


bool BrushTool::isEraser() const
{
    return m_eraser;
}


// ============================================================
// GETTERS
// ============================================================

double BrushTool::size() const
{
    return m_size;
}


double BrushTool::opacity() const
{
    return m_opacity;
}


double BrushTool::hardness() const
{
    return m_hardness;
}


double BrushTool::flow() const
{
    return m_flow;
}


double BrushTool::spacing() const
{
    return m_spacing;
}


QColor BrushTool::color() const
{
    return m_color;
}


// ============================================================
// STROKE
// ============================================================

void BrushTool::beginStroke(
    QImage& image,
    const QPointF& position)
{
    m_drawing =
        true;

    m_lastPosition =
        position;

    m_distanceSinceLastDab =
        0.0;

    drawPoint(
        image,
        position
    );
}


void BrushTool::continueStroke(
    QImage& image,
    const QPointF& position)
{
    if (!m_drawing)
        return;

    drawLine(
        image,
        m_lastPosition,
        position
    );

    m_lastPosition =
        position;
}


void BrushTool::endStroke()
{
    m_drawing =
        false;

    m_distanceSinceLastDab =
        0.0;
}


// ============================================================
// POINT
// ============================================================

void BrushTool::drawPoint(
    QImage& image,
    const QPointF& position)
{
    if (m_eraser)
    {
        eraseBrushDab(
            image,
            position
        );
    }
    else
    {
        drawBrushDab(
            image,
            position
        );
    }
}


// ============================================================
// LINE
// ============================================================

void BrushTool::drawLine(
    QImage& image,
    const QPointF& from,
    const QPointF& to)
{
    const double dx =
        to.x() -
        from.x();

    const double dy =
        to.y() -
        from.y();

    const double distance =
        std::hypot(
            dx,
            dy
        );

    if (distance <= 0.0)
        return;


    const double spacingDistance =
        std::max(
            1.0,
            m_size *
            m_spacing *
            0.5
        );


    const int steps =
        std::max(
            1,
            static_cast<int>(
                std::ceil(
                    distance /
                    spacingDistance
                )
                )
        );


    for (
        int i = 1;
        i <= steps;
        ++i
        )
    {
        const double t =
            static_cast<double>(i) /
            static_cast<double>(steps);

        const QPointF position =
            from +
            (
                to - from
                ) * t;

        drawPoint(
            image,
            position
        );
    }
}


// ============================================================
// NORMAL BRUSH
// ============================================================

void BrushTool::drawBrushDab(
    QImage& image,
    const QPointF& position)
{
    if (image.isNull())
        return;


    const double radius =
        m_size / 2.0;

    if (radius <= 0.0)
        return;


    const double effectiveOpacity =
        m_opacity *
        m_flow;


    QColor color =
        m_color;

    color.setAlphaF(
        effectiveOpacity
    );


    const double hardRadius =
        radius *
        m_hardness;


    QRadialGradient gradient(
        position,
        radius
    );


    if (m_hardness >= 0.999)
    {
        gradient.setColorAt(
            0.0,
            color
        );

        gradient.setColorAt(
            0.999,
            color
        );

        QColor transparent =
            color;

        transparent.setAlpha(
            0
        );

        gradient.setColorAt(
            1.0,
            transparent
        );
    }
    else
    {
        const double hardStop =
            std::clamp(
                hardRadius /
                radius,
                0.0,
                0.999
            );


        gradient.setColorAt(
            0.0,
            color
        );

        gradient.setColorAt(
            hardStop,
            color
        );

        QColor transparent =
            color;

        transparent.setAlpha(
            0
        );

        gradient.setColorAt(
            1.0,
            transparent
        );
    }


    QPainter painter(
        &image
    );

    painter.setRenderHint(
        QPainter::Antialiasing,
        true
    );

    if (!m_clipRegion.isEmpty())
        painter.setClipRegion(m_clipRegion);

    painter.setPen(
        Qt::NoPen
    );

    painter.setBrush(
        QBrush(
            gradient
        )
    );

    painter.drawEllipse(
        position,
        radius,
        radius
    );
}


// ============================================================
// ERASER
// ============================================================

void BrushTool::eraseBrushDab(
    QImage& image,
    const QPointF& position)
{
    if (image.isNull())
        return;


    const double radius =
        m_size / 2.0;

    if (radius <= 0.0)
        return;


    const double effectiveOpacity =
        m_opacity *
        m_flow;


    if (effectiveOpacity <= 0.0)
        return;


    QColor strengthColor(0, 0, 0, qRound(effectiveOpacity * 255.0));
    QRadialGradient gradient(position, radius);
    gradient.setColorAt(0.0, strengthColor);
    gradient.setColorAt(std::clamp(m_hardness, 0.0, 0.999), strengthColor);
    gradient.setColorAt(1.0, QColor(0, 0, 0, 0));

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (!m_clipRegion.isEmpty())
        painter.setClipRegion(m_clipRegion);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(position, radius, radius);
}
