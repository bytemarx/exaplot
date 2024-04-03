/*
 * ZetaPlot
 * 2-dimensional plot
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "plot2d.hpp"


Plot2D::Plot2D(
    const QString& title,
    const QString& labelX,
    const QString& labelY,
    const QCPRange& rangeX,
    const QCPRange& rangeY,
    QCPGraph::LineStyle lineStyle,
    const QCPScatterStyle& scatStyle,
    const QPen& pen,
    bool rescaleAxes
)
    : Plot{title, labelX, labelY}
    , m_graph{m_plot->addGraph()}
    , m_rescaleAxes{rescaleAxes}
{
    if (this->m_graph == nullptr)
        throw std::runtime_error{"Failed to add graph"};
    this->m_plot->xAxis->setRange(rangeX);
    this->m_plot->yAxis->setRange(rangeY);
    this->m_graph->setLineStyle(lineStyle);
    this->m_graph->setScatterStyle(scatStyle);
    this->m_graph->setPen(pen);
}


Plot::Type
Plot2D::type() const
{
    return TWODIMEN;
}


void
Plot2D::clear()
{
    this->m_graph->data()->clear();
}


void
Plot2D::replot()
{
    this->m_plot->replot(QCustomPlot::rpQueuedReplot);
    if (this->m_rescaleAxes)
        this->m_plot->rescaleAxes();
}


void
Plot2D::setRangeX(const QCPRange& range)
{
    this->m_plot->xAxis->setRange(range);
}


const QCPRange
Plot2D::rangeX() const
{
    return this->m_plot->xAxis->range();
}


void
Plot2D::setRangeY(const QCPRange& range)
{
    this->m_plot->yAxis->setRange(range);
}


const QCPRange
Plot2D::rangeY() const
{
    return this->m_plot->yAxis->range();
}


void
Plot2D::setLineStyle(QCPGraph::LineStyle style)
{
    this->m_graph->setLineStyle(style);
}


QCPGraph::LineStyle
Plot2D::lineStyle() const
{
    return this->m_graph->lineStyle();
}


void
Plot2D::setScatStyle(const QCPScatterStyle& style)
{
    this->m_graph->setScatterStyle(style);
}


QCPScatterStyle
Plot2D::scatStyle() const
{
    return this->m_graph->scatterStyle();
}


void
Plot2D::setPen(const QPen& pen)
{
    this->m_graph->setPen(pen);
}


QPen
Plot2D::pen() const
{
    return this->m_graph->pen();
}


void
Plot2D::addData(double x, double y)
{
    this->m_graph->addData(x, y);
}


void
Plot2D::addData(const QVector<double>& x, const QVector<double>& y)
{
    this->m_graph->addData(x, y);
}


void
Plot2D::setRescaleAxes(bool rescaleAxes)
{
    this->m_rescaleAxes = rescaleAxes;
}
