#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    defaultStyle = "border: none; background-color: #FFFFFF;";
    ui->colorButton->setStyleSheet(defaultStyle);
    editing = false;
    QThread* thread = new QThread;
    orbitCalc = new OrbitCalculator;
    orbitCalc->moveToThread(thread);
    connect(orbitCalc, SIGNAL(destroyed()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(ui->addEditPlanetButton, SIGNAL(clicked()), this, SLOT(addPlanet()));
    connect(ui->calcButton, SIGNAL(clicked()), this, SLOT(startStopCalculation()));
    connect(orbitCalc, SIGNAL(finished()), this, SLOT(enableControls()));
    connect(orbitCalc, SIGNAL(finished()), this, SLOT(showCalculationStatus()));
    connect(ui->isStaticCheckBox, SIGNAL(toggled(bool)), this, SLOT(enablePlanetSpeed(bool)));
    connect(ui->timeLineEdit, SIGNAL(editingFinished()), this, SLOT(calculateResultSize()));
    connect(ui->addEditPlanetButton, SIGNAL(clicked()), this, SLOT(calculateResultSize()));
    connect(ui->planetTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(editPlanet()));
    connect(ui->colorButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    thread->start();
}

MainWindow::~MainWindow()
{
    if (orbitCalc->isRunning())
        orbitCalc->stop();
    orbitCalc->deleteLater();
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (this->focusWidget() == ui->planetTableWidget) {
        switch (event->key()) {
        case Qt::Key_Delete:
            if (ui->planetTableWidget->selectedItems().size() > 0) {
                orbitCalc->removePlanet(ui->planetTableWidget->currentRow());
                ui->planetTableWidget->removeRow(ui->planetTableWidget->selectedItems().first()->row());
            }
            break;
        case Qt::Key_Escape:
            ui->planetTableWidget->clearSelection();
            break;
        default:
            QMainWindow::keyPressEvent(event);
            break;
        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::addPlanet()
{
    QPalette invalidPalette;
    invalidPalette.setColor(QPalette::WindowText, Qt::red);
    invalidPalette.setColor(QPalette::Text, Qt::red);

    bool ok;
    bool allOk = true;
    float mass = ui->massLineEdit->text().toFloat(&ok);
    if (ok) {
        ui->massLabel->setPalette(this->palette());
        ui->massLineEdit->setPalette(this->palette());
    } else {
        allOk = false;
        ui->massLabel->setPalette(invalidPalette);
        ui->massLineEdit->setPalette(invalidPalette);
    }
    QVector2D position;
    position.setX(ui->xPositionLineEdit->text().toFloat(&ok));
    if (ok) {
        ui->xPositionLabel->setPalette(this->palette());
        ui->xPositionLineEdit->setPalette(this->palette());
    } else {
        allOk = false;
        ui->xPositionLabel->setPalette(invalidPalette);
        ui->xPositionLineEdit->setPalette(invalidPalette);
    }
    position.setY(ui->yPositionLineEdit->text().toFloat(&ok));
    if (ok) {
        ui->yPositionLabel->setPalette(this->palette());
        ui->yPositionLineEdit->setPalette(this->palette());
    } else {
        allOk = false;
        ui->yPositionLabel->setPalette(invalidPalette);
        ui->yPositionLineEdit->setPalette(invalidPalette);
    }
    QVector2D speed;
    bool isStatic = ui->isStaticCheckBox->isChecked();
    if (isStatic == false) {
        speed.setX(ui->xSpeedLineEdit->text().toFloat(&ok));
        if (ok) {
            ui->xSpeedLabel->setPalette(this->palette());
            ui->xSpeedLineEdit->setPalette(this->palette());
        } else {
            allOk = false;
            ui->xSpeedLabel->setPalette(invalidPalette);
            ui->xSpeedLineEdit->setPalette(invalidPalette);
        }
        speed.setY(ui->ySpeedLineEdit->text().toFloat(&ok));
        if (ok) {
            ui->ySpeedLabel->setPalette(this->palette());
            ui->ySpeedLineEdit->setPalette(this->palette());
        } else {
            allOk = false;
            ui->ySpeedLabel->setPalette(invalidPalette);
            ui->ySpeedLineEdit->setPalette(invalidPalette);
        }
    } else {
        ui->xSpeedLabel->setPalette(this->palette());
        ui->xSpeedLineEdit->setPalette(this->palette());
        ui->ySpeedLabel->setPalette(this->palette());
        ui->ySpeedLineEdit->setPalette(this->palette());
    }
    QColor color = QColor(ui->colorButton->styleSheet().mid(32).left(7));

    if (allOk) {
        if (editing) {
            ui->planetTableWidget->selectedItems()[0]->setText(ui->massLineEdit->text());
            ui->planetTableWidget->selectedItems()[1]->setText(ui->xPositionLineEdit->text());
            ui->planetTableWidget->selectedItems()[2]->setText(ui->yPositionLineEdit->text());
            if (isStatic == false) {
                ui->planetTableWidget->selectedItems()[3]->setText(ui->xSpeedLineEdit->text());
                ui->planetTableWidget->selectedItems()[4]->setText(ui->ySpeedLineEdit->text());
                ui->planetTableWidget->selectedItems()[5]->setText("false");
            } else {
                ui->planetTableWidget->selectedItems()[3]->setText("0");
                ui->planetTableWidget->selectedItems()[4]->setText("0");
                ui->planetTableWidget->selectedItems()[5]->setText("true");
            }
            ui->planetTableWidget->selectedItems()[6]->setBackgroundColor(color);
            orbitCalc->modifyPlanet(ui->planetTableWidget->currentRow(), mass, position, speed, isStatic, color);
            ui->planetTableWidget->clearSelection();
        } else {
            ui->planetTableWidget->insertRow(ui->planetTableWidget->rowCount());
            ui->planetTableWidget->setItem(ui->planetTableWidget->rowCount() - 1, 0, new QTableWidgetItem(ui->massLineEdit->text()));
            ui->planetTableWidget->setItem(ui->planetTableWidget->rowCount() - 1, 1, new QTableWidgetItem(ui->xPositionLineEdit->text()));
            ui->planetTableWidget->setItem(ui->planetTableWidget->rowCount() - 1, 2, new QTableWidgetItem(ui->yPositionLineEdit->text()));
            if (isStatic == false) {
                ui->planetTableWidget->setItem(ui->planetTableWidget->rowCount() - 1, 3, new QTableWidgetItem(ui->xSpeedLineEdit->text()));
                ui->planetTableWidget->setItem(ui->planetTableWidget->rowCount() - 1, 4, new QTableWidgetItem(ui->ySpeedLineEdit->text()));
                ui->planetTableWidget->setItem(ui->planetTableWidget->rowCount() - 1, 5, new QTableWidgetItem("false"));
            } else {
                ui->planetTableWidget->setItem(ui->planetTableWidget->rowCount() - 1, 3, new QTableWidgetItem("0"));
                ui->planetTableWidget->setItem(ui->planetTableWidget->rowCount() - 1, 4, new QTableWidgetItem("0"));
                ui->planetTableWidget->setItem(ui->planetTableWidget->rowCount() - 1, 5, new QTableWidgetItem("true"));
            }
            ui->planetTableWidget->setItem(ui->planetTableWidget->rowCount() - 1, 6, new QTableWidgetItem());
            ui->planetTableWidget->item(ui->planetTableWidget->rowCount() - 1, 6)->setBackgroundColor(color);
            orbitCalc->addPlanet(mass, position, speed, isStatic, color);
        }
        ui->massLineEdit->clear();
        ui->xPositionLineEdit->clear();
        ui->yPositionLineEdit->clear();
        ui->xSpeedLineEdit->clear();
        ui->ySpeedLineEdit->clear();
        ui->isStaticCheckBox->setChecked(false);
        ui->colorButton->setStyleSheet(defaultStyle);
    }
}

void MainWindow::startStopCalculation()
{
    if (orbitCalc->isRunning()) {
        enableControls();
        orbitCalc->stop();
    } else {
        QPalette invalidPalette;
        invalidPalette.setColor(QPalette::WindowText, Qt::red);
        invalidPalette.setColor(QPalette::Text, Qt::red);

        bool ok;
        bool allOk = true;
        double deltaT = ui->deltaTimeLineEdit->text().toDouble(&ok);
        if (ok) {
            ui->deltaTimeLabel->setPalette(this->palette());
            ui->deltaTimeLineEdit->setPalette(this->palette());
        } else {
            allOk = false;
            ui->deltaTimeLabel->setPalette(invalidPalette);
            ui->deltaTimeLineEdit->setPalette(invalidPalette);
        }
        double time = ui->timeLineEdit->text().toDouble(&ok);
        if (ok) {
            ui->timeLabel->setPalette(this->palette());
            ui->timeLineEdit->setPalette(this->palette());
        } else {
            allOk = false;
            ui->timeLabel->setPalette(invalidPalette);
            ui->timeLineEdit->setPalette(invalidPalette);
        }
        QVector2D minBounder;
        minBounder.setX(ui->xMinLineEdit->text().toFloat(&ok));
        if (ok) {
            ui->xMinLabel->setPalette(this->palette());
            ui->xMinLineEdit->setPalette(this->palette());
        } else {
            allOk = false;
            ui->xMinLabel->setPalette(invalidPalette);
            ui->xMinLineEdit->setPalette(invalidPalette);
        }
        minBounder.setY(ui->yMinLineEdit->text().toFloat(&ok));
        if (ok) {
            ui->yMinLabel->setPalette(this->palette());
            ui->yMinLineEdit->setPalette(this->palette());
        } else {
            allOk = false;
            ui->yMinLabel->setPalette(invalidPalette);
            ui->yMinLineEdit->setPalette(invalidPalette);
        }
        QVector2D maxBounder;
        maxBounder.setX(ui->xMaxLineEdit->text().toFloat(&ok));
        if (ok) {
            ui->xMaxLabel->setPalette(this->palette());
            ui->xMaxLineEdit->setPalette(this->palette());
        } else {
            allOk = false;
            ui->xMaxLabel->setPalette(invalidPalette);
            ui->xMaxLineEdit->setPalette(invalidPalette);
        }
        maxBounder.setY(ui->yMaxLineEdit->text().toFloat(&ok));
        if (ok) {
            ui->yMaxLabel->setPalette(this->palette());
            ui->yMaxLineEdit->setPalette(this->palette());
        } else {
            allOk = false;
            ui->yMaxLabel->setPalette(invalidPalette);
            ui->yMaxLineEdit->setPalette(invalidPalette);
        }
        QVector2D resolution;
        resolution.setX(ui->widthLineEdit->text().toFloat(&ok));
        if (ok) {
            ui->widthLabel->setPalette(this->palette());
            ui->widthLineEdit->setPalette(this->palette());
        } else {
            allOk = false;
            ui->widthLabel->setPalette(invalidPalette);
            ui->widthLineEdit->setPalette(invalidPalette);
        }
        resolution.setY(ui->heightLineEdit->text().toFloat(&ok));
        if (ok) {
            ui->heightLabel->setPalette(this->palette());
            ui->heightLineEdit->setPalette(this->palette());
        } else {
            allOk = false;
            ui->heightLabel->setPalette(invalidPalette);
            ui->heightLineEdit->setPalette(invalidPalette);
        }

        if (allOk) {
            ui->controlWidget->setEnabled(false);
            ui->calcButton->setText("Stop Calc");
            orbitCalc->start(deltaT, time, minBounder, maxBounder, resolution);
        }
    }
}

void MainWindow::enablePlanetSpeed(const bool &checked)
{
    if (checked) {
        ui->xSpeedLabel->setEnabled(false);
        ui->xSpeedLineEdit->setEnabled(false);
        ui->ySpeedLabel->setEnabled(false);
        ui->ySpeedLineEdit->setEnabled(false);
    } else {
        ui->xSpeedLabel->setEnabled(true);
        ui->xSpeedLineEdit->setEnabled(true);
        ui->ySpeedLabel->setEnabled(true);
        ui->ySpeedLineEdit->setEnabled(true);
    }
}

void MainWindow::enableControls()
{
    ui->controlWidget->setEnabled(true);
    ui->calcButton->setText("Calc");
}

void MainWindow::calculateResultSize()
{
    bool ok;
    double deltaT = ui->deltaTimeLineEdit->text().toDouble(&ok);
    double time = ui->timeLineEdit->text().toDouble(&ok);
    if (ok && deltaT > 0 && time > deltaT && ui->planetTableWidget->rowCount() > 0) {
        //deltaT + time + minBounder + maxBounder + staticCount + dynamicCount
        float resultSize = sizeof(double) + sizeof(double) + sizeof(QVector2D) + sizeof(QVector2D) + sizeof(unsigned int) + sizeof(unsigned int);
        for (int i = 0; i < ui->planetTableWidget->rowCount(); i++) {
            if (ui->planetTableWidget->item(i, 5)->data(Qt::DisplayRole).toString() == "true") {
                resultSize += sizeof(QVector2D) + sizeof(QRgb);
            } else {
                resultSize += time / deltaT * (sizeof(unsigned short) + sizeof(QVector2D)) + sizeof(QRgb);
            }
        }
        if (resultSize > 1024) {
            resultSize /= 1024;
            if (resultSize > 1024) {
                resultSize /= 1024;
                ui->statusBar->showMessage(QString::number(resultSize) + " Mbytes");
            } else {
                ui->statusBar->showMessage(QString::number(resultSize) + " Kbytes");
            }
        } else {
            ui->statusBar->showMessage(QString::number(resultSize) + " bytes");
        }
    } else {
        ui->statusBar->clearMessage();
    }
}

void MainWindow::editPlanet()
{
    if (ui->planetTableWidget->selectedItems().isEmpty()) {
        editing = false;
        ui->massLineEdit->clear();
        ui->xPositionLineEdit->clear();
        ui->yPositionLineEdit->clear();
        ui->xSpeedLineEdit->clear();
        ui->ySpeedLineEdit->clear();
        ui->isStaticCheckBox->setChecked(false);
        ui->colorButton->setStyleSheet(defaultStyle);
        ui->addEditPlanetButton->setText("Add");
    } else {
        editing = true;
        ui->massLineEdit->setText(ui->planetTableWidget->selectedItems().at(0)->text());
        ui->xPositionLineEdit->setText(ui->planetTableWidget->selectedItems().at(1)->text());
        ui->yPositionLineEdit->setText(ui->planetTableWidget->selectedItems().at(2)->text());
        if (ui->planetTableWidget->selectedItems().at(5)->text() == "false") {
            ui->xSpeedLineEdit->setText(ui->planetTableWidget->selectedItems().at(3)->text());
            ui->ySpeedLineEdit->setText(ui->planetTableWidget->selectedItems().at(4)->text());
            ui->isStaticCheckBox->setChecked(false);
        } else {
            ui->xSpeedLineEdit->clear();
            ui->ySpeedLineEdit->clear();
            ui->isStaticCheckBox->setChecked(true);
        }
        QString style("border: none; background-color: ");
        style.append(ui->planetTableWidget->selectedItems().at(6)->backgroundColor().name() + ";");
        ui->colorButton->setStyleSheet(style);
        ui->addEditPlanetButton->setText("Edit");
    }
}

void MainWindow::showColorDialog()
{
    QColor color = QColorDialog::getColor(QColor(ui->colorButton->styleSheet().mid(32).left(7)), this, "Planet Color");
    if (color.isValid()) {
        QString style("border: none; background-color: ");
        style.append(color.name() + ";");
        ui->colorButton->setStyleSheet(style);
    }
}

void MainWindow::showCalculationStatus()
{
    StatusDialog* dialog = new StatusDialog(this);
    const CalcStatus status = orbitCalc->getCalculationStatus();
    QString msg;
    switch (status.code) {
    case CalcStatus::Ok:
        dialog->showStatus("Calculation Status", "Success", orbitCalc->getPreview());
        break;
    case CalcStatus::OutOfRange:
        msg.append("Out of range:\n");
        for (int i = 0; i < status.values.size(); i++) {
            QVector2D pos = QVector2D(orbitCalc->getDynamicPlanet(status.values.at(i)).currentPositionX, orbitCalc->getDynamicPlanet(status.values.at(i)).currentPositionY);
            msg.append("Planet_" + QString::number(status.values.at(i)) + " (" + QString::number(pos.x()) + ", " + QString::number(pos.y()) + ")\n");
        }
        msg.append("\n");
        msg.append("Full time: " + QString::number(status.time));
        dialog->showStatus("Calculation Status", msg, orbitCalc->getPreview());
        break;
    default:
        break;
    }
    dialog->deleteLater();
}
