#ifndef DATAFILE_H
#define DATAFILE_H

#include <QObject>
#include <QFile>
#include <QDateTime>

#include "imagerender.h"

class DataFile : public QObject
{
    Q_OBJECT
public:
    explicit DataFile(QObject *parent = 0);
    QString getFileName() const;
    void initialize(const double &dt, const double &time, const QVector2D &min, const QVector2D &max,
                    const QList<StaticPlanet> &sPlanets, const QList<DynamicPlanet> &dPlanets);
    void save(const QList<DynamicPlanet> &dPlanets, const double &time, const bool &isRunning);

signals:

public slots:

private:
    QFile m_file;
};

#endif // DATAFILE_H
