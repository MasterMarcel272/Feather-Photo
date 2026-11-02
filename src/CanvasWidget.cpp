#include "CanvasWidget.h"

#include "Layer.h"

#include <QEnterEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>
#include <deque>
#include <QByteArray>
#include <QVector>


CanvasWidget::CanvasWidget(
    QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(
        400,
        300
    );

    setFocusPolicy(
        Qt::StrongFocus
    );

    setMouseTracking(
        true
    );

    m_brush.setSize(
        40.0
    );

    m_brush.setOpacity(
        1.0
    );

    m_brush.setHardness(
        1.0
    );

    m_brush.setFlow(
        1.0
    );

    m_brush.setSpacing(
        0.15
    );

    m_brush.setColor(
        Qt::black
    );

    m_brush.setEraser(
        false
    );

    m_pickerEnabled = false;
}


// ============================================================
// IMAGE
// ============================================================

void CanvasWidget::setImage(
    const QImage& image)
{
    m_image = image;

    updateToolCursor();
    update();
}

void CanvasWidget::setImageRegion(const QRect& imageRect, const QImage& region)
{
    if (m_image.isNull() || region.isNull())
        return;

    const QRect clipped = imageRect.intersected(m_image.rect());
    if (clipped.isEmpty())
        return;

    QPainter painter(&m_image);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(clipped.topLeft(), region, QRect(QPoint(0, 0), clipped.size()));
    painter.end();

    const QRectF canvasRect = calculateImageRect();
    const qreal scaleX = canvasRect.width() / m_image.width();
    const qreal scaleY = canvasRect.height() / m_image.height();
    const QRect dirtyWidgetRect = QRectF(
        canvasRect.left() + clipped.left() * scaleX,
        canvasRect.top() + clipped.top() * scaleY,
        clipped.width() * scaleX,
        clipped.height() * scaleY).adjusted(-2, -2, 2, 2).toAlignedRect();
    update(dirtyWidgetRect);
}


void CanvasWidget::clearImage()
{
    m_image = QImage();

    unsetCursor();

    update();
}


void CanvasWidget::setActiveLayer(
    Layer* layer)
{
    m_activeLayer = layer;
    updateToolCursor();
    update();
}


// ============================================================
// ZOOM
// ============================================================

double CanvasWidget::zoom() const
{
    return m_zoom;
}


void CanvasWidget::setZoom(
    double zoom)
{
    m_zoom =
        std::clamp(
            zoom,
            0.05,
            16.0
        );

    updateToolCursor();
    update();
}


void CanvasWidget::resetZoom()
{
    m_zoom = 1.0;

    m_panOffset =
        QPointF(
            0.0,
            0.0
        );

    updateToolCursor();
    update();
}


void CanvasWidget::fitToWindow()
{
    if (m_image.isNull())
        return;

    const double availableWidth =
        static_cast<double>(
            width()
            ) - 40.0;

    const double availableHeight =
        static_cast<double>(
            height()
            ) - 40.0;

    if (
        availableWidth <= 0.0 ||
        availableHeight <= 0.0
        )
    {
        return;
    }

    const double zoomX =
        availableWidth /
        static_cast<double>(
            m_image.width()
            );

    const double zoomY =
        availableHeight /
        static_cast<double>(
            m_image.height()
            );

    m_zoom =
        std::min(
            zoomX,
            zoomY
        );

    m_zoom =
        std::clamp(
            m_zoom,
            0.05,
            16.0
        );

    m_panOffset =
        QPointF(
            0.0,
            0.0
        );

    updateToolCursor();
    update();
}


// ============================================================
// BRUSH SETTINGS
// ============================================================

void CanvasWidget::setBrushSize(
    double size)
{
    m_brush.setSize(
        size
    );

    updateToolCursor();
    update();
}


void CanvasWidget::setBrushOpacity(
    double opacity)
{
    m_brush.setOpacity(
        opacity
    );

    update();
}


void CanvasWidget::setBrushHardness(
    double hardness)
{
    m_brush.setHardness(
        hardness
    );

    update();
}


void CanvasWidget::setBrushFlow(
    double flow)
{
    m_brush.setFlow(
        flow
    );

    update();
}


void CanvasWidget::setBrushColor(
    const QColor& color)
{
    m_brush.setColor(
        color
    );

    emit brushColorChanged(
        color
    );

    update();
}

QColor CanvasWidget::brushColor() const
{
    return m_brush.color();
}


