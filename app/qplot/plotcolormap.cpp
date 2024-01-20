#include "plotcolormap.hpp"


PlotColorMap::PlotColorMap(
    const QString& title,
    const QString& labelX,
    const QString& labelY,
    const QString& labelZ,
    const QCPRange& rangeX,
    const QCPRange& rangeY,
    const QCPRange& rangeZ,
    int sizeX,
    int sizeY,
    const QCPColorGradient& color
)
    : Plot{title, labelX, labelY}
    , m_map{new QCPColorMap{m_plot->xAxis, m_plot->yAxis}}
    , m_colorScale{new QCPColorScale{m_plot}}
{
    QCPMarginGroup* marginGroup = new QCPMarginGroup{this->m_plot};
    this->m_plot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
    this->m_plot->axisRect()->setupFullAxesBox(true);
    this->m_map->setGradient(color);
    this->m_map->data()->setSize(sizeX, sizeY);
    this->m_map->data()->setRange(rangeX, rangeY);
    this->m_map->rescaleDataRange();
    this->m_colorScale->setType(QCPAxis::atRight);
    this->m_colorScale->setDataRange(rangeZ);
    this->m_colorScale->axis()->setLabel(labelZ);
    this->m_colorScale->axis()->axisRect()->setRangeDrag(
        Qt::Orientation::Horizontal | Qt::Orientation::Vertical);
    this->m_colorScale->axis()->axisRect()->setRangeZoom(
        Qt::Orientation::Horizontal | Qt::Orientation::Vertical);
    this->m_colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
}


Plot::Type
PlotColorMap::type() const
{
    return COLORMAP;
}


void
PlotColorMap::clear()
{
    this->m_map->data()->clear();
}


void
PlotColorMap::setRangeX(const QCPRange& range)
{
    this->m_map->data()->setKeyRange(range);
}


QCPRange
PlotColorMap::rangeX() const
{
    return this->m_map->data()->keyRange();
}


void
PlotColorMap::setRangeY(const QCPRange& range)
{
    this->m_map->data()->setValueRange(range);
}


QCPRange
PlotColorMap::rangeY() const
{
    return this->m_map->data()->valueRange();
}


void
PlotColorMap::setRangeZ(const QCPRange& range)
{
    this->m_colorScale->setDataRange(range);
}


QCPRange
PlotColorMap::rangeZ() const
{
    return this->m_colorScale->dataRange();
}


void
PlotColorMap::setDataSize(int x, int y)
{
    this->m_map->data()->setSize(x, y);
}


int
PlotColorMap::getDataSizeX() const
{
    return this->m_map->data()->keySize();
}


int
PlotColorMap::getDataSizeY() const
{
    return this->m_map->data()->valueSize();
}


void
PlotColorMap::setColorGradient(const QCPColorGradient& colorGradient)
{
    this->m_map->setGradient(colorGradient);
}


QCPColorGradient
PlotColorMap::colorGradient() const
{
    return this->m_map->gradient();
}


void
PlotColorMap::setCell(int x, int y, double z)
{
    this->m_map->data()->setCell(x, y, z);
}
