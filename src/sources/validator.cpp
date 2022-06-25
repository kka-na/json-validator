#include "validator.h"

#include <iostream>
#include "jsoncpp/json/json.h"

using namespace std;

Validator::Validator(QObject *parent) : QThread(parent)
{
}

Validator::~Validator()
{
}