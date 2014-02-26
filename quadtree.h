#ifndef QUADTREE_H
#define QUADTREE_H

#include <QRectF>
#include <QVector>
#include <cmath>

#include "pointdouble2d.h"
#include "imagerender.h"

struct StaticPlanet;
struct DynamicPlanet;
struct PlanetPtr;

class QuadTree
{
public:
    ~QuadTree();
    static QuadTree* createTree(QList<PlanetPtr> &ps, const QRectF& rect);
    static void getRoot(QuadTree *&root);
    void calcForce(DynamicPlanet *p, const float& teta, const int& deepLevel = 0);
    void calcPosition(DynamicPlanet *p, const double &deltaT);

private:
    explicit QuadTree(const QRectF& rect, QuadTree *p = nullptr);
    void addPlanet(StaticPlanet *p);
    void subdivide();
    void putPlanetInChild(StaticPlanet *p);
    void calcCenterOfMass();
    void movePlanet(StaticPlanet* p);

    QuadTree* northWest;
    QuadTree* northEast;
    QuadTree* southWest;
    QuadTree* southEast;
    QuadTree* parent;
    bool isLeaf;
    QRectF boundary;
    float mass;
    PointDouble2D centerOfMass;
    StaticPlanet* planet;
};

#endif // QUADTREE_H
