#pragma once

#include <QString>
#include <QImage>

class Layer
{
public:
    Layer(const QString& name, int width, int height);

    QString name() const;
    void setName(const QString& name);

    QImage& image();
    const QImage& image() const;

    bool isVisible() const;
    void setVisible(bool visible);

    float opacity() const;
    void setOpacity(float opacity);

private:
    QString m_name;
    QImage m_image;

    bool m_visible = true;
    float m_opacity = 1.0f;
};