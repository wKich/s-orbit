#ifndef BRUTEFORCECALCULATOR_H
#define BRUTEFORCECALCULATOR_H

#include <QObject>

#include "orbitcalculator.h"

class BruteforceCalculator : public OrbitCalculator
{
    Q_OBJECT
public:
    explicit BruteforceCalculator(QObject *parent = 0);

signals:

public slots:

private:
    void calc();
};

#endif // BRUTEFORCECALCULATOR_H
