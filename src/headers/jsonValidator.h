#ifndef JsonValidator_H
#define JsonValidator_H
#pragma once

#include <QThread>
#include <QObject>
#include <QString>
#include <QStringList>
#include <string>

#include <valijson/adapters/rapidjson_adapter.hpp>
#include <valijson/utils/rapidjson_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
using valijson::Schema;
using valijson::SchemaParser;
using valijson::adapters::RapidJsonAdapter;

class JsonValidator : public QThread
{
    Q_OBJECT

public:
    JsonValidator(QObject *parent = 0);
    ~JsonValidator();

    QString data_dir;
    QStringList data_list;
    void setOpen();

private:
    std::string cur_data_path;
    rapidjson::Document mySchemaDoc;
    Schema mySchema;
    SchemaParser parser;

signals:
    void sendResult(bool);

private slots:
    void setStart();
};

#endif