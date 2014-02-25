#include "barneshutcalculator.h"

BarnesHutCalculator::BarnesHutCalculator(QObject *parent) :
    OrbitCalculator(parent),
    teta(0)
{
}

void BarnesHutCalculator::setTeta(const float &t)
{
    teta = t;
}

void BarnesHutCalculator::calc()
{

}