void CanvasWidget::setPickerEnabled(
    bool enabled)
{
    if (enabled)
        setActiveTool("Picker");
    else if (m_toolMode == ToolMode::Picker)
        setActiveTool("Brush");
}


bool CanvasWidget::pickerEnabled() const
{
    return m_pickerEnabled;
}


QString CanvasWidget::activeTool() const
{
    return m_activeToolName;
}

void CanvasWidget::selectAll()
{
    if (m_image.isNull()) return;
    m_selection = QRegion(m_image.rect());
    m_hasSelection = true;
    update();
}

void CanvasWidget::deselect()
{
    m_selection = QRegion();
    m_hasSelection = false;
    update();
}

void CanvasWidget::invertSelection()
{
    if (m_image.isNull()) return;
    const QRegion bounds(m_image.rect());
    m_selection = m_hasSelection ? bounds.subtracted(m_selection) : bounds;
    m_hasSelection = true;
    update();
}

bool CanvasWidget::hasSelection() const
{
    return m_hasSelection && !m_selection.isEmpty();
}

QRegion CanvasWidget::selectionRegion() const
{
    return m_selection;
}

void CanvasWidget::setSelectionRegion(const QRegion& region)
{
    m_selection = m_image.isNull() ? region : region.intersected(QRegion(m_image.rect()));
    m_hasSelection = !m_selection.isEmpty();
    update();
}

QRect CanvasWidget::strokeDirtyRect(const QPointF& from, const QPointF& to) const
{
    const qreal padding = std::ceil(std::max(1.0, m_brush.size() * 0.5)) + 2.0;
    return QRectF(from, to).normalized()
        .adjusted(-padding, -padding, padding, padding)
        .toAlignedRect()
        .intersected(m_activeLayer ? m_activeLayer->image().rect() : QRect());
}

QRect CanvasWidget::imageRectToWidgetRect(const QRect& imageRect) const
{
    if (m_image.isNull() || imageRect.isEmpty())
        return QRect();
    const QRectF canvasRect = calculateImageRect();
    const qreal scaleX = canvasRect.width() / m_image.width();
    const qreal scaleY = canvasRect.height() / m_image.height();
    return QRectF(
        canvasRect.left() + imageRect.left() * scaleX,
        canvasRect.top() + imageRect.top() * scaleY,
        imageRect.width() * scaleX,
        imageRect.height() * scaleY).adjusted(-3, -3, 3, 3).toAlignedRect();
}


void CanvasWidget::setActiveTool(const QString& toolName)
{
    const QString normalized = toolName.trimmed().toLower();
    ToolMode mode;
    QString canonical;

    if (normalized == "brush") { mode = ToolMode::Brush; canonical = "Brush"; }
    else if (normalized == "eraser") { mode = ToolMode::Eraser; canonical = "Eraser"; }
    else if (normalized == "pencil") { mode = ToolMode::Pencil; canonical = "Pencil"; }
    else if (normalized == "line") { mode = ToolMode::Line; canonical = "Line"; }
    else if (normalized == "rect" || normalized == "rectangle") { mode = ToolMode::Rectangle; canonical = "Rectangle"; }
    else if (normalized == "ellipse") { mode = ToolMode::Ellipse; canonical = "Ellipse"; }
    else if (normalized == "fill") { mode = ToolMode::Fill; canonical = "Fill"; }
    else if (normalized == "picker") { mode = ToolMode::Picker; canonical = "Picker"; }
    else if (normalized == "move") { mode = ToolMode::Move; canonical = "Move"; }
    else if (normalized == "crop") { mode = ToolMode::Crop; canonical = "Crop"; }
    else if (normalized == "select" || normalized == "rect select" || normalized == "rectangular select") { mode = ToolMode::Select; canonical = "Select"; }
    else return;

    m_toolMode = mode;
    m_activeToolName = canonical;
    m_pickerEnabled = (mode == ToolMode::Picker);
    m_brush.setEraser(mode == ToolMode::Eraser);
    m_toolDragging = false;
    m_brushDrawing = false;
    m_moveOriginal = QImage();
    m_moveSelectionOriginal = QRegion();
    m_moveLastOffset = QPoint();
    m_moveWithSelection = false;
    updateToolCursor();
    update();
    emit toolChanged(canonical);
}


// ============================================================
// TOOL
// ============================================================

void CanvasWidget::setEraserEnabled(
    bool enabled)
{
    if (enabled)
        setActiveTool("Eraser");
    else if (m_toolMode == ToolMode::Eraser)
        setActiveTool("Brush");
}


