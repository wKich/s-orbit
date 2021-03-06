#ifndef ORBITCALCULATOR_H
#define ORBITCALCULATOR_H

#include <QtCore>
#include <QVector>
#include <QVector2D>

#include "imagerender.h"
#include "datafile.h"

struct CalcStatus {
    enum StatusCode {
        Ok,
        OutOfRange
    };

    StatusCode code;
    QList<unsigned int> values;
    double time;
    //if Code == OutOfRange
    //values contains id planets that were out of range
    //time is full time from start to out of range

    CalcStatus() :
        code(CalcStatus::Ok)
    {}
};

class OrbitCalculator : public QObject
{
    Q_OBJECT
public:
    explicit OrbitCalculator(QObject* parent = 0);
    ~OrbitCalculator();
    void createSurface();
    const DynamicPlanet & getDynamicPlanet(const int &id) const;
    const QImage& getPreview() const;
    bool isRunning() const;
    void setPlanets(const QList<StaticPlanet>& sp, const QList<DynamicPlanet>& dp);
    void setProperties(const float &dt, const float &t, const QVector2D &min, const QVector2D &max, const QVector2D &res);
    void stop();
    const CalcStatus & getCalculationStatus() const;

signals:
    void exec();
    void finished();

public slots:

protected:
    QList<StaticPlanet> staticPlanets;
    QList<DynamicPlanet> dynamicPlanets;
    QList<PlanetPtr> planetPtrs;
    bool running;
    CalcStatus status;

    //must be time / deltaT <= 10E+15 (double)
    //must be time / deltaT <= 10E+7 (float)
    double deltaT;
    double time;
    PointDouble2D minBound;
    PointDouble2D maxBound;
    PointDouble2D resolution;

    DataFile dFile;
    ImageRender previewRender;
    QImage preview;

    void updatePreview();
    void reducePlanetsSamples();

    virtual void calc() = 0;

private slots:
    void run();
};

#endif // ORBITCALCULATOR_H
