#pragma once

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <QPointF>
#include <QColor>
#include <QRect>
#include <QString>
#include <QRegion>

#include "BrushTool.h"

class Layer;

class CanvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget* parent = nullptr);

    void setImage(const QImage& image);
    void setImageRegion(const QRect& imageRect, const QImage& region);
    void clearImage();

    void setActiveLayer(Layer* layer);

    double zoom() const;

    void setZoom(double zoom);
    void resetZoom();
    void fitToWindow();

    // Brush settings
    void setBrushSize(double size);
    void setBrushOpacity(double opacity);
    void setBrushHardness(double hardness);
    void setBrushFlow(double flow);
    void setBrushColor(const QColor& color);
    QColor brushColor() const;

    // Tool
    void setEraserEnabled(bool enabled);
    bool eraserEnabled() const;

    void setPickerEnabled(bool enabled);
    bool pickerEnabled() const;
    void setActiveTool(const QString& toolName);
    QString activeTool() const;
    void selectAll();
    void deselect();
    void invertSelection();
    bool hasSelection() const;
    QRegion selectionRegion() const;
    void setSelectionRegion(const QRegion& region);

signals:
    void brushColorChanged(const QColor& color);
    void toolChanged(const QString& toolName);
    void cropRequested(const QRect& imageRect);
    void layerContentChanged(const QRect& dirtyRect = QRect());
    void editAboutToChange();

protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void drawCheckerboard(QPainter& painter, const QRectF& rect);
    void drawBrushPreview(QPainter& painter);
    void drawSelectionOverlay(QPainter& painter);
    QRect strokeDirtyRect(const QPointF& from, const QPointF& to) const;
    QRect imageRectToWidgetRect(const QRect& imageRect) const;
    void updateToolCursor();

    QRectF calculateImageRect() const;
    QPointF widgetToImage(const QPointF& position) const;

    QImage m_image;

    Layer* m_activeLayer = nullptr;

    BrushTool m_brush;

    double m_zoom = 1.0;

    QPointF m_panOffset;

    bool m_panning = false;
    bool m_spacePressed = false;
    bool m_brushDrawing = false;
    bool m_showBrushPreview = false;
    bool m_pickerEnabled = false;

    enum class ToolMode { Brush, Eraser, Pencil, Line, Rectangle, Ellipse, Fill, Picker, Move, Crop, Select };
    ToolMode m_toolMode = ToolMode::Brush;
    QPointF m_toolStart;
    QPointF m_toolCurrent;
    QPointF m_lastToolPoint;
    bool m_toolDragging = false;
    QImage m_moveOriginal;
    QRegion m_moveSelectionOriginal;
    QPoint m_moveLastOffset;
    bool m_moveWithSelection = false;
    QString m_activeToolName = "Brush";
    QRegion m_selection;
    bool m_hasSelection = false;

    QPointF m_brushPreviewPosition;

    QPoint m_lastMousePosition;
};
