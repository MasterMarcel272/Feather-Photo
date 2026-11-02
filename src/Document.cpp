#include "Document.h"

#include <algorithm>
#include <QtAlgorithms>

Document::Document(int width, int height)
    : m_width(width),
    m_height(height)
{
}

Document::~Document()
{
    qDeleteAll(m_layers);
}

int Document::width() const
{
    return m_width;
}

int Document::height() const
{
    return m_height;
}

void Document::setSize(int width, int height)
{
    if (width <= 0 || height <= 0)
        return;
    m_width = width;
    m_height = height;
}

const QList<Layer*>& Document::layers() const
{
    return m_layers;
}

Layer* Document::addLayer(const QString& name)
{
    return insertLayer(static_cast<int>(m_layers.size()), name);
}

Layer* Document::insertLayer(int index, const QString& name)
{
    index = std::clamp(index, 0, static_cast<int>(m_layers.size()));
    auto* layer = new Layer(name, m_width, m_height);
    m_layers.insert(index, layer);
    m_activeLayer = index;
    return layer;
}

void Document::removeLayer(int index)
{
    if (index < 0 || index >= m_layers.size())
        return;

    const int oldActive = m_activeLayer;
    delete m_layers[index];
    m_layers.removeAt(index);

    if (m_layers.isEmpty())
    {
        m_activeLayer = -1;
    }
    else if (oldActive > index)
        m_activeLayer = oldActive - 1;
    else if (oldActive == index)
        m_activeLayer = std::min(index, static_cast<int>(m_layers.size()) - 1);
    else
        m_activeLayer = oldActive;
}

void Document::moveLayer(int fromIndex, int toIndex)
{
    if (fromIndex < 0 || fromIndex >= m_layers.size() || toIndex < 0 || toIndex >= m_layers.size() || fromIndex == toIndex)
        return;
    Layer* active = activeLayer();
    m_layers.move(fromIndex, toIndex);
    m_activeLayer = m_layers.indexOf(active);
}

Layer* Document::activeLayer()
{
    if (m_activeLayer < 0 || m_activeLayer >= m_layers.size())
        return nullptr;

    return m_layers[m_activeLayer];
}

void Document::setActiveLayer(int index)
{
    if (index < 0 || index >= m_layers.size())
        return;

    m_activeLayer = index;
}

int Document::activeLayerIndex() const
{
    return m_activeLayer;
}
