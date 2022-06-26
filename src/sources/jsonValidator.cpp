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
    for (int i = 0; i < data_list.size(); i++)
    {
        cur_data_path = string(data_dir.toLocal8Bit().constData()) + "/" + string((data_list.at(i)).toLocal8Bit().constData());
        rapidjson::Document myTargetDoc;
        if (!valijson::utils::loadDocument(cur_data_path, myTargetDoc))
        {
            throw std::runtime_error("Failed to load target document");
        }

        RapidJsonAdapter myTargetAdapter(myTargetDoc);
        emit sendResult(validator.validate(mySchema, myTargetAdapter, NULL));
    }
}