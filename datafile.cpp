#include "datafile.h"

DataFile::DataFile(QObject *parent) :
    QObject(parent)
{
}

QString DataFile::getFileName() const
{
    return m_file.fileName();
}

void DataFile::initialize(const double &dt, const double &time, const QVector2D &min, const QVector2D &max,
                          const QList<StaticPlanet> &sPlanets, const QList<DynamicPlanet> &dPlanets)
{
    m_file.setFileName(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + "_points.dat");
    m_file.open(QFile::ReadWrite);
    m_file.write(reinterpret_cast<const char*>(&dt), sizeof(double));
    m_file.write(reinterpret_cast<const char*>(&time), sizeof(double));
    m_file.write(reinterpret_cast<const char*>(&min), sizeof(QVector2D));
    m_file.write(reinterpret_cast<const char*>(&max), sizeof(QVector2D));
    int sPlanetsSize = sPlanets.size();
    m_file.write(reinterpret_cast<char*>(&sPlanetsSize), sizeof(int));
    for (int i = 0; i < sPlanetsSize; i++) {
        QRgb color = sPlanets.at(i).color.rgb();
        m_file.write(reinterpret_cast<char*>(&color), sizeof(QRgb));
    }
    for (int i = 0; i < sPlanetsSize; i++)
        m_file.write(reinterpret_cast<const char*>(&sPlanets.at(i).position), sizeof(PointDouble2D));
    int dPlanetsSize = dPlanets.size();
    m_file.write(reinterpret_cast<char*>(&dPlanetsSize), sizeof(int));
    for (int i = 0; i < dPlanetsSize; i++) {
        QRgb color = dPlanets.at(i).color.rgb();
        m_file.write(reinterpret_cast<char*>(&color), sizeof(QRgb));
    }
}

void DataFile::save(const QList<DynamicPlanet> &dPlanets, const double &time, const bool &isRunning)
{
    //Warning
    //if (maxPlanetSamples - minPlanetSamples) == (1024 * 1024 * 1024 / sizeof(PositionSample) / dynamicPlanets.size()) it may crash app
    //Or solve it with add additional zero data
    //|------------------------------------------------------------------------------------------------------
    //| num | Planet01.x1 | Planet01.y1 | num | Planet02.x1 | Planet02.y1 | num | Planet01.x2 | Planet01.y2 |  .....
    //|------------------------------------------------------------------------------------------------------
    int r = isRunning ? 1 : 0;
    int maxPlanetSamples = 0;
    for (int i = 0; i < dPlanets.size(); i++)
        if (maxPlanetSamples < dPlanets.at(i).samples.size())
            maxPlanetSamples = dPlanets.at(i).samples.size();
    QByteArray zeroBytes(sizeof(PositionSample), 0);
    for (int i = 0; i < maxPlanetSamples; i++) {
        for (int j = 0; j < dPlanets.size(); j++) {
            if (i < (dPlanets.at(j).samples.size() - r)) {
                m_file.write(reinterpret_cast<const char*>(dPlanets.at(j).samples.constData() + i), sizeof(PositionSample));
            } else {
                m_file.write(zeroBytes);
            }
        }
    }

    if (isRunning == false) {
        m_file.seek(sizeof(double));
        m_file.write(reinterpret_cast<const char*>(&time), sizeof(double));
        m_file.close();
    }
}
