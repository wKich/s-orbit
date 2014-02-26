#include "bruteforcecalculator.h"

BruteforceCalculator::BruteforceCalculator(QObject *parent) :
    OrbitCalculator(parent)
{
}

void BruteforceCalculator::calc()
{
    double t = 0;
    PointDouble2D r, f, d;
    d = (maxBound - minBound) / resolution;
    while (running && qAbs(t) < qAbs(time)) {
        //Расчет параметров для каждой планеты
        for (int j = 0; j < dynamicPlanets.size(); j++) {
            f = PointDouble2D();
            for (int k = 0; k < planetPtrs.size(); k++) {
                if (planetPtrs.at(k).ptr != &dynamicPlanets.at(j)) {
                    r = planetPtrs.at(k).ptr->position - dynamicPlanets.at(j).position;
                    f += r / r.lenght() * planetPtrs.at(k).ptr->mass * dynamicPlanets.at(j).mass / r.squareLenght();
                }
            }
            dynamicPlanets[j].speed += f / dynamicPlanets.at(j).mass * deltaT;
            dynamicPlanets[j].position += dynamicPlanets.at(j).speed * deltaT;
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
    if (qAbs(t) < qAbs(time)) {
        status.code = CalcStatus::OutOfRange;
        status.time = t;
        time = t;
    }
}
