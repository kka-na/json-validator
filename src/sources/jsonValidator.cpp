#include "jsonValidator.h"

#include <iostream>
#include "jsoncpp/json/json.h"

#include <QDir>

#include <valijson/validator.hpp>

using namespace std;

using valijson::Validator;

JsonValidator::JsonValidator(QObject *parent) : QThread(parent)
{
}

JsonValidator::~JsonValidator()
{
}

void JsonValidator::setOpen()
{
    QDir data_qdir(data_dir);
    data_list = data_qdir.entryList(QStringList() << "*.json");

    string scheme_data = string(data_dir.toLocal8Bit().constData()) + string("/scheme/scheme.json");

    if (!valijson::utils::loadDocument(scheme_data, mySchemaDoc))
    {
        throw std::runtime_error("Failed to load schema document");
    }

    RapidJsonAdapter mySchemaAdapter(mySchemaDoc);
    parser.populateSchema(mySchemaAdapter, mySchema);
}

void JsonValidator::setStart()
{
    Validator validator;
    bool status = true;
    int error_cnt = 0;

    for (int i = 0; i < data_list.size(); i++)
    {
        cur_data_path = string(data_dir.toLocal8Bit().constData()) + "/" + string((data_list.at(i)).toLocal8Bit().constData());

        rapidjson::Document myTargetDoc;
        // If Target File Structure is Wrong,
        if (!valijson::utils::loadDocument(cur_data_path, myTargetDoc))
        {
            status = false;
            error_cnt += 1;
            error_data_list << data_list.at(i);
            continue;
        }

        RapidJsonAdapter myTargetAdapter(myTargetDoc);
        // If Validation Failed,
        if (!validator.validate(mySchema, myTargetAdapter, NULL))
        {
            status = false;
            error_cnt += 1;
            error_data_list << data_list.at(i);
        }
    }

    double error_rate = double(double(error_cnt) / double(data_list.size())) * double(100);

    emit sendResult(status);
    emit sendErrorRate(error_rate);
}