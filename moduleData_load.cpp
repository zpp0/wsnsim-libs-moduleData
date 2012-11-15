/**
 *
 * File: moduleData_load.cpp
 * Description: methods of loading module data from XML
 * Author: Yarygin Alexander <yarygin.alexander@gmail.com>
 *
 **/

#include "moduleData.h"

ModuleDescriptionRaw ModuleData::load(QString& fileName, QString* errorMessage)
{
    ModuleDescriptionRaw moduleParams;

    QFile file(fileName);

    QDomDocument dd_doc;

    int errorLine;
    int errorColumn;

    if (!file.open(QFile::ReadOnly)) {

        // TODO: generate more readable message
        qDebug("Error open file: %s", qPrintable(file.error()));
        *errorMessage = QString::number(file.error());

        return moduleParams;
    }

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("utf-8"));

    QString content = out.readAll();
    file.close();

    QString errorString;
    if (!dd_doc.setContent(content, true, &errorString, &errorLine, &errorColumn)) {

        *errorMessage = "Error in xml structure file: " + errorString + " line " + errorLine +  " column " + errorColumn;

        qDebug("Error in xml structure file: %s\n line %x\n column %x ",
               qPrintable(*errorMessage), errorLine, errorColumn);

        return moduleParams;
    }

    // переходим к данным XML
    QDomElement de_root = dd_doc.documentElement();

    QDomNode dn_node = de_root.firstChild();

    while (!dn_node.isNull()) {

        if (dn_node.nodeName() == "author")
            moduleParams.author = dn_node.toElement().text();

        if (dn_node.nodeName() == "name")
            moduleParams.name = dn_node.toElement().text();

        if (dn_node.nodeName() == "type")
            moduleParams.type = dn_node.toElement().text();

        if (dn_node.nodeName() == "version")
            moduleParams.version = dn_node.toElement().text();

        if (dn_node.nodeName() == "shortDescription")
            moduleParams.shortDescription = dn_node.toElement().text();

        if (dn_node.nodeName() == "description")
            moduleParams.description = dn_node.toElement().text();

        if (dn_node.nodeName() == "params")
            moduleParams.params = loadModuleParams(dn_node);

        if (dn_node.nodeName() == "interface")
            moduleParams.interface = loadModuleInterface(dn_node);

        if (dn_node.nodeName() == "dependencies")
            moduleParams.dependencies = loadModuleDependencies(dn_node);

        dn_node = dn_node.nextSibling();
    }

    qDebug("end");

    return moduleParams;
}

QList<ModuleParamRaw> ModuleData::loadModuleParams(QDomNode dn_node)
{
    QList<ModuleParamRaw> params;

    QDomNode dn_nextNode = dn_node.firstChild();

    // перебираем узлы, пока не закончатся
    while (!dn_nextNode.isNull()) {

        if (dn_nextNode.nodeName() == "param") {
            ModuleParamRaw param;

            QMap<QString, QString> paramInfo = loadInfo(dn_nextNode);

            param.name = paramInfo["name"];
            param.type = paramInfo["type"];

            QDomNode dn_paramNode = dn_nextNode.firstChild();

            while (!dn_paramNode.isNull()) {
                if (dn_paramNode.nodeName() == "args") {
                    QDomNode dn_argNode = dn_paramNode.firstChild();

                    while (!dn_argNode.isNull()) {
                        if (dn_argNode.nodeName() == "arg") {
                            QMap<QString, QString> attrs = loadInfo(dn_argNode);
                            param.arguments[attrs["name"]] = dn_argNode.toElement().text();
                        }
                        dn_argNode = dn_argNode.nextSibling();
                    }
                }

                if (dn_paramNode.nodeName() == "info") {
                    param.info = dn_paramNode.toElement().text();
                }

                if (dn_paramNode.nodeName() == "units") {
                    param.units = dn_paramNode.toElement().text();
                }

                dn_paramNode = dn_paramNode.nextSibling();
            }
            params += param;
        }

        dn_nextNode = dn_nextNode.nextSibling();
    }

    return params;
}

ModuleInterfaceRaw ModuleData::loadModuleInterface(QDomNode dn_node)
{
    ModuleInterfaceRaw interface;

    QDomNode dn_nextNode = dn_node.firstChild();

    while (!dn_nextNode.isNull()) {
        if (dn_nextNode.nodeName() == "functions")
            interface.functions = loadModuleFunctions(dn_nextNode);
        if (dn_nextNode.nodeName() == "events")
            interface.events = loadModuleEvents(dn_nextNode);

        dn_nextNode = dn_nextNode.nextSibling();
    }

    return interface;
}

