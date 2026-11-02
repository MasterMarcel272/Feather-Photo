#include "Layer.h"

#include <QtGlobal>

Layer::Layer(const QString& name, int width, int height)
    : m_name(name),
    m_image(width, height, QImage::Format_RGBA8888)
{
    m_image.fill(Qt::transparent);
}

QString Layer::name() const
{
    return m_name;
}

void Layer::setName(const QString& name)
{
    m_name = name;
}

QImage& Layer::image()
{
    return m_image;
}

const QImage& Layer::image() const
{
    return m_image;
}

bool Layer::isVisible() const
{
    return m_visible;
}

void Layer::setVisible(bool visible)
{
    m_visible = visible;
}

float Layer::opacity() const
{
    return m_opacity;
}

void Layer::setOpacity(float opacity)
{
    m_opacity = qBound(0.0f, opacity, 1.0f);
}