#include "orbitcalculator.h"

OrbitCalculator::OrbitCalculator(QObject *parent) :
    QObject(parent),
    running(false)
{
    samples = 0;
    connect(this, SIGNAL(exec()), this, SLOT(run()));
    connect(this, SIGNAL(finished()), this, SLOT(save()));
}

OrbitCalculator::~OrbitCalculator()
{
}

void OrbitCalculator::addPlanet(const float &mass, const QVector2D &pos, const QVector2D &speed, const bool &isStatic, const QColor &color)
{
    if (isStatic) {
        staticPlanets.append(StaticPlanet(staticPlanets.size(), mass, pos, color));
        planetPtrs.append(PlanetPtr(&staticPlanets.last(), isStatic));
    } else {
        dynamicPlanets.append(DynamicPlanet(dynamicPlanets.size(), mass, pos, speed, color));
        planetPtrs.append(PlanetPtr(&dynamicPlanets.last(), isStatic));
    }
}

void OrbitCalculator::modifyPlanet(const int &id, const float &mass, const QVector2D &startPos, const QVector2D &startSpeed, const bool &isStatic, const QColor &color)
{
    if (planetPtrs.at(id).isStatic != isStatic) {
        if (planetPtrs.at(id).isStatic) {
            //from static to dynamic
            dynamicPlanets.append(DynamicPlanet(dynamicPlanets.size(), mass, startPos, startSpeed, color));
            for (int i = planetPtrs.at(id).ptr->index + 1; i < staticPlanets.size(); i++)
                staticPlanets[i].index--;
            staticPlanets.removeAt(planetPtrs.at(id).ptr->index);
            planetPtrs[id].ptr = &dynamicPlanets.last();
        } else {
            //from dynamic to static
            staticPlanets.append(StaticPlanet(staticPlanets.size(), mass, startPos, color));
            for (int i = planetPtrs.at(id).ptr->index + 1; i < dynamicPlanets.size(); i++)
                dynamicPlanets[i].index--;
            dynamicPlanets.removeAt(planetPtrs.at(id).ptr->index);
            planetPtrs[id].ptr = &staticPlanets.last();
        }
        planetPtrs[id].isStatic = isStatic;
    } else {
        planetPtrs.at(id).ptr->mass = mass;
        planetPtrs.at(id).ptr->startPosition = startPos;
        planetPtrs.at(id).ptr->color = color;
        if (planetPtrs.at(id).isStatic == false)
            dynamicPlanets[planetPtrs.at(id).ptr->index].startSpeed = startSpeed;
    }
}

void OrbitCalculator::removePlanet(const int &id)
{
    if (planetPtrs.at(id).isStatic) {
        for (int i = planetPtrs.at(id).ptr->index + 1; i < staticPlanets.size(); i++)
            staticPlanets[i].index--;
        staticPlanets.removeAt(planetPtrs.at(id).ptr->index);
    } else {
        for (int i = planetPtrs.at(id).ptr->index + 1; i < dynamicPlanets.size(); i++)
            dynamicPlanets[i].index--;
        dynamicPlanets.removeAt(planetPtrs.at(id).ptr->index);
    }
    planetPtrs.removeAt(id);
}

const DynamicPlanet &OrbitCalculator::getDynamicPlanet(const int &id) const
{
    return dynamicPlanets.at(id);
}

bool OrbitCalculator::isRunning() const
{
    return running;
}

void OrbitCalculator::start(const float &dt, const unsigned int &c, const QVector2D &min, const QVector2D &max)
{
    status.code = CalcStatus::Ok;
    status.values.clear();

    deltaT = dt;
    samples = c;
    minBounder = min;
    maxBounder = max;
    for (int i = 0; i < planetPtrs.size(); i++) {
        planetPtrs.at(i).ptr->currentPositionX = planetPtrs.at(i).ptr->startPosition.x();
        planetPtrs.at(i).ptr->currentPositionY = planetPtrs.at(i).ptr->startPosition.y();
        if (planetPtrs.at(i).isStatic == false) {
            dynamicPlanets[planetPtrs.at(i).ptr->index].currentSpeedX = dynamicPlanets.at(planetPtrs.at(i).ptr->index).startSpeed.x();
            dynamicPlanets[planetPtrs.at(i).ptr->index].currentSpeedY = dynamicPlanets.at(planetPtrs.at(i).ptr->index).startSpeed.y();
            dynamicPlanets[planetPtrs.at(i).ptr->index].xPositions.reserve(samples);
            dynamicPlanets[planetPtrs.at(i).ptr->index].yPositions.reserve(samples);
            dynamicPlanets[planetPtrs.at(i).ptr->index].xPositions.append(planetPtrs.at(i).ptr->startPosition.x());
            dynamicPlanets[planetPtrs.at(i).ptr->index].yPositions.append(planetPtrs.at(i).ptr->startPosition.y());
        }
    }
    emit exec();
}

