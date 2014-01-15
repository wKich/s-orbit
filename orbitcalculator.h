#ifndef ORBITCALCULATOR_H
#define ORBITCALCULATOR_H

#include <QtCore>
#include <QVector>
#include <QVector2D>
#include <QThread>
#include <QFile>
#include <QDateTime>
#include <QColor>

struct Planet {
    float mass;
    QVector<float> xPositions;
    QVector<float> yPositions;
    QVector2D startPosition;
    QVector2D currentPosition;
    QVector2D startSpeed;
    QVector2D currentSpeed;
    bool isStatic;
    QColor color;

    Planet() {}
    Planet(const float &m, const QVector2D &pos, const QVector2D &speed, const bool &stat, const QColor &c) :
        mass(m),
        startPosition(pos),
        startSpeed(speed),
        isStatic(stat),
        color(c)
    {}
    Planet(const Planet &p) :
        mass(p.mass),
        startPosition(p.startPosition),
        startSpeed(p.startSpeed),
        isStatic(p.isStatic),
        color(p.color)
    {}
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
    void modifyPlanet(const int &id, const float &mass, const QVector2D &curPos, const QVector2D &curSpeed, const bool &isStatic, const QColor &color);
    void removePlanet(const int &id);
    const Planet & getPlanet(const int &id) const;
    bool isRunning() const;
    void start(const float &dt, const unsigned int &c, const QVector2D &min, const QVector2D &max);
    void stop();
    const CalcStatus & getCalculationStatus() const;

signals:
    void exec();
    void finished();

private:
    QVector<Planet> planets;
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
