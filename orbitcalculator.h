#ifndef ORBITCALCULATOR_H
#define ORBITCALCULATOR_H

#include <QtCore>
#include <QVector>
#include <QVector2D>
#include <QThread>
#include <QFile>
#include <QDateTime>
#include <QColor>

struct StaticPlanet {
    int index;
    float mass;
    QVector2D startPosition;
    double currentPositionX;    //for backport calculation
    double currentPositionY;    //for backport calculation
    QColor color;

    StaticPlanet(){}
    StaticPlanet(const int &id, const float &m, const QVector2D &pos, const QColor &c) :
        index(id),
        mass(m),
        startPosition(pos),
        color(c)
    {}
    StaticPlanet(const StaticPlanet &p) :
        index(p.index),
        mass(p.mass),
        startPosition(p.startPosition),
        color(p.color)
    {}
    virtual ~StaticPlanet(){}
};

struct DynamicPlanet : StaticPlanet {
    QVector<float> xPositions;
    QVector<float> yPositions;
    QVector2D startSpeed;
    double currentSpeedX;
    double currentSpeedY;

    DynamicPlanet() {}
    DynamicPlanet(const int &id, const float &m, const QVector2D &pos, const QVector2D &speed, const QColor &c) :
        StaticPlanet(id, m, pos, c),
        startSpeed(speed)
    {}
    DynamicPlanet(const DynamicPlanet &p) :
        StaticPlanet(p.index, p.mass, p.startPosition, p.color),
        startSpeed(p.startSpeed)
    {}
    ~DynamicPlanet(){}
};

struct CalcStatus {
    enum StatusCode {
        Ok,
        OutOfRange
    };

    StatusCode code;
    QList<unsigned int> values;
    //if Code == OutOfRange
    //values contains id planets that were out of range
    //first value is sample count

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
    bool isRunning() const;
    void start(const float &dt, const unsigned int &c, const QVector2D &min, const QVector2D &max);
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

    float deltaT;
    unsigned int samples;
    QVector2D minBounder;
    QVector2D maxBounder;

private slots:
    void run();
    void save();
};

#endif // ORBITCALCULATOR_H