void OrbitCalculator::stop()
{
    running = false;
}

const CalcStatus & OrbitCalculator::getCalculationStatus() const
{
    return status;
}

void OrbitCalculator::run()
{
    running = true;
    unsigned int i = 0;
    double dr, rx, ry;
    double df, fx, fy;
    while (running && i < samples) {
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
            dynamicPlanets[j].xPositions.append(dynamicPlanets.at(j).currentPositionX);
            dynamicPlanets[j].yPositions.append(dynamicPlanets.at(j).currentPositionY);
            if (dynamicPlanets.at(j).currentPositionX < minBounder.x() || dynamicPlanets.at(j).currentPositionX > maxBounder.x() ||
                dynamicPlanets.at(j).currentPositionY < minBounder.y() || dynamicPlanets.at(j).currentPositionY > maxBounder.y())
            {
                running = false;
                status.values.append(j);
            }
        }
        i++;
    }
    if (i < samples) {
        status.code = CalcStatus::OutOfRange;
        status.values.prepend(i);
        samples = i;
    }
    stop(); //на всякий случай
    emit finished();
}

void OrbitCalculator::save()
{
    float x,y;
    QFile file(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + "_points.dat");
    file.open(QFile::WriteOnly);
    file.write(reinterpret_cast<char*>(&deltaT), sizeof(float));
    x = minBounder.x();
    y = minBounder.y();
    file.write(reinterpret_cast<char*>(&x), sizeof(float));
    file.write(reinterpret_cast<char*>(&y), sizeof(float));
    x = maxBounder.x();
    y = maxBounder.y();
    file.write(reinterpret_cast<char*>(&x), sizeof(float));
    file.write(reinterpret_cast<char*>(&y), sizeof(float));
    file.write(reinterpret_cast<char*>(&samples), sizeof(unsigned int));
    unsigned int sPlanets = staticPlanets.size();
    file.write(reinterpret_cast<char*>(&sPlanets), sizeof(unsigned int));
    for (int i = 0; i < staticPlanets.size(); i++) {
        QRgb color = staticPlanets.at(i).color.rgb();
        file.write(reinterpret_cast<char*>(&color), sizeof(QRgb));
    }
    for (int i = 0; i < staticPlanets.size(); i++) {
        x = staticPlanets.at(i).startPosition.x();
        y = staticPlanets.at(i).startPosition.y();
        file.write(reinterpret_cast<char*>(&x), sizeof(float));
        file.write(reinterpret_cast<char*>(&y), sizeof(float));
    }
    unsigned int dPlanets = dynamicPlanets.size();
    file.write(reinterpret_cast<char*>(&dPlanets), sizeof(unsigned int));
    for (int i = 0; i < dynamicPlanets.size(); i++) {
        QRgb color = dynamicPlanets.at(i).color.rgb();
        file.write(reinterpret_cast<char*>(&color), sizeof(QRgb));
    }
    //|------------------------------------------------------------------------------------
    //| Planet01.x1 | Planet01.y1 | Planet02.x1 | Planet02.y1 | Planet01.x2 | Planet01.y2 |  .....
    //|------------------------------------------------------------------------------------
    for (int j = 0; j < samples; j++) {
        for (int i = 0; i < dynamicPlanets.size(); i++) {
            file.write(reinterpret_cast<const char*>(dynamicPlanets.at(i).xPositions.constData() + j), sizeof(float));
            file.write(reinterpret_cast<const char*>(dynamicPlanets.at(i).yPositions.constData() + j), sizeof(float));
        }
    }
    file.close();

    for (int i = 0; i < dynamicPlanets.size(); i++) {
        dynamicPlanets[i].xPositions.clear();
        dynamicPlanets[i].yPositions.clear();
    }
}
