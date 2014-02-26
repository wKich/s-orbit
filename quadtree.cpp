#include "quadtree.h"

QuadTree::~QuadTree()
{
    if (northWest != nullptr)
        delete northWest;
    if (northEast != nullptr)
        delete northEast;
    if (southWest != nullptr)
        delete southWest;
    if (southEast != nullptr)
        delete southEast;
}

QuadTree *QuadTree::createTree(QList<PlanetPtr> &ps, const QRectF &rect)
{
    QuadTree* root = new QuadTree(rect);
    for (int i = 0; i < ps.size(); i++) {
        root->addPlanet(ps[i].ptr);
    }
    return root;
}

void QuadTree::getRoot(QuadTree*& root)
{
    while(root->parent != nullptr) {
        root = root->parent;
    }
}

void QuadTree::calcForce(DynamicPlanet *p, const float &teta, const int &deepLevel)
{
    if (isLeaf) {
        if (planet != p) {
            PointDouble2D distance = planet->position - p->position;
            p->force += distance / distance.lenght() * planet->mass * p->mass / distance.squareLenght();
        }
    } else {
        if (((100.0 / pow(2, deepLevel)) / ((centerOfMass - p->position).lenght())) < teta) {
            PointDouble2D distance = centerOfMass - p->position;
            p->force += distance / distance.lenght() * mass * p->mass / distance.squareLenght();
        } else {
            if (!northWest->isLeaf || northWest->planet != nullptr)
                northWest->calcForce(p, deepLevel + 1);
            if (!northEast->isLeaf || northEast->planet != nullptr)
                northEast->calcForce(p, deepLevel + 1);
            if (!southWest->isLeaf || southWest->planet != nullptr)
                southWest->calcForce(p, deepLevel + 1);
            if (!southEast->isLeaf || southEast->planet != nullptr)
                southEast->calcForce(p, deepLevel + 1);
        }
    }
}

void QuadTree::calcPosition(DynamicPlanet *p, const double &deltaT)
{
    p->speed += p->force / p->mass * deltaT;
    p->position += p->speed * deltaT;
    p->force = PointDouble2D();
    if (p->leaf->boundary.contains(p->position.toPointF()) == false) {
        QuadTree* leaf = p->leaf;
        leaf->planet = nullptr;
        leaf->mass = 0;
        leaf->centerOfMass = leaf->boundary.center();
        leaf->parent->movePlanet(p);
    }
}

QuadTree::QuadTree(const QRectF& rect, QuadTree *p) :
    northWest(nullptr),
    northEast(nullptr),
    southWest(nullptr),
    southEast(nullptr),
    parent(p),
    isLeaf(true),
    boundary(rect),
    mass(0),
    centerOfMass(boundary.center()),
    planet(nullptr)
{
}

void QuadTree::addPlanet(StaticPlanet* p)
{
    //BUG if planet outside root boundary
    if (isLeaf) {
        if (!planet) {
            planet = p;
            planet->leaf = this;
            mass = planet->mass;
            centerOfMass = planet->position;
        } else {
            subdivide();
            putPlanetInChild(p);
            putPlanetInChild(planet);
            planet = nullptr;
            mass += p->mass;
            calcCenterOfMass();
        }
    } else {
        putPlanetInChild(p);
        mass += p->mass;
        calcCenterOfMass();
    }
}

void QuadTree::subdivide()
{
    northWest = new QuadTree(QRectF(boundary.topLeft(), boundary.center()), this);
    southEast = new QuadTree(QRectF(boundary.center(), boundary.bottomRight()), this);
    northEast = new QuadTree(QRectF(northWest->boundary.topRight(), southEast->boundary.topRight()), this);
    southWest = new QuadTree(QRectF(northWest->boundary.bottomLeft(), southEast->boundary.bottomLeft()), this);
    isLeaf = false;
}

void QuadTree::putPlanetInChild(StaticPlanet *p)
{
    if (northWest->boundary.contains(p->position.toPointF())) {
        northWest->addPlanet(p);
    } else if (northEast->boundary.contains(p->position.toPointF())) {
        northEast->addPlanet(p);
    } else if (southWest->boundary.contains(p->position.toPointF())) {
        southWest->addPlanet(p);
    } else {
        southEast->addPlanet(p);
    }
}

void QuadTree::calcCenterOfMass()
{
    centerOfMass = (northWest->centerOfMass * northWest->mass +
                    northEast->centerOfMass * northEast->mass +
                    southWest->centerOfMass * southWest->mass +
                    southEast->centerOfMass * southEast->mass) / mass;
}

void QuadTree::movePlanet(StaticPlanet *p)
{
    if (boundary.contains(p->position.toPointF())) {
        putPlanetInChild(p);
        calcCenterOfMass();
    } else {
        if (northWest->isLeaf && northEast->isLeaf &&
            southWest->isLeaf && southEast->isLeaf)
        {
            int planets = 0;
            QuadTree* child = nullptr;
            if (northWest->planet != nullptr) {
                planets++;
                child = northWest;
            }
            if (northEast->planet != nullptr) {
                planets++;
                child = northEast;
            }
            if (southWest->planet != nullptr) {
                planets++;
                child = southWest;
            }
            if (southEast->planet != nullptr) {
                planets++;
                child = southEast;
            }

            if (planets == 1) {
                planet = child->planet;
                planet->leaf = this;
                planets--;
            }
            if (planets == 0)
            {
                delete northWest;
                delete northEast;
                delete southWest;
                delete southEast;
                northWest = nullptr;
                northEast = nullptr;
                southWest = nullptr;
                southEast = nullptr;
                isLeaf = true;
                if (planet != nullptr) {
                    mass = planet->mass;
                    centerOfMass = planet->position;
                } else {
                    mass = 0;
                    centerOfMass = boundary.center();
                }
            } else {
                mass -= p->mass;
                calcCenterOfMass();
            }
        } else {
            mass -= p->mass;
            calcCenterOfMass();
        }

        if (parent != nullptr) {
            parent->movePlanet(p);
        } else {
            if (boundary.right() < p->position.x() || boundary.bottom() < p->position.y()) {
                parent = new QuadTree(QRectF(boundary.topLeft(), boundary.size() * 2));
                parent->northWest = this;
                parent->northEast = new QuadTree(QRectF(boundary.topRight(), boundary.size()), parent);
                parent->southWest = new QuadTree(QRectF(boundary.bottomLeft(), boundary.size()), parent);
                parent->southEast = new QuadTree(QRectF(boundary.bottomRight(), boundary.size()), parent);
            } else {
                parent = new QuadTree(QRectF(QPointF(boundary.left() - boundary.width(), boundary.top() - boundary.height()), boundary.size() * 2));
                parent->northWest = new QuadTree(QRectF(parent->boundary.topLeft(), boundary.size()), parent);
                parent->northEast = new QuadTree(QRectF(parent->northWest->boundary.topRight(), boundary.size()), parent);
                parent->southWest = new QuadTree(QRectF(parent->northWest->boundary.bottomLeft(), boundary.size()), parent);
                parent->southEast = this;
            }
            parent->mass = mass + p->mass;
            parent->isLeaf = false;
            parent->movePlanet(p);
        }
    }
}
