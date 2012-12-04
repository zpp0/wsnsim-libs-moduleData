/**
 *
 * File: moduleParams.h
 * Description: module description params
 * Author: Yarygin Alexander <yarygin.alexander@gmail.com>
 *
 **/

#ifndef MODULEPARAMS_H
#define MODULEPARAMS_H

#include <QList>
#include <QString>
#include <QMap>
#include <QVariant>

struct ModuleEventParamRaw
{
    QString name;
    QString info;
    QString type;
    quint16 ID;
};

struct ModuleFunctionArgumentRaw
{
    QString name;
    QString type;
    quint16 ID;
};

struct ModuleFunctionRaw
{
    QString name;
    QString info;
    QList<ModuleFunctionArgumentRaw> arguments;
    QString resultType;
};

struct ModuleParamRaw
{
    QString name;
    QString type;
    QString info;
    QString units;
    QVariant defaultValue;
    bool optional;
    QMap<QString, QString> arguments;
};

struct ModuleEventRaw
{
    QString name;
    QString info;
    QList<ModuleEventParamRaw> params;
};

struct ModuleInterfaceRaw
{
    QList<ModuleFunctionRaw> functions;
    QList<ModuleEventRaw> events;
};

struct ModuleDependRaw
{
    QString name;
    QString type;
    QString info;
    bool optional;
    ModuleInterfaceRaw interface;
};

struct ModuleDescriptionRaw
{
    QString name;
    QString author;
    QString type;
    QString fileName;
    QString description;
    QList<ModuleParamRaw> params;
    ModuleInterfaceRaw interface;
    QList<ModuleDependRaw> dependencies;
};

#endif // MODULEPARAMS_H
