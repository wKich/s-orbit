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
    planets.append(Planet(mass, pos, speed, isStatic, color));
}

void OrbitCalculator::modifyPlanet(const int &id, const float &mass, const QVector2D &curPos, const QVector2D &curSpeed, const bool &isStatic, const QColor &color)
{
    planets[id].mass = mass;
    planets[id].startPosition = curPos;
    planets[id].startSpeed = curSpeed;
    planets[id].isStatic = isStatic;
    planets[id].color = color;
}

void OrbitCalculator::removePlanet(const int &id)
{
    planets.removeAt(id);
}

const Planet &OrbitCalculator::getPlanet(const int &id) const
{
    return planets.at(id);
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
    for (int i = 0; i < planets.size(); i++) {
        planets[i].currentPosition = planets.at(i).startPosition;
        planets[i].currentSpeed = planets.at(i).startSpeed;
        if (planets.at(i).isStatic == false) {
            planets[i].xPositions.reserve(samples);
            planets[i].yPositions.reserve(samples);
            planets[i].xPositions.append(planets.at(i).startPosition.x());
            planets[i].yPositions.append(planets.at(i).startPosition.y());
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
    while (running && i < samples) {
        //Расчет параметров для каждой планеты
        for (int j = 0; j < planets.size(); j++) {
            if (planets.at(j).isStatic == false) {
                QVector2D r;
                float df;
                QVector2D f;
                for (int k = 0; k < planets.size(); k++) {
                    if (k != j) {
                        r = planets.at(k).currentPosition - planets.at(j).currentPosition;
                        df = planets.at(k).mass * planets.at(j).mass / r.lengthSquared();
                        f += r / r.length() * df;
                    }
                }
                planets[j].currentSpeed += f / planets.at(j).mass * deltaT;
                planets[j].currentPosition += planets.at(j).currentSpeed * deltaT;
                planets[j].xPositions.append(planets.at(j).currentPosition.x());
                planets[j].yPositions.append(planets.at(j).currentPosition.y());
                if (planets.at(j).xPositions.last() < minBounder.x() || planets.at(j).xPositions.last() > maxBounder.x() ||
                    planets.at(j).yPositions.last() < minBounder.y() || planets.at(j).yPositions.last() > maxBounder.y())
                {
                    running = false;
                    status.values.append(j);
                }
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
    unsigned int staticPlanets = 0;
    for (int i = 0; i < planets.size(); i++) {
        if (planets.at(i).isStatic)
            staticPlanets++;
    }
    file.write(reinterpret_cast<char*>(&staticPlanets), sizeof(unsigned int));
    for (int i = 0; i < planets.size(); i++) {
        if (planets.at(i).isStatic) {
            QRgb color = planets.at(i).color.rgb();
            file.write(reinterpret_cast<char*>(&color), sizeof(QRgb));
        }
    }
    for (int i = 0; i < planets.size(); i++) {
        if (planets.at(i).isStatic) {
            file.write(reinterpret_cast<const char*>(planets.at(i).xPositions.constData()), sizeof(float));
            file.write(reinterpret_cast<const char*>(planets.at(i).yPositions.constData()), sizeof(float));
        }
    }
    unsigned int dynamicPlanets = planets.size() - staticPlanets;
    file.write(reinterpret_cast<char*>(&dynamicPlanets), sizeof(unsigned int));
    for (int i = 0; i < planets.size(); i++) {
        if (planets.at(i).isStatic == false) {
            QRgb color = planets.at(i).color.rgb();
            file.write(reinterpret_cast<char*>(&color), sizeof(QRgb));
        }
    }
    //|------------------------------------------------------------------------------------
    //| Planet01.x1 | Planet01.y1 | Planet02.x1 | Planet02.y1 | Planet01.x2 | Planet01.y2 |  .....
    //|------------------------------------------------------------------------------------
    for (int j = 0; j < samples; j++) {
        for (int i = 0; i < planets.size(); i++) {
            if (planets.at(i).isStatic == false) {
                file.write(reinterpret_cast<const char*>(planets.at(i).xPositions.constData() + j), sizeof(float));
                file.write(reinterpret_cast<const char*>(planets.at(i).yPositions.constData() + j), sizeof(float));
            }
        }
    }
    file.close();

    for (int i = 0; i < planets.size(); i++) {
        if (planets.at(i).isStatic == false) {
            planets[i].xPositions.clear();
            planets[i].yPositions.clear();
        }
    }
}