bool CanvasWidget::eraserEnabled() const
{
    return m_brush.isEraser();
}


// ============================================================
// TOOL CURSOR
// ============================================================

void CanvasWidget::updateToolCursor()
{
    if (
        m_spacePressed ||
        m_panning
        )
    {
        return;
    }

    if (m_image.isNull())
    {
        unsetCursor();
        return;
    }

    /*
     * Picker używa prostego kursora systemowego.
     * Nie pokazujemy tutaj dużego okręgu pędzla.
     */

    if (m_pickerEnabled || m_toolMode == ToolMode::Line || m_toolMode == ToolMode::Rectangle || m_toolMode == ToolMode::Ellipse || m_toolMode == ToolMode::Fill || m_toolMode == ToolMode::Crop || m_toolMode == ToolMode::Select)
    {
        setCursor(
            Qt::CrossCursor
        );

        return;
    }

    // The brush outline is drawn on the canvas; hide the system cursor so
    // the user sees a single, correctly sized brush preview.
    if (m_toolMode == ToolMode::Brush || m_toolMode == ToolMode::Eraser || m_toolMode == ToolMode::Pencil)
    {
        setCursor(Qt::BlankCursor);
        return;
    }

    /*
     * Kursor Qt ma być mały i czytelny.
     *
     * Nie próbujemy tutaj tworzyć ogromnego pixmapowego
     * kursora dla każdego rozmiaru pędzla.
     *
     * Właściwy okrąg rozmiaru nadal pokazuje preview
     * rysowany w CanvasWidget.
     */

    QPixmap cursorPixmap(
        24,
        24
    );

    cursorPixmap.fill(
        Qt::transparent
    );

    QPainter painter(
        &cursorPixmap
    );

    painter.setRenderHint(
        QPainter::Antialiasing,
        true
    );

    QColor outline =
        m_brush.isEraser()
        ? QColor(
            255,
            255,
            255
        )
        : QColor(
            255,
            255,
            255
        );

    painter.setPen(
        QPen(
            outline,
            1.5
        )
    );

    painter.setBrush(
        Qt::NoBrush
    );

    painter.drawEllipse(
        QRectF(
            4.0,
            4.0,
            16.0,
            16.0
        )
    );

    painter.setPen(
        QPen(
            QColor(
                0,
                0,
                0
            ),
            1.0
        )
    );

    painter.drawEllipse(
        QRectF(
            5.0,
            5.0,
            14.0,
            14.0
        )
    );

    painter.end();

    setCursor(
        QCursor(
            cursorPixmap,
            12,
            12
        )
    );
}


// ============================================================
// IMAGE RECT
// ============================================================

QRectF CanvasWidget::calculateImageRect() const
{
    if (m_image.isNull())
        return QRectF();

    const double imageWidth =
        m_image.width() *
        m_zoom;

    const double imageHeight =
        m_image.height() *
        m_zoom;

    const double x =
        (
            static_cast<double>(
                width()
                ) -
            imageWidth
            ) / 2.0
        +
        m_panOffset.x();

    const double y =
        (
            static_cast<double>(
                height()
                ) -
            imageHeight
            ) / 2.0
        +
        m_panOffset.y();

    return QRectF(
        x,
        y,
        imageWidth,
        imageHeight
    );
}


// ============================================================
// WIDGET → IMAGE
// ============================================================

QPointF CanvasWidget::widgetToImage(
    const QPointF& position) const
{
    const QRectF imageRect =
        calculateImageRect();

    if (imageRect.isEmpty())
        return QPointF();

    const double x =
        (
            position.x() -
            imageRect.left()
            ) /
        imageRect.width();

    const double y =
        (
            position.y() -
            imageRect.top()
            ) /
        imageRect.height();

    return QPointF(
        x *
        static_cast<double>(
            m_image.width()
            ),

        y *
        static_cast<double>(
            m_image.height()
            )
    );
}


// ============================================================
// CHECKERBOARD
// ============================================================

void CanvasWidget::drawCheckerboard(
    QPainter& painter,
    const QRectF& rect)
{
    const int tileSize = 16;

    painter.save();

    painter.setClipRect(rect);

    for (
        int y = static_cast<int>(rect.top());
        y < static_cast<int>(rect.bottom());
        y += tileSize
        )
    {
        for (
            int x = static_cast<int>(rect.left());
            x < static_cast<int>(rect.right());
            x += tileSize
            )
        {
            const bool even =
                ((x / tileSize) + (y / tileSize)) % 2 == 0;

            painter.fillRect(
                QRect(
                    x,
                    y,
                    tileSize,
                    tileSize
                ),
                even
                ? QColor(220, 220, 220)
                : QColor(190, 190, 190)
            );
        }
    }

    painter.restore();
}


