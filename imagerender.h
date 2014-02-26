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

#include "pointdouble2d.h"
#include "quadtree.h"

class QuadTree;

struct PositionSample {
    unsigned short count;
    QVector2D position;

    PositionSample(){}
    PositionSample(const unsigned short& c, const QVector2D& v) :
        count(c),
        position(v)
    {}
    PositionSample(const PositionSample& ps) :
        count(ps.count),
        position(ps.position)
    {}
};

struct StaticPlanet {
    int index;
    float mass;
    PointDouble2D position;
    QColor color;

    //barnes-hut
    QuadTree* leaf;

    StaticPlanet(){}
    StaticPlanet(const int &id, const float &m, const PointDouble2D &pos, const QColor &c) :
        index(id),
        mass(m),
        position(pos),
        color(c),
        leaf(nullptr)
    {}
    StaticPlanet(const int &id, const float &m, const PointDouble2D &pos, const QColor &c, QuadTree* l) :
        index(id),
        mass(m),
        position(pos),
        color(c),
        leaf(l)
    {}
    StaticPlanet(const StaticPlanet &p) :
        index(p.index),
        mass(p.mass),
        position(p.position),
        color(p.color),
        leaf(p.leaf)
    {}
    virtual ~StaticPlanet(){}
};

struct DynamicPlanet : StaticPlanet {
    QVector<PositionSample> samples;
    PointDouble2D speed;

    //barnes-hut
    PointDouble2D force;

    DynamicPlanet() {}
    DynamicPlanet(const int &id, const float &m, const PointDouble2D &pos, const QVector2D &sp, const QColor &c) :
        StaticPlanet(id, m, pos, c),
        speed(sp),
        force(PointDouble2D())
    {}
    DynamicPlanet(const int &id, const float &m, const PointDouble2D &pos, const QVector2D &sp, const QColor &c, const PointDouble2D& f) :
        StaticPlanet(id, m, pos, c),
        speed(sp),
        force(f)
    {}
    DynamicPlanet(const DynamicPlanet &p) :
        StaticPlanet(p.index, p.mass, p.position, p.color, p.leaf),
        speed(p.speed),
        force(p.force)
    {}
    ~DynamicPlanet(){}
};

struct PlanetPtr {
    bool isStatic;
    StaticPlanet* ptr;

    PlanetPtr() {}
    PlanetPtr(StaticPlanet* p, const bool &s) :
        isStatic(s),
        ptr(p)
    {}
    PlanetPtr(const PlanetPtr &p) :
        isStatic(p.isStatic),
        ptr(p.ptr)
    {}
};

class ImageRender : public QObject
{
    Q_OBJECT
public:
    explicit ImageRender(QObject *parent = 0);
    ~ImageRender();
    void createSurface();
    void initialize(const QVector2D& min, const QVector2D& max, const QList<StaticPlanet> &staticPlanets);
    void render(const QList<DynamicPlanet>& dynamicPlanets);
    QImage getImage() const;

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

    QVector2D minBound;
    QVector2D maxBound;
};

#endif // IMAGERENDER_H
