/*
 * ExaPlot
 * QButtonGrid widget plugin
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "qbuttongridplugin.hpp"
#include "qbuttongrid.hpp"

#include <QStringLiteral>
#include <QtPlugin>


QButtonGridPlugin::QButtonGridPlugin(QObject* parent)
    : QObject{parent}
{
}


QString
QButtonGridPlugin::name() const
{
    return QStringLiteral("QButtonGrid");
}


QString
QButtonGridPlugin::group() const
{
    return QStringLiteral("Custom Widgets");
}


QString
QButtonGridPlugin::toolTip() const
{
    return QStringLiteral("A dynamically resizable grid of push buttons");
}


QString
QButtonGridPlugin::whatsThis() const
{
    return QStringLiteral("A grid of buttons that emits a signal when the grid changes");
}


QString
QButtonGridPlugin::includeFile() const
{
    return QStringLiteral("qbuttongrid.hpp");
}


QIcon
QButtonGridPlugin::icon() const
{
    return {};
}


bool
QButtonGridPlugin::isContainer() const
{
    return false;
}


QWidget*
QButtonGridPlugin::createWidget(QWidget* parent)
{
    return new QButtonGrid{parent};
}


bool
QButtonGridPlugin::isInitialized() const
{
    return true;
}


void
QButtonGridPlugin::initialize(QDesignerFormEditorInterface*)
{
}


QString
QButtonGridPlugin::domXml() const
{
    return R"(
<ui language="c++">
    <widget class="QButtonGrid" name="buttonGrid">
        <property name="geometry">
            <rect>
                <x>0</x>
                <y>0</y>
                <width>400</width>
                <height>400</height>
            </rect>
        </property>
    </widget>
</ui>
)";
}
