#ifndef STATUSDIALOG_H
#define STATUSDIALOG_H

#include <QDialog>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QGridLayout>

class StatusDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StatusDialog(QWidget *parent = 0);
    void showStatus(const QString& title, const QString& text, const QImage& preview);

signals:

public slots:

private:
    QLabel textLabel;
    QLabel previewLabel;
};

#endif // STATUSDIALOG_H
