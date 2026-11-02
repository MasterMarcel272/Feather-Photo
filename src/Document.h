#pragma once

#include "Layer.h"

#include <QList>

class Document
{
public:
    Document(int width, int height);
    ~Document();

    int width() const;
    int height() const;
    void setSize(int width, int height);

    const QList<Layer*>& layers() const;

    Layer* addLayer(const QString& name);
    Layer* insertLayer(int index, const QString& name);
    void removeLayer(int index);
    void moveLayer(int fromIndex, int toIndex);

    Layer* activeLayer();
    void setActiveLayer(int index);

    int activeLayerIndex() const;

private:
    int m_width;
    int m_height;

    QList<Layer*> m_layers;

    int m_activeLayer = -1;
};
