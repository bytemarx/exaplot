/*
 * ExaPlot
 * QPlot widget plugin
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "qplotplugin.hpp"
#include "qplot.hpp"

#include <QStringLiteral>
#include <QtPlugin>


QPlotPlugin::QPlotPlugin(QObject* parent)
    : QObject{parent}
{
}


QString
QPlotPlugin::name() const
{
    return QStringLiteral("QPlot");
}


QString
QPlotPlugin::group() const
{
    return QStringLiteral("Custom Widgets");
}


QString
QPlotPlugin::toolTip() const
{
    return QStringLiteral("A plot");
}


QString
QPlotPlugin::whatsThis() const
{
    return this->toolTip();
}


QString
QPlotPlugin::includeFile() const
{
    return QStringLiteral("qplot.hpp");
}


QIcon
QPlotPlugin::icon() const
{
    return {};
}


bool
QPlotPlugin::isContainer() const
{
    return false;
}


QWidget*
QPlotPlugin::createWidget(QWidget* parent)
{
    return new QPlot{parent};
}


bool
QPlotPlugin::isInitialized() const
{
    return true;
}


void
QPlotPlugin::initialize(QDesignerFormEditorInterface*)
{
}


QString
QPlotPlugin::domXml() const
{
    return R"(
<widget class="QPlot" name="plot">
    <property name="geometry">
        <rect>
            <x>0</x>
            <y>0</y>
            <width>400</width>
            <height>400</height>
        </rect>
    </property>
    <property name="title">
        <string>title</string>
    </property>
    <property name="xAxis">
        <string>x-axis</string>
    </property>
    <property name="yAxis">
        <string>y-axis</string>
    </property>
    <property name="plotType">
        <enum>TWODIMEN</enum>
    </property>
</widget>
)";
}
