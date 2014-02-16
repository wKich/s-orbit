#include "orbitcalculator.h"

OrbitCalculator::OrbitCalculator(QObject *parent) :
    QObject(parent),
    running(false),
    previewRender(nullptr)
{
    time = 0;
    connect(this, SIGNAL(exec()), this, SLOT(run()));
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

const QImage &OrbitCalculator::getPreview() const
{
    return preview;
}

bool OrbitCalculator::isRunning() const
{
    return running;
}

void OrbitCalculator::start(const float &dt, const float &t, const QVector2D &min, const QVector2D &max, const QVector2D &res)
{
    status.code = CalcStatus::Ok;
    status.values.clear();

    deltaT = dt;
    time = t;
    minBound = min;
    maxBound = max;
    resolution = res;
    for (int i = 0; i < planetPtrs.size(); i++) {
        planetPtrs.at(i).ptr->currentPositionX = planetPtrs.at(i).ptr->startPosition.x();
        planetPtrs.at(i).ptr->currentPositionY = planetPtrs.at(i).ptr->startPosition.y();
        if (planetPtrs.at(i).isStatic == false) {
            dynamicPlanets[planetPtrs.at(i).ptr->index].currentSpeedX = dynamicPlanets.at(planetPtrs.at(i).ptr->index).startSpeed.x();
            dynamicPlanets[planetPtrs.at(i).ptr->index].currentSpeedY = dynamicPlanets.at(planetPtrs.at(i).ptr->index).startSpeed.y();
            if (deltaT < 0) {
                dynamicPlanets[planetPtrs.at(i).ptr->index].positions.append(planetPtrs.at(i).ptr->startPosition + dynamicPlanets.at(planetPtrs.at(i).ptr->index).startSpeed * deltaT);
            } else {
                dynamicPlanets[planetPtrs.at(i).ptr->index].positions.append(planetPtrs.at(i).ptr->startPosition);
            }
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

void OrbitCalculator::setSurface(QOffscreenSurface *surface)
{
    if (!previewRender)
        previewRender = new ImageRender(this);
    previewRender->setSurface(surface);
}

void OrbitCalculator::updatePreview()
{
    previewRender->render(dynamicPlanets);

    if (running == false) {
        preview = previewRender->getImage();
        preview.save(dFile.getFileName().left(dFile.getFileName().size() - 4) + ".png");
    }
}

void OrbitCalculator::reducePlanetsSamples()
{
    if (running) {
        for (int i = 0; i < dynamicPlanets.size(); i++) {
            dynamicPlanets[i].positions.remove(0, dynamicPlanets.at(i).positions.size() - 1);
            dynamicPlanets[i].samples.remove(0, dynamicPlanets.at(i).samples.size() - 1);
        }
    } else {
        for (int i = 0; i < dynamicPlanets.size(); i++) {
            dynamicPlanets[i].positions.clear();
            dynamicPlanets[i].samples.clear();
        }
    }
}

void OrbitCalculator::run()
{
    dFile.initialize(deltaT, time, minBound, maxBound, staticPlanets, dynamicPlanets);
    previewRender->initialize(minBound, maxBound, staticPlanets);

    running = true;
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
    stop(); //на всякий случай
    dFile.save(dynamicPlanets, time, running);
    updatePreview();
    reducePlanetsSamples();
    emit finished();
}