// ============================================================
// BRUSH PREVIEW
// ============================================================

void CanvasWidget::drawBrushPreview(
    QPainter& painter)
{
    if (!m_showBrushPreview)
        return;

    if (m_toolDragging && (m_toolMode == ToolMode::Line || m_toolMode == ToolMode::Rectangle || m_toolMode == ToolMode::Ellipse || m_toolMode == ToolMode::Crop))
    {
        const QRectF imageRect = calculateImageRect();
        const QPointF start(imageRect.left() + m_toolStart.x() * m_zoom, imageRect.top() + m_toolStart.y() * m_zoom);
        const QPointF current(imageRect.left() + m_toolCurrent.x() * m_zoom, imageRect.top() + m_toolCurrent.y() * m_zoom);
        painter.save();
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(m_toolMode == ToolMode::Crop ? QColor(80, 170, 255) : m_brush.color(), 1.5, Qt::DashLine));
        if (m_toolMode == ToolMode::Line) painter.drawLine(start, current);
        else if (m_toolMode == ToolMode::Rectangle) painter.drawRect(QRectF(start, current).normalized());
        else if (m_toolMode == ToolMode::Ellipse) painter.drawEllipse(QRectF(start, current).normalized());
        else painter.drawRect(QRectF(start, current).normalized());
        painter.restore();
        return;
    }

    if (m_toolMode == ToolMode::Line || m_toolMode == ToolMode::Rectangle || m_toolMode == ToolMode::Ellipse || m_toolMode == ToolMode::Fill || m_toolMode == ToolMode::Move || m_toolMode == ToolMode::Crop || m_toolMode == ToolMode::Select)
        return;

    if (m_pickerEnabled)
        return;

    if (m_image.isNull())
        return;

    if (m_brushDrawing)
        return;

    const QRectF imageRect =
        calculateImageRect();

    if (
        !imageRect.contains(
            m_brushPreviewPosition
        )
        )
    {
        return;
    }

    const double brushDiameter =
        m_brush.size() *
        m_zoom;

    const double radius =
        brushDiameter /
        2.0;

    if (radius <= 0.0)
        return;

    painter.save();

    painter.setBrush(
        Qt::NoBrush
    );

    // Czarne obramowanie
    painter.setPen(
        QPen(
            QColor(
                0,
                0,
                0,
                220
            ),
            2.0
        )
    );

    painter.drawEllipse(
        m_brushPreviewPosition,
        radius,
        radius
    );

    // Białe wewnętrzne obramowanie
    painter.setPen(
        QPen(
            QColor(
                255,
                255,
                255,
                220
            ),
            1.0
        )
    );

    if (radius > 2.0)
    {
        painter.drawEllipse(
            m_brushPreviewPosition,
            radius - 2.0,
            radius - 2.0
        );
    }

    painter.restore();
}

void CanvasWidget::drawSelectionOverlay(QPainter& painter)
{
    if (m_image.isNull()) return;
    const QRectF imageRect = calculateImageRect();
    painter.save();
    painter.setBrush(QColor(60, 130, 255, 28));
    painter.setPen(QPen(QColor(15, 15, 15), 2.0, Qt::DashLine));

    if (m_toolDragging && m_toolMode == ToolMode::Select)
    {
        const QRectF rect = QRectF(m_toolStart, m_toolCurrent).normalized();
        const QRectF screen(imageRect.left() + rect.x() * m_zoom,
                            imageRect.top() + rect.y() * m_zoom,
                            rect.width() * m_zoom,
                            rect.height() * m_zoom);
        painter.drawRect(screen);
    }
    else if (m_hasSelection)
    {
        painter.setPen(QPen(QColor(15, 15, 15), 2.0, Qt::DashLine));
        for (const QRect& rect : m_selection)
        {
            const QRectF screen(imageRect.left() + rect.x() * m_zoom,
                                imageRect.top() + rect.y() * m_zoom,
                                rect.width() * m_zoom,
                                rect.height() * m_zoom);
            painter.drawRect(screen);
            painter.setPen(QPen(Qt::white, 1.0, Qt::DashLine));
            painter.drawRect(screen);
            painter.setPen(QPen(QColor(15, 15, 15), 2.0, Qt::DashLine));
        }
    }
    painter.restore();
}


