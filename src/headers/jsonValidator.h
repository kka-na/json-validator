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

#include "rapidjson/document.h"
#include <fstream>
#include "rapidjson/filereadstream.h"
#include <cstdio>

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
    QDir result_qdir;
    QString top_dir;
    QString result_dir;
    QStringList top_dir_list;
    QStringList whole_data_list;
    QStringList error_data_list;
    QStringList error_data_path_list;

    int type = 0;
    int whole_data_size = 0;
    int whole_data_cnt = 0;
    void setOpen();

private:
    rapidjson::Document mySchemaDoc;
    Schema mySchema;
    SchemaParser parser;
    int getLine(std::string, int);
    std::string getErrorValue(rapidjson::FileReadStream, std::vector<std::string>);

    std::string load_errors[18] = {
        "오류가 없습니다.",
        "문서가 비어 있습니다.",
        "문서 루트 뒤에 다른 값이 따라오면 안 됩니다.",
        "잘못된 값.",
        "개체 구성원의 이름이 없습니다.",
        "개체 구성원의 이름 뒤에 콜론이 없습니다.",
        "개체 구성원 뒤에 쉼표 또는 '}' 가 없습니다",
        "배열 요소 뒤에 쉼표 또는 ']' 가 없습니다.",
        "문자열에서 이스케이프 뒤에 잘못된 16진수가 있습니다.",
        "문자열의 서로게이트 쌍이 잘못되었습니다.",
        "문자열에 잘못된 이스케이프 문자가 있습니다.",
        "문자열에 닫는 따옴표가 없습니다.",
        "문자열의 인코딩이 잘못되었습니다.",
        "더블에 저장하기에는 숫자가 너무 큽니다.",
        "숫자의 미스 프랙션 부분.",
        "숫자의 지수가 누락되었습니다.",
        "구문 분석이 종료되었습니다.",
        "불특정 구문 오류입니다."};

signals:
    void updateErrorList(QString);
    void updateSlider(int);
    void sendResult(bool);
    void sendErrorRate(double);

private slots:
    void setStart();
};

#endif