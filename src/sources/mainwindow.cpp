#include "mainwindow.h"

#include <iostream>
#include <string>

#include <QCoreApplication>
#include <QFileDialog>
#include <QThread>
#include <QTimer>
#include <QRadioButton>


using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}