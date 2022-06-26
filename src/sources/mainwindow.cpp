#include "mainwindow.h"

#include <iostream>
#include <string>

#include <QCoreApplication>
#include <QFileDialog>
#include <QThread>
#include <QTimer>
#include <QDir>
#include <QStringList>
#include <QRadioButton>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setFunction();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setFunction()
{
	validator = new JsonValidator(this);
	connect(ui->openButton, SIGNAL(clicked()), this, SLOT(setOpen()));
	connect(ui->startButton, SIGNAL(clicked()), validator, SLOT(setStart()));
	connect(validator, SIGNAL(sendResult(bool)), this, SLOT(setResult(bool)));
}

void MainWindow::setOpen()
{

	validator->data_dir = QFileDialog::getExistingDirectory(this, "Select Top Directory of JSON files", QDir::currentPath(), QFileDialog::ShowDirsOnly);
	validator->setOpen();
	ui->listWidget->addItems(validator->data_list);
}

void MainWindow::setResult(bool result)
{
	QString result_txt = result ? "Success" : "Failed";
	ui->resultLabel->setText(result_txt);
}