QList<ModuleDependRaw> ModuleData::loadModuleDependencies(QDomNode dn_node)
{
    QList<ModuleDependRaw> dependencies;

    QDomNode dn_nextNode = dn_node.firstChild();
    while (!dn_nextNode.isNull()) {

        if (dn_nextNode.nodeName() == "depend") {
            ModuleDependRaw dep;

            QMap<QString, QString> attrs = loadInfo(dn_nextNode);

            dep.name = attrs["name"];
            dep.type = attrs["type"];

            QDomNode dn_interfaceNode = dn_nextNode.firstChild();
            if (dn_interfaceNode.nodeName() == "interface")
                dep.interface = loadModuleInterface(dn_interfaceNode);

            dependencies += dep;
        }

        dn_nextNode = dn_nextNode.nextSibling();
    }

    return dependencies;
}

QList<ModuleFunctionRaw> ModuleData::loadModuleFunctions(QDomNode dn_node)
{
    QList<ModuleFunctionRaw> functions;
    QDomNode dn_nextNode = dn_node.firstChild();
    while (!dn_nextNode.isNull()) {
        if (dn_nextNode.nodeName() == "function") {
            ModuleFunctionRaw func;

            QMap<QString, QString> attr = loadInfo(dn_nextNode);
            func.name = attr["name"];

            QDomNode dn_funcNode = dn_nextNode.firstChild();
            while (!dn_funcNode.isNull()) {
                if (dn_funcNode.nodeName() == "info")
                    func.info = dn_funcNode.toElement().text();

                if (dn_funcNode.nodeName() == "result")
                    func.result = dn_funcNode.toElement().text();

                if (dn_funcNode.nodeName() == "args") {
                    QList<ModuleFunctionArgumentRaw> arguments;
                    QDomNode dn_argNode = dn_funcNode.firstChild();
                    while (!dn_argNode.isNull()) {
                        if (dn_argNode.nodeName() == "arg") {
                            ModuleFunctionArgumentRaw argument;
                            QMap<QString, QString> attrs = loadInfo(dn_argNode);

                            argument.name = attrs["name"];
                            argument.type = attrs["type"];
                            argument.ID = attrs["ID"].toUInt();

                            arguments += argument;
                        }

                        dn_argNode = dn_argNode.nextSibling();
                    }
                    func.arguments = arguments;
                }

                dn_funcNode = dn_funcNode.nextSibling();
            }
            functions += func;
        }

        dn_nextNode = dn_nextNode.nextSibling();
    }

    return functions;
}

QList<ModuleEventRaw> ModuleData::loadModuleEvents(QDomNode dn_node)
{
    QList<ModuleEventRaw> events;
    QDomNode dn_nextNode = dn_node.firstChild();
    while (!dn_nextNode.isNull()) {
        if (dn_nextNode.nodeName() == "event") {
            ModuleEventRaw event;
            QMap<QString, QString> attrs = loadInfo(dn_nextNode);

            event.name = attrs["name"];

            QDomNode dn_eventNode = dn_nextNode.firstChild();
            while (!dn_eventNode.isNull()) {
                if (dn_eventNode.nodeName() == "info")
                    event.info = dn_eventNode.toElement().text();

                if (dn_eventNode.nodeName() == "arguments") {
                    QList<ModuleEventArgumentRaw> arguments;
                    QDomNode dn_argNode = dn_eventNode.firstChild();

                    while (!dn_argNode.isNull()) {
                        if (dn_argNode.nodeName() == "argument") {
                            ModuleEventArgumentRaw arg;
                            QMap<QString, QString> attrs = loadInfo(dn_argNode);

                            arg.name = attrs["name"];
                            arg.type = attrs["type"];
                            arg.ID = attrs["ID"].toUInt();
                            arg.info = dn_argNode.toElement().text();

                            arguments += arg;
                        }
                        dn_argNode = dn_argNode.nextSibling();
                    }
                    event.arguments = arguments;
                }

                dn_eventNode = dn_eventNode.nextSibling();
            }
            events += event;
        }

        dn_nextNode = dn_nextNode.nextSibling();
    }

    return events;
}

QMap<QString, QString> ModuleData::loadInfo(QDomNode dn_node)
{
    QMap<QString, QString> info;

    if (dn_node.hasAttributes()) {

        QDomNamedNodeMap attributes = dn_node.attributes();

        for (uint index = 0; index < attributes.length(); index++) {
            QDomAttr attr = attributes.item(index).toAttr();
            info[attr.name()] = attr.value();
        }
    }

    return info;
}
