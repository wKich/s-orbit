#ifndef IMAGERENDER_H
#define IMAGERENDER_H

#include <QObject>
#include <QColor>
#include <QImage>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

struct StaticPlanet {
    int index;
    float mass;
    QVector2D startPosition;
    double currentPositionX;    //for backport calculation
    double currentPositionY;    //for backport calculation
    QColor color;

    StaticPlanet(){}
    StaticPlanet(const int &id, const float &m, const QVector2D &pos, const QColor &c) :
        index(id),
        mass(m),
        startPosition(pos),
        color(c)
    {}
    StaticPlanet(const StaticPlanet &p) :
        index(p.index),
        mass(p.mass),
        startPosition(p.startPosition),
        color(p.color)
    {}
    virtual ~StaticPlanet(){}
};

struct DynamicPlanet : StaticPlanet {
    QVector<QVector2D> positions;
    QVector<unsigned char> samples;
    QVector2D startSpeed;
    double currentSpeedX;
    double currentSpeedY;

    DynamicPlanet() {}
    DynamicPlanet(const int &id, const float &m, const QVector2D &pos, const QVector2D &speed, const QColor &c) :
        StaticPlanet(id, m, pos, c),
        startSpeed(speed)
    {}
    DynamicPlanet(const DynamicPlanet &p) :
        StaticPlanet(p.index, p.mass, p.startPosition, p.color),
        startSpeed(p.startSpeed)
    {}
    ~DynamicPlanet(){}
};

class ImageRender : public QObject
{
    Q_OBJECT
public:
    explicit ImageRender(QObject *parent = 0);
    ~ImageRender();
    QImage getImage(const QVector2D& minBounder, const QVector2D& maxBounder,
                    const QList<StaticPlanet>& staticPlanets, const QList<DynamicPlanet>& dynamicPlanets);

signals:

public slots:

private:
    QOpenGLContext* m_context;
    QOffscreenSurface m_surface;
    QOpenGLFramebufferObject* m_fbo;
    QOpenGLShaderProgram* m_program;
    int m_vertexAttr;
    int m_colorUni;
    int m_matrixUni;

    void initialize();
};

#endif // IMAGERENDER_H