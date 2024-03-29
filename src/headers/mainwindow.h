#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "jsonValidator.h"

#include "aboutwindow.h"

#include <string>

#include <QMainWindow>
#include <QObject>
#include <QLabel>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    class JsonValidator::JsonValidator *validator;

private:
    void setFunction();

private slots:
    void setOpen();
    void setAbout();
    void setResult(bool);
    void setErrorRate(double);
    void setReset();
    void updateSlider(int);
    void updateErrorList(QString);
};
#endif // MAINWINDOW_H