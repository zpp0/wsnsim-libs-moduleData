/**
 *
 * File: moduleData.h
 * Author: Yarygin Alexander <yarygin.alexander@gmail.com>
 *
 **/

#ifndef MODULEDATA_H
#define MODULEDATA_H

#include <QtCore>

#include <QtXml>
#include <QDomElement>
#include <QDomDocument>
#include <QDomText>

#include "moduleParams.h"

class ModuleData : public QObject
{
    Q_OBJECT
public:

    ModuleData();

    ModuleDescriptionRaw load(QString& fileName, QString* errorMessage);
    void save(QString& fileName, QString* errorMessage, ModuleDescriptionRaw& params);

private:

    QList<ModuleParamRaw> loadModuleParams(QDomNode dn_node);
    ModuleInterfaceRaw loadModuleInterface(QDomNode dn_node);
    QList<ModuleDependRaw> loadModuleDependencies(QDomNode dn_node);
    QList<ModuleFunctionRaw> loadModuleFunctions(QDomNode dn_node);
    QList<ModuleEventRaw> loadModuleEvents(QDomNode dn_node);

    QMap<QString, QString> loadInfo(QDomNode dn_node);

    void saveModuleInfo(QDomDocument* result, QDomElement* parent, ModuleDescriptionRaw info);

    void createXml(QDomDocument* result, QDomElement* parent, QString XNodeName, QString XNodeValue);
    void createXml(QDomDocument* result, QDomElement* parent, QString XNodeName, QString XNodeValue, QMap<QString, QString> attrs);
};

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT ModuleDescriptionRaw load(QString& fileName,
                                               QString* errorMessage);
extern "C" MY_EXPORT void save(QString& fileName,
                               QString* errorMessage,
                               ModuleDescriptionRaw& params);

#endif // MODULEDATA_H
