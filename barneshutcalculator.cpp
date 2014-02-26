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
    double t = 0;
    PointDouble2D d;
    d = (maxBound - minBound) / resolution;
    QuadTree* root = QuadTree::createTree(planetPtrs, QRectF(minBound.toPointF(), maxBound.toPointF()));
    while (running && qAbs(t) < qAbs(time)) {
        //Расчет параметров для каждой планеты
        for (int j = 0; j < dynamicPlanets.size(); j++) {
            root->calcForce(&dynamicPlanets[j], teta);
        }
        for (int j = 0; j < dynamicPlanets.size(); j++) {
            root->calcPosition(&dynamicPlanets[j], deltaT);
            if (!(PointDouble2D::abs(dynamicPlanets.at(j).position - dynamicPlanets.at(j).samples.last().position) > d) &&
                dynamicPlanets.at(j).samples.last().count < 65535)
            {
                dynamicPlanets[j].samples.last().count++;
            } else {
                //~1Gb buffer for positions samples
                if (dynamicPlanets[j].samples.size() >= (1024 * 1024 * 1024 / sizeof(PositionSample) / dynamicPlanets.size()) ) {
                    dFile.save(dynamicPlanets, time, running);
                    updatePreview();
                    reducePlanetsSamples();
                }
                dynamicPlanets[j].samples.append(PositionSample(1, dynamicPlanets.at(j).position.toVector2D()));
            }
            if (dynamicPlanets.at(j).position < minBound || dynamicPlanets.at(j).position > maxBound)
            {
                running = false;
                status.values.append(j);
            }
        }
        t += deltaT;
    }
    QuadTree::getRoot(root);
    delete root;
    if (qAbs(t) < qAbs(time)) {
        status.code = CalcStatus::OutOfRange;
        status.time = t;
        time = t;
    }
}
