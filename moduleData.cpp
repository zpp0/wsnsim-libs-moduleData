/**
 *
 * File: moduleData.cpp
 * Description: methods of loading module data from XML
 * Author: Yarygin Alexander <yarygin.alexander@gmail.com>
 *
 **/

#include "moduleData.h"

ModuleData::ModuleData()
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
}

extern "C" MY_EXPORT ModuleDescriptionRaw load(QString& projectFileName, QString* errorMessage)
{
    ModuleData moduleData;
    return moduleData.load(projectFileName, errorMessage);
}

extern "C" MY_EXPORT void save(QString& projectFileName,
                               QString* errorMessage,
                               ModuleDescriptionRaw& params)
{
    ModuleData moduleData;
    moduleData.save(projectFileName, errorMessage, params);
}

