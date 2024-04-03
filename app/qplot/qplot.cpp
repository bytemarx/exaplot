/*
 * ZetaPlot
 * QPlot widget
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include <QVBoxLayout>

#include "qplot.hpp"


QPlot::QPlot(QWidget* parent)
    : QWidget{parent}
    , m_layout{new QVBoxLayout{this}}
    , m_plot2D{new Plot2D}
    , m_plotColorMap{new PlotColorMap}
    , m_current{Plot::TWODIMEN}
    , m_queued{false}
{
    this->m_layout->addWidget(this->m_plot2D->widget());
}


QPlot::~QPlot()
{
    delete this->m_plot2D;
    delete this->m_plotColorMap;
}


void
QPlot::queue()
{
    this->m_queued = true;
}


void
QPlot::redraw()
{
    if (this->m_queued) {
        this->plot()->replot();
        this->m_queued = false;
    }
}


void
QPlot::clear()
{
    this->m_plot2D->clear();
    this->m_plotColorMap->clear();
}


void
QPlot::setTitle(const QString& title)
{
    this->m_plot2D->setTitle(title);
    this->m_plotColorMap->setTitle(title);
    this->m_queued = true;
}


QString
QPlot::title() const
{
    return this->m_plot2D->title();
}


void
QPlot::setLabelX(const QString& label)
{
    this->m_plot2D->setLabelX(label);
    this->m_plotColorMap->setLabelX(label);
    this->m_queued = true;
}


QString
QPlot::labelX() const
{
    return this->m_plot2D->labelX();
}


void
QPlot::setLabelY(const QString& label)
{
    this->m_plot2D->setLabelY(label);
    this->m_plotColorMap->setLabelY(label);
    this->m_queued = true;
}


QString
QPlot::labelY() const
{
    return this->m_plot2D->labelY();
}


void
QPlot::setType(Plot::Type type)
{
    if (type == this->m_current)
        return;

    auto current = this->plot()->widget();
    this->m_layout->removeWidget(current);
    current->hide();

    switch (type) {
    case Plot::TWODIMEN:
        this->m_layout->addWidget(this->m_plot2D->widget());
        this->m_plot2D->widget()->show();
        break;
    case Plot::COLORMAP:
        this->m_layout->addWidget(this->m_plotColorMap->widget());
        this->m_plotColorMap->widget()->show();
        break;
    default:
        assert(false);
        throw std::runtime_error{QString{"Unexpected plot type: %1"}.arg(type).toStdString()};
    }
    this->m_current = type;
    this->m_queued = true;
}


Plot::Type
QPlot::type() const
{
    return this->m_current;
}


Plot2D*
QPlot::plot2D()
{
    return this->m_plot2D;
}


PlotColorMap*
QPlot::plotColorMap()
{
    return this->m_plotColorMap;
}


Plot*
QPlot::plot()
{
    switch (this->m_current) {
    case Plot::TWODIMEN:
        return this->m_plot2D;
    case Plot::COLORMAP:
        return this->m_plotColorMap;
    default:
        assert(false);
        throw std::runtime_error{QString{"Unexpected plot type: %1"}.arg(this->m_current).toStdString()};
    }
    return nullptr;
}


const Plot*
QPlot::plot() const
{
    return static_cast<const Plot*>(this->plot());
}