// ============================================================
// PAINT
// ============================================================

void CanvasWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(
        this
    );

    painter.setRenderHint(
        QPainter::Antialiasing,
        true
    );

    painter.fillRect(
        rect(),
        QColor(
            35,
            35,
            35
        )
    );

    if (m_image.isNull())
        return;

    const QRectF imageRect =
        calculateImageRect();

    drawCheckerboard(
        painter,
        imageRect
    );

    painter.drawImage(
        imageRect,
        m_image
    );

    painter.setPen(
        QPen(
            QColor(
                100,
                100,
                100
            ),
            1
        )
    );

    painter.setBrush(
        Qt::NoBrush
    );

    painter.drawRect(
        imageRect
    );

    drawBrushPreview(
        painter
    );

    drawSelectionOverlay(painter);
}


// ============================================================
// MOUSE WHEEL
// ============================================================

void CanvasWidget::wheelEvent(
    QWheelEvent* event)
{
    if (m_image.isNull())
        return;

    const QPointF mousePosition =
        event->position();

    const double oldZoom =
        m_zoom;

    const double zoomFactor =
        event->angleDelta().y() > 0
        ? 1.1
        : 0.9;

    const double newZoom =
        std::clamp(
            oldZoom *
            zoomFactor,
            0.05,
            16.0
        );

    if (
        std::abs(
            newZoom -
            oldZoom
        ) < 0.0001
        )
    {
        return;
    }

    const QRectF oldRect =
        calculateImageRect();

    const QPointF imagePosition =
        QPointF(
            (
                mousePosition.x() -
                oldRect.left()
                ) /
            oldRect.width(),

            (
                mousePosition.y() -
                oldRect.top()
                ) /
            oldRect.height()
        );

    m_zoom =
        newZoom;

    const QRectF newRect =
        calculateImageRect();

    const QPointF newMousePosition =
        QPointF(
            newRect.left() +
            imagePosition.x() *
            newRect.width(),

            newRect.top() +
            imagePosition.y() *
            newRect.height()
        );

    m_panOffset +=
        mousePosition -
        newMousePosition;

    updateToolCursor();
    update();

    event->accept();
}


// ============================================================
// MOUSE PRESS
// ============================================================

