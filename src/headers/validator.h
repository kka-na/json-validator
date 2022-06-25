#ifndef Validator_H
#define Validator_H
#pragma once

#include <QThread>
#include <QObject>
#include <QImage>
#include <chrono>
#include <string>

class Validator : public QThread
{
    Q_OBJECT

public:
    Validator(QObject *parent = 0);
    ~Validator();
};

#endif