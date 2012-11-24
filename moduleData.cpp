/**
 *
 * File: moduleData.cpp
 * Description: methods of loading module data from XML
 * Author: Yarygin Alexander <yarygin.alexander@gmail.com>
 *
 **/

#include "moduleData.h"

ModuleDescriptionRaw* ModuleData::m_module;

ModuleData::ModuleData()
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
    open();
}

void ModuleData::open()
{
    // getting new state
    m_lua = luaL_newstate();

    luaL_openlibs(m_lua);

    lua_register(m_lua, "declare_module", ModuleData::declareModule);
}

void ModuleData::close()
{
    lua_close(m_lua);
}

int ModuleData::loadFile(QString path)
{
    int ret = luaL_dofile(m_lua, path.toUtf8().constData());
    if (ret) {
        *m_errorString = lua_tostring(m_lua, -1);
        return 0;
    }
    return 1;
}

QString ModuleData::getField(lua_State* lua, QString name)
{
    QString value;
    lua_getfield(lua, -1, name.toUtf8().constData());
    if (lua_isstring(lua, -1))
        value = lua_tostring(lua, -1);
    lua_pop(lua, 1);

    return value;
}

QList<ModuleParamRaw> ModuleData::getParams(lua_State* lua)
{
    QList<ModuleParamRaw> params;
    int paramsNum = luaL_getn(lua, -1);
    for (int i = 1; i <= paramsNum; i++) {

        lua_rawgeti(lua, -1, i);

        if (!lua_istable(lua, -1)) {
            lua_pop(lua, 1);
            continue;
        }

        ModuleParamRaw param;
        param.name = getField(lua, "name");
        param.type = getField(lua, "type");
        param.info = getField(lua, "info");
        param.units = getField(lua, "units");

        lua_getfield(lua, -1, "args");

        if (lua_istable(lua, -1)) {
            lua_pushnil(lua);

            while (lua_next(lua, -2)) {
                QString key = lua_tostring(lua, -2);
                QString value = lua_tostring(lua, -1);

                param.arguments[key] = value;
                lua_pop(lua, 1);
            }

            lua_pop(lua, 1);
        }
        else
            lua_pop(lua, 1);

        lua_pop(lua, 1);

        params += param;
    }

    return params;
}

ModuleInterfaceRaw ModuleData::getInterface(lua_State* lua)
{
    ModuleInterfaceRaw interface;

    lua_getfield(lua, -1, "functions");
    if (lua_istable(lua, -1)) {
        int functions = luaL_getn(lua, -1);

        for (int i = 1; i <= functions; i++) {

            lua_rawgeti(lua, -1, i);

            if (!lua_istable(lua, -1)) {
                lua_pop(lua, 1);
                continue;
            }

            ModuleFunctionRaw function;
            function.name = getField(lua, "name");
            function.info = getField(lua, "info");
            function.resultType = getField(lua, "resultType");

            lua_getfield(lua, -1, "args");
            if (lua_istable(lua, -1)) {

                int args = luaL_getn(lua, -1);

                for (int j = 1; j <= args; j++) {

                    lua_rawgeti(lua, -1, j);

                    if (!lua_istable(lua, -1)) {
                        lua_pop(lua, 1);
                        continue;
                    }

                    ModuleFunctionArgumentRaw param;

                    param.ID = j - 1;
                    param.name = getField(lua, "name");
                    param.type = getField(lua, "type");

                    function.arguments += param;

                    lua_pop(lua, 1);
                }
            }
            lua_pop(lua, 2);

            interface.functions += function;
        }
    }
    lua_pop(lua, 1);

    lua_getfield(lua, -1, "events");
    if (lua_istable(lua, -1)) {
        int events = luaL_getn(lua, -1);

        for (int i = 1; i <= events; i++) {

            lua_rawgeti(lua, -1, i);

            if (!lua_istable(lua, -1)) {
                lua_pop(lua, 1);
                continue;
            }

            ModuleEventRaw event;
            event.name = getField(lua, "name");
            event.info = getField(lua, "info");

            lua_getfield(lua, -1, "params");
            if (lua_istable(lua, -1)) {
                int params = luaL_getn(lua, -1);

                for (int j = 1; j <= params; j++) {

                    lua_rawgeti(lua, -1, j);

                    if (!lua_istable(lua, -1)) {
                        lua_pop(lua, 1);
                        continue;
                    }

                    ModuleEventParamRaw param;

                    param.ID = j - 1;
                    param.name = getField(lua, "name");
                    param.info = getField(lua, "info");
                    param.type = getField(lua, "type");

                    event.params += param;

                    lua_pop(lua, 1);
                }
            }
            lua_pop(lua, 2);

            interface.events += event;
        }
    }
    lua_pop(lua, 1);

    return interface;
}

