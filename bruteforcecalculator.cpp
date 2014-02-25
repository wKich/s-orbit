#include "bruteforcecalculator.h"

BruteforceCalculator::BruteforceCalculator(QObject *parent) :
    OrbitCalculator(parent)
{
}

void BruteforceCalculator::calc()
{
    double t = 0;
    double dr, rx, ry;
    double df, fx, fy;
    float dx = (maxBound.x() - minBound.x()) / resolution.x();
    float dy = (maxBound.y() - minBound.y()) / resolution.y();
    while (running && qAbs(t) < qAbs(time)) {
        //Расчет параметров для каждой планеты
        for (int j = 0; j < dynamicPlanets.size(); j++) {
            fx = 0;
            fy = 0;
            for (int k = 0; k < planetPtrs.size(); k++) {
                if (planetPtrs.at(k).ptr != &dynamicPlanets.at(j)) {
                    rx = planetPtrs.at(k).ptr->currentPositionX - dynamicPlanets.at(j).currentPositionX;
                    ry = planetPtrs.at(k).ptr->currentPositionY - dynamicPlanets.at(j).currentPositionY;
                    dr = sqrt(rx*rx + ry*ry);
                    df = planetPtrs.at(k).ptr->mass * dynamicPlanets.at(j).mass / (dr*dr);
                    fx += rx / dr * df;
                    fy += ry / dr * df;
                }
            }
            dynamicPlanets[j].currentSpeedX += fx / dynamicPlanets.at(j).mass * deltaT;
            dynamicPlanets[j].currentSpeedY += fy / dynamicPlanets.at(j).mass * deltaT;
            dynamicPlanets[j].currentPositionX += dynamicPlanets.at(j).currentSpeedX * deltaT;
            dynamicPlanets[j].currentPositionY += dynamicPlanets.at(j).currentSpeedY * deltaT;
            if (qAbs(dynamicPlanets.at(j).currentPositionX - dynamicPlanets.at(j).positions.last().x()) < dx &&
                qAbs(dynamicPlanets.at(j).currentPositionY - dynamicPlanets.at(j).positions.last().y()) < dy &&
                dynamicPlanets.at(j).samples.last() < 65535)
            {
                dynamicPlanets[j].samples.last()++;
            } else {
                //~1Gb buffer for positions samples
                if (dynamicPlanets[j].samples.size() >= (1024 * 1024 * 1024 / (sizeof(unsigned short) + sizeof(QVector2D)) / dynamicPlanets.size()) ) {
                    dFile.save(dynamicPlanets, time, running);
                    updatePreview();
                    reducePlanetsSamples();
                }
                dynamicPlanets[j].positions.append(QVector2D(dynamicPlanets.at(j).currentPositionX, dynamicPlanets.at(j).currentPositionY));
                dynamicPlanets[j].samples.append(1);
            }
            if (dynamicPlanets.at(j).currentPositionX < minBound.x() || dynamicPlanets.at(j).currentPositionX > maxBound.x() ||
                dynamicPlanets.at(j).currentPositionY < minBound.y() || dynamicPlanets.at(j).currentPositionY > maxBound.y())
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
