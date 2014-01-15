#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTableWidgetItem>
#include <QColorDialog>
#include <QMessageBox>

#include <QDebug>

#include "orbitcalculator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::MainWindow *ui;
    OrbitCalculator* orbitCalc;
    bool editing;
    QPalette defaultPalette;

private slots:
    void addPlanet();
    void startStopCalculation();
    void enablePlanetSpeed(const bool &checked);
    void enableControls();
    void calculateResultSize();
    void editPlanet();
    void showColorDialog();
    void showCalculationStatus();
};

#endif // MAINWINDOW_H