QList<ModuleDependRaw> ModuleData::getDependencies(lua_State* lua)
{
    QList<ModuleDependRaw> dependencies;
    int deps = luaL_getn(lua, -1);

    for (int i = 1; i <= deps; i++) {

        lua_rawgeti(lua, -1, i);

        if (!lua_istable(lua, -1)) {
            lua_pop(lua, 1);
            continue;
        }

        ModuleDependRaw dependence;

        dependence.name = getField(lua, "name");
        dependence.type = getField(lua, "type");

        lua_getfield(lua, -1, "interface");
        if (lua_istable(lua, -1))
            dependence.interface = getInterface(lua);

        lua_pop(lua, 2);

        dependencies += dependence;
    }

    return dependencies;
}

int ModuleData::declareModule(lua_State* lua)
{
    m_module->name = getField(lua, "name");

    m_module->author = getField(lua, "author");
    m_module->type = getField(lua, "type");
    m_module->description = getField(lua, "description");

    lua_getfield(lua, -1, "params");
    if (lua_istable(lua, -1))
        m_module->params = getParams(lua);
    lua_pop(lua, 1);

    lua_getfield(lua, -1, "interface");
    if (lua_istable(lua, -1))
        m_module->interface = getInterface(lua);
    lua_pop(lua, 1);

    lua_getfield(lua, -1, "dependencies");
    if (lua_istable(lua, -1))
        m_module->dependencies = getDependencies(lua);
    lua_pop(lua, 1);

    return 1;
}

ModuleDescriptionRaw ModuleData::load(QString& fileName, QString* errorMessage)
{
    ModuleDescriptionRaw moduleParams;
    m_module = &moduleParams;
    m_module->fileName = fileName;
    m_errorString = errorMessage;

    loadFile(fileName);

    qDebug("-- module --");
    qDebug("  name %s, author %s, type %s, description %s",
           qPrintable(m_module->name),
           qPrintable(m_module->author),
           qPrintable(m_module->type),
           qPrintable(m_module->description));
    qDebug("  -- params --");
    foreach(ModuleParamRaw param, m_module->params) {
        qDebug("    param name %s, type %s, info %s, units %s",
               qPrintable(param.name),
               qPrintable(param.type),
               qPrintable(param.info),
               qPrintable(param.units));
        foreach(QString arg, param.arguments.keys())
            qDebug("      param arg name %s, value %s",
                   qPrintable(arg),
                   qPrintable(param.arguments[arg]));
    }
    qDebug("  -- interface --");
    foreach(ModuleFunctionRaw function, m_module->interface.functions) {
        qDebug("    function name %s, info %s, resultType %s",
               qPrintable(function.name),
               qPrintable(function.info),
               qPrintable(function.resultType));
        foreach(ModuleFunctionArgumentRaw argument, function.arguments)
            qDebug("      argument ID %i, name %s, type %s",
                   argument.ID,
                   qPrintable(argument.name),
                   qPrintable(argument.type));
    }
    foreach(ModuleEventRaw event, m_module->interface.events) {
        qDebug("    event name %s, info %s",
               qPrintable(event.name),
               qPrintable(event.info));
        foreach(ModuleEventParamRaw param, event.params)
            qDebug("      param ID %i, name %s, info %s, type %s",
                   param.ID,
                   qPrintable(param.name),
                   qPrintable(param.info),
                   qPrintable(param.type));
    }
    qDebug("  -- dependencies --");
    foreach(ModuleDependRaw dependence, m_module->dependencies)
        qDebug("    depend name %s, type %s",
               qPrintable(dependence.name),
               qPrintable(dependence.type));

    close();

    return moduleParams;
}

extern "C" MY_EXPORT ModuleDescriptionRaw load(QString& luaFile, QString* errorMessage)
{
    ModuleData moduleData;
    return moduleData.load(luaFile, errorMessage);
}
