#ifndef BARNESHUTCALCULATOR_H
#define BARNESHUTCALCULATOR_H

#include <QObject>

#include "orbitcalculator.h"
#include "quadtree.h"

class BarnesHutCalculator : public OrbitCalculator
{
    Q_OBJECT
public:
    explicit BarnesHutCalculator(QObject *parent = 0);
    void setTeta(const float& t);

signals:

public slots:

private:
    float teta;

    void calc();
};

#endif // BARNESHUTCALCULATOR_H
