#include "jsonValidator.h"

#include <iostream>
#include "jsoncpp/json/json.h"
#include <fstream>

#include <QDir>
#include <QChar>

#include <valijson/validator.hpp>
#include <valijson/validation_results.hpp>

using namespace std;

using valijson::ValidationResults;
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

    vector<string> load_result;
    if (!valijson::utils::loadDocument(scheme_data, mySchemaDoc, load_result))
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

        // If Target File Structure is Wrong,
        rapidjson::Document myTargetDoc;
        vector<string> load_result;
        if (!valijson::utils::loadDocument(cur_data_path, myTargetDoc, load_result))
        {
            int pos = data_list.at(i).lastIndexOf(QChar('.'));
            string err_result_path = string(data_dir.toLocal8Bit().constData()) + "/" + string((data_list.at(i).left(pos)).toLocal8Bit().constData()) + "_error.txt";
            ofstream writeFile;
            writeFile.open(err_result_path.c_str());
            for (int i = 0; i < int(load_result.size()); i += 2)
            {
                writeFile << "Error #" << load_result[i] << "\n";
                writeFile << " ";
                writeFile << "  - " << load_result[i + 1] << "\n";
            }

            writeFile.close();
            status = false;
            error_cnt += 1;
            error_data_list << data_list.at(i);
            continue;
        }

        // If Validation Failed,
        RapidJsonAdapter myTargetAdapter(myTargetDoc);
        ValidationResults err_results;
        if (!validator.validate(mySchema, myTargetAdapter, &err_results))
        {
            int pos = data_list.at(i).lastIndexOf(QChar('.'));
            string err_result_path = string(data_dir.toLocal8Bit().constData()) + "/" + string((data_list.at(i).left(pos)).toLocal8Bit().constData()) + "_error.txt";

            ofstream writeFile;
            writeFile.open(err_result_path.c_str());

            ValidationResults::Error error;
            unsigned int errorNum = 1;
            while (err_results.popError(error))
            {
                writeFile << "Error #" << errorNum << "\n";
                writeFile << " ";
                for (const string &contextElement : error.context)
                {
                    writeFile << contextElement << " ";
                }
                writeFile << "\n";
                writeFile << "   - " << error.description << "\n";
                ++errorNum;
            }

            writeFile.close();
            status = false;
            ++error_cnt;
            error_data_list << data_list.at(i);
        }
    }

    double error_rate = double(double(error_cnt) / double(data_list.size())) * double(100);

    emit sendResult(status);
    emit sendErrorRate(error_rate);
}