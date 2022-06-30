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
	connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(setReset()));
	connect(validator, SIGNAL(sendResult(bool)), this, SLOT(setResult(bool)));
	connect(validator, SIGNAL(sendErrorRate(double)), this, SLOT(setErrorRate(double)));
}

void MainWindow::setOpen()
{
	validator->data_dir = QFileDialog::getExistingDirectory(this, "Select Top Directory of JSON files", QDir::currentPath(), QFileDialog::ShowDirsOnly);
	validator->setOpen();
	ui->listWidget->addItems(validator->data_list);
}

void MainWindow::setResult(bool result)
{
	QString result_txt;
	if (result)
	{
		result_txt = "Success";
		ui->resultLabel->setStyleSheet("QLabel{color: #4cd964;}");
	}
	else
	{
		result_txt = "Failed";
		ui->resultLabel->setStyleSheet("QLabel{color: #ff3b30;}");
	}
	ui->resultLabel->setText(result_txt);
}

void MainWindow::setErrorRate(double error_rate)
{
	ui->errorRateLabel->setText(QString::number(error_rate).append(" %"));
	ui->listWidget_2->addItems(validator->error_data_list);
}

void MainWindow::setReset()
{
	ui->resultLabel->setStyleSheet("QLabel{color: #000000;}");
	ui->listWidget->clear();
	ui->listWidget_2->clear();
	ui->resultLabel->setText("Result");
	ui->errorRateLabel->setText("0 %");
}