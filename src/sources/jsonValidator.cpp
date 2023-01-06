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

string JsonValidator::getErrorValue(rapidjson::FileReadStream is, vector<string> context)
{
    rapidjson::Document doc;
    doc.ParseStream(is);
    string return_value = context.back();
    if (doc.IsObject())
    {
        rapidjson::Value obj = doc.GetObject();
        if (context.size() > 1)
        {
            for (int i = 1; i < int(context.size()); i++)
            {
                if (obj.HasMember(context[i].c_str()))
                {
                    if (obj[context[i].c_str()].IsObject())
                    {
                        obj = obj[context[i].c_str()].GetObject();
                        return_value = context[i];
                    }
                    else if (obj[context[i].c_str()].IsArray())
                    {
                        if (i + 1 < int(context.size()))
                        {
                            int ccc = 0;
                            for (auto &arr : obj[context[i].c_str()].GetArray())
                            {
                                if (ccc == stoi(context[i + 1]))
                                {
                                    if (arr.IsString())
                                    {
                                        return_value = arr.GetString();
                                        break;
                                    }
                                    else if (arr.IsInt())
                                    {
                                        return_value = arr.GetInt();
                                        break;
                                    }
                                    else if (arr.IsNumber())
                                    {
                                        return_value = arr.GetInt();
                                        break;
                                    }
                                }
                                ++ccc;
                            }
                        }
                        else
                        {
                            return_value = context[i];
                            break;
                        }
                    }
                    else if (obj[context[i].c_str()].IsString())
                    {
                        return_value = obj[context[i].c_str()].GetString();
                        break;
                    }
                    else if (obj[context[i].c_str()].IsInt())
                    {
                        return_value = obj[context[i].c_str()].GetInt();
                        break;
                    }
                    else if (obj[context[i].c_str()].IsNumber())
                    {
                        return_value = obj[context[i].c_str()].GetInt();
                        break;
                    }
                }
            }
        }
    }
    else
    {
        return return_value;
    }

    return return_value;
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

        ofstream writeFile;
        string whole_error_path = string(result_dir.toLocal8Bit().constData()) + "/" + string(data_qdir.dirName().toLocal8Bit().constData()) + "_error_report.csv";
        cout << string(data_qdir.dirName().toLocal8Bit().constData()) << endl;
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

                writeFile.open(whole_error_path.c_str(), ios::app);

                for (int k = 0; k < int(load_result.size()); k += 3)
                {
                    writeFile << string((data_list.at(j).left(pos)).toLocal8Bit().constData()) << ", ";
                    writeFile << getLine(cur_data_path, stoi(load_result[k + 1])) << " , ";
                    writeFile << "구조 오류, ";
                    writeFile << load_result[k + 2] << ", ";
                    writeFile << load_errors[stoi(load_result[k])] << "\n";
                }

                status = false;
                error_cnt += 1;
                error_data_list << data_list.at(j);
                error_data_path_list << data_qdir.filePath(data_list.at(j));
                writeFile.close();
                continue;
                
            }

            // If Validation Failed,
            RapidJsonAdapter myTargetAdapter(myTargetDoc);
            ValidationResults err_results;
            if (!validator.validate(mySchema, myTargetAdapter, &err_results))
            {
                FILE *fp = fopen(cur_data_path.c_str(), "rb");
                char readBuffer[65536];
                rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

                writeFile.open(whole_error_path.c_str(), ios::app);
                error_detected = true;
                int pos = cur_data_qpath.lastIndexOf(QChar('.'));

                ValidationResults::Error error;
                while (err_results.popError(error))
                {
                    writeFile << string((data_list.at(j).left(pos)).toLocal8Bit().constData()) << ", /";
                    vector<string> context;
                    for (const string &contextElement : error.context)
                    {
                        writeFile << contextElement << "/";
                        context.push_back(contextElement);
                    }
                    writeFile << ", ";
                    writeFile << "구문오류, ";
                    writeFile << string(getErrorValue(is, context)) << ", ";
                    writeFile << error.description << "\n";
                }

                status = false;
                ++error_cnt;
                error_data_list << data_list.at(j);
                error_data_path_list << data_qdir.filePath(data_list.at(j));
                fclose(fp);
                writeFile.close();
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