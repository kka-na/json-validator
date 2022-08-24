#include "mainwindow.h"

#include <iostream>
#include <string>

#include <QCoreApplication>
#include <QFileDialog>
#include <QThread>
#include <QTimer>
#include <QDir>
#include <QDebug>
#include <QStringList>

#include <QRadioButton>
#include <QGroupBox>

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
	connect(ui->actionAbout_json_validator, SIGNAL(triggered()), this, SLOT(setAbout()));
	connect(validator, SIGNAL(sendResult(bool)), this, SLOT(setResult(bool)));
	connect(validator, SIGNAL(sendErrorRate(double)), this, SLOT(setErrorRate(double)));
	connect(validator, SIGNAL(updateSlider(int)), this, SLOT(updateSlider(int)));
	connect(validator, SIGNAL(updateErrorList(QString)), this, SLOT(updateErrorList(QString)));
}

void MainWindow::setOpen()
{
	validator->type = ui->ssRadioButton->isChecked() ? 0 : 1;
	validator->top_dir = QFileDialog::getExistingDirectory(this, "Select Top Directory of JSON files", QDir::currentPath(), QFileDialog::ShowDirsOnly);
	validator->setOpen();
	ui->listWidget->addItems(validator->whole_data_list);
	ui->processSlider->setRange(0, validator->whole_data_size);
}

void MainWindow::setAbout()
{
	AboutWindow *aw = new AboutWindow();
	aw->setModal(true);
	aw->show();
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
}

void MainWindow::setReset()
{
	ui->resultLabel->setStyleSheet("QLabel{color: #000000;}");
	ui->listWidget->clear();
	ui->listWidget_2->clear();
	ui->resultLabel->setText("Result");
	ui->errorRateLabel->setText("0 %");
	ui->processSlider->setValue(0);
}

void MainWindow::updateSlider(int tic)
{
	ui->processSlider->setValue(tic);
	QCoreApplication::processEvents();
}

void MainWindow::updateErrorList(QString error_list)
{
	ui->listWidget_2->addItem(error_list);
	QCoreApplication::processEvents();
}