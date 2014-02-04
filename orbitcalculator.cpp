#include "orbitcalculator.h"

OrbitCalculator::OrbitCalculator(QObject *parent) :
    QObject(parent),
    running(false)
{
    time = 0;
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

void OrbitCalculator::start(const float &dt, const float &t, const QVector2D &min, const QVector2D &max)
{
    status.code = CalcStatus::Ok;
    status.values.clear();

    deltaT = dt;
    time = t;
    minBounder = min;
    maxBounder = max;
    for (int i = 0; i < planetPtrs.size(); i++) {
        planetPtrs.at(i).ptr->currentPositionX = planetPtrs.at(i).ptr->startPosition.x();
        planetPtrs.at(i).ptr->currentPositionY = planetPtrs.at(i).ptr->startPosition.y();
        if (planetPtrs.at(i).isStatic == false) {
            dynamicPlanets[planetPtrs.at(i).ptr->index].currentSpeedX = dynamicPlanets.at(planetPtrs.at(i).ptr->index).startSpeed.x();
            dynamicPlanets[planetPtrs.at(i).ptr->index].currentSpeedY = dynamicPlanets.at(planetPtrs.at(i).ptr->index).startSpeed.y();
            dynamicPlanets[planetPtrs.at(i).ptr->index].positions.append(planetPtrs.at(i).ptr->startPosition);
            dynamicPlanets[planetPtrs.at(i).ptr->index].samples.append(1);
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
    double t = 0;
    double dr, rx, ry;
    double df, fx, fy;
    float dx = (maxBounder.x() - minBounder.x()) / 1920.0;
    float dy = (maxBounder.y() - minBounder.y()) / 1080.0;
    while (running && t < time) {
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
                dynamicPlanets.at(j).samples.last() < 255)
            {
                dynamicPlanets[j].samples.last()++;
            } else {
                dynamicPlanets[j].positions.append(QVector2D(dynamicPlanets.at(j).currentPositionX, dynamicPlanets.at(j).currentPositionY));
                dynamicPlanets[j].samples.append(1);
            }
            if (dynamicPlanets.at(j).currentPositionX < minBounder.x() || dynamicPlanets.at(j).currentPositionX > maxBounder.x() ||
                dynamicPlanets.at(j).currentPositionY < minBounder.y() || dynamicPlanets.at(j).currentPositionY > maxBounder.y())
            {
                running = false;
                status.values.append(j);
            }
        }
        t += deltaT;
    }
    if (t < time) {
        status.code = CalcStatus::OutOfRange;
        status.time = t;
        time = t;
    }
    stop(); //на всякий случай
    emit finished();
}

void OrbitCalculator::save()
{
    float x,y;
    QFile file(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + "_points.dat");
    file.open(QFile::WriteOnly);
    file.write(reinterpret_cast<char*>(&deltaT), sizeof(double));
    file.write(reinterpret_cast<char*>(&time), sizeof(double));
    x = minBounder.x();
    y = minBounder.y();
    file.write(reinterpret_cast<char*>(&x), sizeof(float));
    file.write(reinterpret_cast<char*>(&y), sizeof(float));
    x = maxBounder.x();
    y = maxBounder.y();
    file.write(reinterpret_cast<char*>(&x), sizeof(float));
    file.write(reinterpret_cast<char*>(&y), sizeof(float));
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
    //|------------------------------------------------------------------------------------------------------
    //| num | Planet01.x1 | Planet01.y1 | num | Planet02.x1 | Planet02.y1 | num | Planet01.x2 | Planet01.y2 |  .....
    //|------------------------------------------------------------------------------------------------------
    int maxPlanetSamples = 0;
    for (int i = 0; i < dynamicPlanets.size(); i++)
        if (maxPlanetSamples < dynamicPlanets.at(i).samples.size())
            maxPlanetSamples = dynamicPlanets.at(i).samples.size();
    QByteArray zeroBytes;
    zeroBytes.fill(0, sizeof(unsigned char) + sizeof(QVector2D));
    for (int i = 0; i < maxPlanetSamples; i++) {
        for (int j = 0; j < dynamicPlanets.size(); j++) {
            if (i < dynamicPlanets.at(j).samples.size()) {
                file.write(reinterpret_cast<const char*>(dynamicPlanets.at(j).samples.constData() + i), sizeof(unsigned char));
                file.write(reinterpret_cast<const char*>(dynamicPlanets.at(j).positions.constData() + i), sizeof(QVector2D));
            } else {
                file.write(zeroBytes);
            }
        }
    }
    file.close();

    for (int i = 0; i < dynamicPlanets.size(); i++) {
        dynamicPlanets[i].positions.clear();
        dynamicPlanets[i].samples.clear();
    }
}
