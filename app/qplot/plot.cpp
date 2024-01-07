#include "plot.hpp"


Plot::Plot(
    const QString& title,
    const QString& labelX,
    const QString& labelY)
    : m_plot{new QCustomPlot}
    , m_title{new QCPTextElement{m_plot, title}}
{
    this->m_plot->plotLayout()->insertRow(0);
    this->m_plot->plotLayout()->addElement(0, 0, this->m_title);
    this->m_plot->xAxis->setLabel(labelX);
    this->m_plot->yAxis->setLabel(labelY);
    this->m_plot->setContextMenuPolicy(Qt::CustomContextMenu);
    this->m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}


Plot::~Plot()
{
    delete this->m_plot;
}


void
Plot::setTitle(const QString& title)
{
    this->m_title->setText(title);
}


QString
Plot::title() const
{
    return this->m_title->text();
}


void
Plot::setLabelX(const QString& label)
{
    this->m_plot->xAxis->setLabel(label);
}


QString
Plot::labelX() const
{
    return this->m_plot->xAxis->label();
}


void
Plot::setLabelY(const QString& label)
{
    this->m_plot->yAxis->setLabel(label);
}


QString
Plot::labelY() const
{
    return this->m_plot->yAxis->label();
}


QCustomPlot*
Plot::widget()
{
    return this->m_plot;
}