void CanvasWidget::mousePressEvent(QMouseEvent* event)
{
    setFocus();
    if (event->button() == Qt::MiddleButton || (event->button() == Qt::LeftButton && m_spacePressed))
    {
        m_panning = true;
        m_lastMousePosition = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    if (event->button() != Qt::LeftButton || !m_activeLayer)
        return;

    const QRectF imageRect = calculateImageRect();
    if (!imageRect.contains(event->position()))
        return;
    const QPointF imagePosition = widgetToImage(event->position());

    if (m_toolMode == ToolMode::Picker)
    {
        const int x = static_cast<int>(std::floor(imagePosition.x()));
        const int y = static_cast<int>(std::floor(imagePosition.y()));
        if (x >= 0 && y >= 0 && x < m_activeLayer->image().width() && y < m_activeLayer->image().height())
        {
            setBrushColor(m_activeLayer->image().pixelColor(x, y));
            setActiveTool("Brush");
        }
        event->accept();
        return;
    }

    if (m_toolMode == ToolMode::Select)
    {
        m_toolStart = imagePosition;
        m_toolCurrent = imagePosition;
        m_toolDragging = true;
        event->accept();
        update();
        return;
    }

    if (m_toolMode == ToolMode::Fill)
    {
        QImage& image = m_activeLayer->image();
        const int sx = static_cast<int>(std::floor(imagePosition.x()));
        const int sy = static_cast<int>(std::floor(imagePosition.y()));
        if (sx < 0 || sy < 0 || sx >= image.width() || sy >= image.height()) return;
        const QRgb source = image.pixel(sx, sy);
        QColor replacement = m_brush.color();
        replacement.setAlphaF(replacement.alphaF() * m_brush.opacity());
        const QRgb replacementPixel = replacement.rgba();
        if (source != replacementPixel)
        {
            emit editAboutToChange();
            const int width = image.width();
            const int height = image.height();
            QVector<QPoint> pending;
            pending.reserve(1024);
            pending.append(QPoint(sx, sy));
            while (!pending.isEmpty())
            {
                const QPoint seed = pending.takeLast();
                const int y = seed.y();
                int x = seed.x();
                if (y < 0 || y >= height || x < 0 || x >= width) continue;
                if (image.pixel(x, y) != source || (m_hasSelection && !m_selection.contains(seed))) continue;

                int left = x;
                while (left > 0 && image.pixel(left - 1, y) == source &&
                       (!m_hasSelection || m_selection.contains(QPoint(left - 1, y))))
                    --left;
                int right = x;
                while (right + 1 < width && image.pixel(right + 1, y) == source &&
                       (!m_hasSelection || m_selection.contains(QPoint(right + 1, y))))
                    ++right;

                for (int fillX = left; fillX <= right; ++fillX)
                    image.setPixel(fillX, y, replacementPixel);

                for (const int adjacentY : { y - 1, y + 1 })
                {
                    if (adjacentY < 0 || adjacentY >= height) continue;
                    int scanX = left;
                    while (scanX <= right)
                    {
                        const QPoint candidate(scanX, adjacentY);
                        if (image.pixel(scanX, adjacentY) == source &&
                            (!m_hasSelection || m_selection.contains(candidate)))
                        {
                            pending.append(candidate);
                            do { ++scanX; }
                            while (scanX <= right && image.pixel(scanX, adjacentY) == source &&
                                   (!m_hasSelection || m_selection.contains(QPoint(scanX, adjacentY))));
                        }
                        else
                        {
                            ++scanX;
                        }
                    }
                }
            }
            emit layerContentChanged();
            update();
        }
        event->accept();
        return;
    }

    m_toolStart = imagePosition;
    m_toolCurrent = imagePosition;
    m_lastToolPoint = imagePosition;
    m_toolDragging = true;
    if (m_toolMode == ToolMode::Brush || m_toolMode == ToolMode::Eraser ||
        m_toolMode == ToolMode::Pencil || m_toolMode == ToolMode::Move ||
        m_toolMode == ToolMode::Line || m_toolMode == ToolMode::Rectangle ||
        m_toolMode == ToolMode::Ellipse)
        emit editAboutToChange();
    if (m_toolMode == ToolMode::Brush || m_toolMode == ToolMode::Eraser)
    {
        m_brush.setClipRegion(m_hasSelection ? m_selection : QRegion());
        m_brush.beginStroke(m_activeLayer->image(), imagePosition);
        m_brushDrawing = true;
        emit layerContentChanged(strokeDirtyRect(imagePosition, imagePosition));
    }
    else if (m_toolMode == ToolMode::Pencil)
    {
        QPainter painter(&m_activeLayer->image());
        painter.setRenderHint(QPainter::Antialiasing, false);
        if (m_hasSelection) painter.setClipRegion(m_selection);
        QColor color = m_brush.color();
        color.setAlphaF(color.alphaF() * m_brush.opacity());
        QPen pen(color, std::max(1.0, m_brush.size()), Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
        painter.setPen(pen);
        painter.drawPoint(imagePosition);
    }
    else if (m_toolMode == ToolMode::Move)
    {
        m_moveOriginal = m_activeLayer->image().copy();
        m_moveWithSelection = m_hasSelection;
        m_moveSelectionOriginal = m_hasSelection ? m_selection : QRegion(m_activeLayer->image().rect());
        m_moveLastOffset = QPoint();
    }
    if (m_toolMode == ToolMode::Pencil)
        emit layerContentChanged(strokeDirtyRect(imagePosition, imagePosition));
    update();
    event->accept();
}
// ============================================================
// MOUSE MOVE
// ============================================================

void CanvasWidget::mouseMoveEvent(QMouseEvent* event)
{
    const QPointF oldPreviewPosition = m_brushPreviewPosition;
    m_brushPreviewPosition = event->position();
    m_showBrushPreview = true;
    if (m_panning)
    {
        const QPoint delta = event->pos() - m_lastMousePosition;
        m_panOffset += QPointF(delta.x(), delta.y());
        m_lastMousePosition = event->pos();
        update();
        event->accept();
        return;
    }
    if (m_toolDragging && m_activeLayer)
    {
        const QRectF imageRect = calculateImageRect();
        const QPointF current = widgetToImage(event->position());
        QRect dirtyWidgetRect;
        m_toolCurrent = current;
        if (m_toolMode == ToolMode::Brush || m_toolMode == ToolMode::Eraser)
        {
            if (imageRect.contains(event->position()))
                m_brush.continueStroke(m_activeLayer->image(), current);
            const QRect dirtyRect = strokeDirtyRect(m_lastToolPoint, current);
            emit layerContentChanged(dirtyRect);
            dirtyWidgetRect = imageRectToWidgetRect(dirtyRect);
            m_lastToolPoint = current;
        }
        else if (m_toolMode == ToolMode::Pencil)
        {
            const QPointF previous = m_lastToolPoint;
            QPainter painter(&m_activeLayer->image());
            painter.setRenderHint(QPainter::Antialiasing, false);
            if (m_hasSelection) painter.setClipRegion(m_selection);
            QColor color = m_brush.color();
            color.setAlphaF(color.alphaF() * m_brush.opacity());
            painter.setPen(QPen(color, std::max(1.0, m_brush.size()), Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
            painter.drawLine(m_lastToolPoint, current);
            m_lastToolPoint = current;
            painter.end();
            const QRect dirtyRect = strokeDirtyRect(previous, current);
            emit layerContentChanged(dirtyRect);
            dirtyWidgetRect = imageRectToWidgetRect(dirtyRect);
        }
        else if (m_toolMode == ToolMode::Move && !m_moveOriginal.isNull())
        {
            const QPoint offset(qRound(current.x() - m_toolStart.x()),
                                qRound(current.y() - m_toolStart.y()));
            const QRect sourceBounds = m_moveSelectionOriginal.boundingRect();
            const QRect dirtyRect = sourceBounds
                .united(sourceBounds.translated(m_moveLastOffset))
                .united(sourceBounds.translated(offset))
                .intersected(m_activeLayer->image().rect());
            const QRegion dirtyRegion(dirtyRect);
            QImage& target = m_activeLayer->image();
            QPainter painter(&target);
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            painter.setClipRect(dirtyRect);
            painter.drawImage(0, 0, m_moveOriginal);
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
            painter.setClipRegion(m_moveSelectionOriginal.intersected(dirtyRegion));
            painter.fillRect(dirtyRect, Qt::transparent);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.setClipRegion(m_moveSelectionOriginal.translated(offset).intersected(dirtyRegion));
            painter.drawImage(offset, m_moveOriginal);
            painter.end();
            if (m_moveWithSelection)
            {
                m_selection = m_moveSelectionOriginal.translated(offset)
                    .intersected(QRegion(target.rect()));
                m_hasSelection = true;
            }
            m_moveLastOffset = offset;
            emit layerContentChanged(dirtyRect);
            dirtyWidgetRect = imageRectToWidgetRect(dirtyRect);
        }
        if (m_toolMode == ToolMode::Select || m_toolMode == ToolMode::Line ||
            m_toolMode == ToolMode::Rectangle || m_toolMode == ToolMode::Ellipse || m_toolMode == ToolMode::Crop)
        {
            const QRectF start = QRectF(imageRect.topLeft() + m_toolStart * m_zoom,
                                        imageRect.topLeft() + m_toolCurrent * m_zoom);
            const QRectF previous = QRectF(imageRect.topLeft() + m_toolStart * m_zoom,
                                           imageRect.topLeft() + m_lastToolPoint * m_zoom);
            dirtyWidgetRect = start.united(previous).adjusted(-4, -4, 4, 4).toAlignedRect();
            m_lastToolPoint = current;
        }
        update(dirtyWidgetRect.isEmpty() ? rect() : dirtyWidgetRect);
        event->accept();
        return;
    }
    if (m_toolMode == ToolMode::Brush || m_toolMode == ToolMode::Eraser || m_toolMode == ToolMode::Pencil)
    {
        const qreal radius = std::max(1.0, m_brush.size() * m_zoom * 0.5) + 4.0;
        update(QRectF(oldPreviewPosition.x() - radius, oldPreviewPosition.y() - radius, radius * 2, radius * 2).toAlignedRect());
        update(QRectF(m_brushPreviewPosition.x() - radius, m_brushPreviewPosition.y() - radius, radius * 2, radius * 2).toAlignedRect());
    }
    QWidget::mouseMoveEvent(event);
}
// ============================================================
// MOUSE RELEASE
// ============================================================

void CanvasWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton)
    {
        if (m_brushDrawing)
        {
            m_brush.endStroke();
            m_brushDrawing = false;
        }
        if (m_toolDragging && m_activeLayer)
        {
            m_toolCurrent = widgetToImage(event->position());
            if (m_toolMode == ToolMode::Line || m_toolMode == ToolMode::Rectangle || m_toolMode == ToolMode::Ellipse)
            {
                QPainter painter(&m_activeLayer->image());
                if (m_hasSelection) painter.setClipRegion(m_selection);
                painter.setRenderHint(QPainter::Antialiasing, true);
                QColor color = m_brush.color();
                color.setAlphaF(color.alphaF() * m_brush.opacity());
                painter.setPen(QPen(color, std::max(1.0, m_brush.size()), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painter.setBrush(Qt::NoBrush);
                if (m_toolMode == ToolMode::Line) painter.drawLine(m_toolStart, m_toolCurrent);
                else if (m_toolMode == ToolMode::Rectangle) painter.drawRect(QRectF(m_toolStart, m_toolCurrent).normalized());
                else painter.drawEllipse(QRectF(m_toolStart, m_toolCurrent).normalized());
                painter.end();
                emit layerContentChanged();
            }
            else if (m_toolMode == ToolMode::Select)
            {
                const QRect selected = QRectF(m_toolStart, m_toolCurrent).normalized().toAlignedRect().intersected(m_activeLayer->image().rect());
                m_selection = selected.isEmpty() ? QRegion() : QRegion(selected);
                m_hasSelection = !selected.isEmpty();
            }
            else if (m_toolMode == ToolMode::Crop)
            {
                const QRect crop = QRect(QPoint(static_cast<int>(std::floor(std::min(m_toolStart.x(), m_toolCurrent.x()))), static_cast<int>(std::floor(std::min(m_toolStart.y(), m_toolCurrent.y())))), QPoint(static_cast<int>(std::ceil(std::max(m_toolStart.x(), m_toolCurrent.x()))) - 1, static_cast<int>(std::ceil(std::max(m_toolStart.y(), m_toolCurrent.y()))) - 1)).intersected(m_activeLayer->image().rect());
                if (crop.width() > 1 && crop.height() > 1) emit cropRequested(crop);
            }
            if (m_toolMode == ToolMode::Move && m_moveWithSelection)
                m_hasSelection = !m_selection.isEmpty();
            m_toolDragging = false;
            m_moveOriginal = QImage();
            m_moveSelectionOriginal = QRegion();
            m_moveLastOffset = QPoint();
            m_moveWithSelection = false;
        }
        m_panning = false;
        updateToolCursor();
        update();
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}
// ============================================================
// KEY PRESS
// ============================================================

void CanvasWidget::keyPressEvent(
    QKeyEvent* event)
{
    if (
        event->key() ==
        Qt::Key_Space
        )
    {
        m_spacePressed =
            true;

        setCursor(
            Qt::OpenHandCursor
        );

        event->accept();

        return;
    }


    // --------------------------------------------------------
    // E = ERASER
    // --------------------------------------------------------

    if (
        event->key() ==
        Qt::Key_E

        &&

        !event->isAutoRepeat()
        )
    {
        setEraserEnabled(
            !eraserEnabled()
        );

        event->accept();

        return;
    }


    // --------------------------------------------------------
// P = COLOR PICKER
// --------------------------------------------------------

    if (
        event->key() ==
        Qt::Key_P

        &&

        !event->isAutoRepeat()
        )
    {
        setPickerEnabled(
            !pickerEnabled()
        );

        event->accept();

        return;
    }


    // --------------------------------------------------------
    // 1 = 100%
    // --------------------------------------------------------

    if (
        event->key() ==
        Qt::Key_1
        )
    {
        resetZoom();

        event->accept();

        return;
    }


    // --------------------------------------------------------
    // 2 = FIT
    // --------------------------------------------------------

    if (
        event->key() ==
        Qt::Key_2
        )
    {
        fitToWindow();

        event->accept();

        return;
    }

    QWidget::keyPressEvent(
        event
    );
}


// ============================================================
// KEY RELEASE
// ============================================================

void CanvasWidget::keyReleaseEvent(
    QKeyEvent* event)
{
    if (
        event->key() ==
        Qt::Key_Space
        )
    {
        m_spacePressed =
            false;

        updateToolCursor();

        event->accept();

        return;
    }

    QWidget::keyReleaseEvent(
        event
    );
}


// ============================================================
// ENTER
// ============================================================

void CanvasWidget::enterEvent(
    QEnterEvent* event)
{
    Q_UNUSED(event);

    m_showBrushPreview =
        true;

    updateToolCursor();
    update();
}


// ============================================================
// LEAVE
// ============================================================

void CanvasWidget::leaveEvent(
    QEvent* event)
{
    Q_UNUSED(event);

    m_showBrushPreview =
        false;

    unsetCursor();

    update();
}


// ============================================================
// RESIZE
// ============================================================

void CanvasWidget::resizeEvent(
    QResizeEvent* event)
{
    QWidget::resizeEvent(
        event
    );

    updateToolCursor();
    update();
}
