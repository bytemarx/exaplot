#include "qplottabplugin.hpp"
#include "qplottabprivate.hpp"

#include <QStringLiteral>
#include <QtPlugin>


QPlotPlugin::QPlotPlugin(QObject* parent)
    : QObject{parent}
{
}


QString
QPlotPlugin::name() const
{
    return QStringLiteral("QPlotTab");
}


QString
QPlotPlugin::group() const
{
    return QStringLiteral("Custom Widgets");
}


QString
QPlotPlugin::toolTip() const
{
    return QStringLiteral("A plot tab");
}


QString
QPlotPlugin::whatsThis() const
{
    return this->toolTip();
}


QString
QPlotPlugin::includeFile() const
{
    return QStringLiteral("qplottab.hpp");
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
    return new QPlotTab{parent};
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
<ui language="c++">
    <widget class="QPlotTab" name="widget">
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
