#ifndef JsonValidator_H
#define JsonValidator_H
#pragma once

#include <QThread>
#include <QObject>
#include <QString>
#include <QStringList>
#include <string>
#include <QDir>

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

    QDir top_qdir;
    QString top_dir;
    QStringList top_dir_list;
    QStringList whole_data_list;
    QStringList error_data_list;

    int type = 0;
    void setOpen();

private:
    rapidjson::Document mySchemaDoc;
    Schema mySchema;
    SchemaParser parser;
    void getLine(std::string, int, int);

signals:
    void sendResult(bool);
    void sendErrorRate(double);

private slots:
    void setStart();
};

#endif