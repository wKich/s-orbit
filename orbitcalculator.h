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
        currentPosition(startPosition),
        startSpeed(speed),
        currentSpeed(startSpeed),
        isStatic(stat),
        color(c)
    {
        xPositions.append(currentPosition.x());
        yPositions.append(currentPosition.y());
    }
    Planet(const Planet &p) :
        mass(p.mass),
        startPosition(p.startPosition),
        currentPosition(startPosition),
        startSpeed(p.startSpeed),
        currentSpeed(startSpeed),
        isStatic(p.isStatic),
        color(p.color)
    {
        xPositions.append(currentPosition.x());
        yPositions.append(currentPosition.y());
    }
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
    bool isRunning() const;
    void start(const float &dt, const unsigned int &c, const QVector2D &min, const QVector2D &max);
    void stop();

signals:
    void exec();
    void finished();

private:
    QVector<Planet> planets;
    bool running;

    float deltaT;
    unsigned int samples;
    QVector2D minBounder;
    QVector2D maxBounder;

private slots:
    void run();
    void save();
};

#endif // ORBITCALCULATOR_H
