#include "jsonValidator.h"

#include <iostream>
#include "jsoncpp/json/json.h"
#include <fstream>
#include <chrono>

#include <QChar>
#include <QDebug>

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
    whole_data_size = 0;
    whole_data_cnt = 0;
    top_qdir = QDir(top_dir);
    top_dir_list = top_qdir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs);

    result_dir = QString(top_dir.append("/result"));
    result_qdir = QDir(result_dir);
    if (!result_qdir.exists())
        result_qdir.mkpath(".");
    // Just Display Lists
    for (int i = 0; i < top_dir_list.size(); i++)
    {
        QString data_dir = top_qdir.filePath(top_dir_list.at(i));
        QDir data_qdir(data_dir);
        QStringList data_list = data_qdir.entryList(QStringList() << "*.json");
        whole_data_list << top_dir_list.at(i);
        whole_data_list << data_list;
        whole_data_size += data_list.size();
    }

    string scheme_data = !type ? "../data/scheme/2dscheme.json" : "../data/scheme/2d3dscheme.json";
    vector<string> load_result;
    if (!valijson::utils::loadDocument(scheme_data, mySchemaDoc, load_result))
    {
        throw std::runtime_error("Failed to load schema document");
    }

    RapidJsonAdapter mySchemaAdapter(mySchemaDoc);
    parser.populateSchema(mySchemaAdapter, mySchema);
}

int JsonValidator::getLine(std::string path, int beg)
{
    ifstream file(path.c_str());
    char *buffer;
    int cnt = 0;
    int whole_offset = 0;
    while (file.getline(buffer, beg))
    {
        cnt += 1;
        whole_offset += strlen(buffer) + 1;
        if (whole_offset >= beg)
            break;
    }
    return cnt;
}

void JsonValidator::setStart()
{
    Validator validator;
    bool status = true;
    int error_cnt = 0;
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < top_dir_list.size(); i++)
    {
        QString data_dir = top_qdir.filePath(top_dir_list.at(i));
        QDir data_qdir(data_dir);
        QStringList data_list = data_qdir.entryList(QStringList() << "*.json");

        for (int j = 0; j < data_list.size(); j++)
        {
            bool error_detected = false;
            QString cur_data_qpath = data_qdir.filePath(data_list.at(j));
            string cur_data_path = cur_data_qpath.toLocal8Bit().constData();

            // If Target File Structure is Wrong,
            rapidjson::Document myTargetDoc;
            vector<string> load_result;
            if (!valijson::utils::loadDocument(cur_data_path, myTargetDoc, load_result))
            {
                error_detected = true;
                int pos = data_list.at(j).lastIndexOf(QChar('.'));
                string err_result_path = string(result_dir.toLocal8Bit().constData()) + "/" + string((data_list.at(j).left(pos)).toLocal8Bit().constData()) + "_error.txt";

                ofstream writeFile;
                writeFile.open(err_result_path.c_str());
                for (int k = 0; k < int(load_result.size()); k += 3)
                {
                    writeFile << "Error #" << load_result[k];
                    writeFile << " at Line [ " << getLine(cur_data_path, stoi(load_result[k + 1])) << " ]\n";
                    writeFile << load_result[k + 2] << "\n- Parse Error \n";
                }

                writeFile.close();
                status = false;
                error_cnt += 1;
                error_data_list << data_list.at(j);
                error_data_path_list << data_qdir.filePath(data_list.at(j));
                continue;
            }

            // If Validation Failed,
            RapidJsonAdapter myTargetAdapter(myTargetDoc);
            ValidationResults err_results;
            if (!validator.validate(mySchema, myTargetAdapter, &err_results))
            {
                error_detected = true;
                int pos = cur_data_qpath.lastIndexOf(QChar('.'));
                string err_result_path = string(result_dir.toLocal8Bit().constData()) + "/" + string((data_list.at(j).left(pos)).toLocal8Bit().constData()) + "_error.txt";

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
                error_data_list << data_list.at(j);
                error_data_path_list << data_qdir.filePath(data_list.at(j));
            }
            if (error_detected)
                emit updateErrorList(data_list.at(j));
            whole_data_cnt += 1;
            emit updateSlider(whole_data_cnt);
        }
    }

    double error_rate = double(double(error_cnt) / double(whole_data_size)) * double(100);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    string result_path = string(result_dir.toLocal8Bit().constData()) + "/result.txt";
    ofstream writeFile;
    writeFile.open(result_path.c_str());
    writeFile << top_dir.toLocal8Bit().constData() << " \n";
    writeFile << "Whole Files : " << whole_data_size << " , Error Files : " << error_cnt << " \n";
    writeFile << "Error Rate : " << error_rate << " %\n";
    writeFile << "Checking Time : " << duration.count() << " msec \n";

    for (int i = 0; i < int(error_data_path_list.size()); i++)
    {
        writeFile << string(error_data_path_list.at(i).toLocal8Bit().constData()) << "\n";
    }
    emit sendResult(status);
    emit sendErrorRate(error_rate);
}