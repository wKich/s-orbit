#include "orbitcalculator.h"

OrbitCalculator::OrbitCalculator(QObject *parent) :
    QObject(parent),
    running(false)
{
    time = 0;
    connect(this, SIGNAL(exec()), this, SLOT(run()));
}

OrbitCalculator::~OrbitCalculator()
{
}

void OrbitCalculator::createSurface()
{
    previewRender.createSurface();
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

void OrbitCalculator::setPlanets(const QList<StaticPlanet> &sp, const QList<DynamicPlanet> &dp)
{
    staticPlanets = sp;
    dynamicPlanets = dp;
    for (int i = 0; i < staticPlanets.size(); i++)
        planetPtrs.append(PlanetPtr(&staticPlanets[i], true));
    for (int i = 0; i < dynamicPlanets.size(); i++)
        planetPtrs.append(PlanetPtr(&dynamicPlanets[i], false));
}

void OrbitCalculator::setProperties(const float &dt, const float &t, const QVector2D &min, const QVector2D &max, const QVector2D &res)
{
    status.code = CalcStatus::Ok;
    status.values.clear();

    deltaT = dt;
    time = t;
    minBound = min;
    maxBound = max;
    resolution = res;
    for (int i = 0; i < planetPtrs.size(); i++)
        if (planetPtrs.at(i).isStatic == false)
            dynamicPlanets[planetPtrs.at(i).ptr->index].samples.append(PositionSample(1, planetPtrs.at(i).ptr->position.toVector2D()));
}

void OrbitCalculator::stop()
{
    running = false;
}

const CalcStatus & OrbitCalculator::getCalculationStatus() const
{
    return status;
}

void OrbitCalculator::updatePreview()
{
    previewRender.render(dynamicPlanets);

    if (running == false) {
        preview = previewRender.getImage();
        preview.save(dFile.getFileName().left(dFile.getFileName().size() - 4) + ".png");
    }
}

void OrbitCalculator::reducePlanetsSamples()
{
    if (running) {
        for (int i = 0; i < dynamicPlanets.size(); i++)
            dynamicPlanets[i].samples.remove(0, dynamicPlanets.at(i).samples.size() - 1);
    } else {
        for (int i = 0; i < dynamicPlanets.size(); i++)
            dynamicPlanets[i].samples.clear();
    }
}

void OrbitCalculator::run()
{
    dFile.initialize(deltaT, time, minBound.toVector2D(), maxBound.toVector2D(), staticPlanets, dynamicPlanets);
    previewRender.initialize(minBound.toVector2D(), maxBound.toVector2D(), staticPlanets);

    running = true;
    calc();
    stop(); //на всякий случай
    dFile.save(dynamicPlanets, time, running);
    updatePreview();
    reducePlanetsSamples();
    emit finished();
}
