#include "statusdialog.h"

StatusDialog::StatusDialog(QWidget *parent) :
    QDialog(parent)
{
    QGridLayout* grid = new QGridLayout(this);
    grid->addWidget(&textLabel, 0, 0);
    grid->addWidget(&previewLabel, 1, 0);
    previewLabel.setStyleSheet("border: none; background-color: #000000;");
}

void StatusDialog::showStatus(const QString &title, const QString &text, const QImage &preview)
{
    setWindowTitle(title);
    textLabel.setText(text);
    previewLabel.setPixmap(QPixmap::fromImage(preview));
    exec();
}
