#ifndef ORBITCALCULATOR_H
#define ORBITCALCULATOR_H

#include <QtCore>
#include <QVector>
#include <QVector2D>
#include <QThread>
#include <QFile>
#include <QDateTime>

#include "imagerender.h"

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
    void addPlanet(const float &mass, const QVector2D &pos, const QVector2D &curSpeed, const bool &isStatic, const QColor &color);
    void modifyPlanet(const int &id, const float &mass, const QVector2D &startPos, const QVector2D &startSpeed, const bool &isStatic, const QColor &color);
    void removePlanet(const int &id);
    const DynamicPlanet & getDynamicPlanet(const int &id) const;
    const QImage& getPreview() const;
    bool isRunning() const;
    void start(const float &dt, const float &t, const QVector2D &min, const QVector2D &max, const QVector2D &res);
    void stop();
    const CalcStatus & getCalculationStatus() const;

signals:
    void exec();
    void finished();

private:
    struct PlanetPtr {
        bool isStatic;
        StaticPlanet* ptr;

        PlanetPtr() {}
        PlanetPtr(StaticPlanet* p, const bool &s) :
            isStatic(s),
            ptr(p)
        {}
        PlanetPtr(const PlanetPtr &p) :
            isStatic(p.isStatic),
            ptr(p.ptr)
        {}
    };

    QList<StaticPlanet> staticPlanets;
    QList<DynamicPlanet> dynamicPlanets;
    QList<PlanetPtr> planetPtrs;
    bool running;
    CalcStatus status;

    //must be time / deltaT <= 10E+15 (double)
    //must be time / deltaT <= 10E+7 (float)
    double deltaT;
    double time;
    QVector2D minBounder;
    QVector2D maxBounder;
    QVector2D resolution;

    ImageRender* previewRender;
    QImage preview;
    QFile dataFile;

private slots:
    void run();
    void save();
};

#endif // ORBITCALCULATOR_H
