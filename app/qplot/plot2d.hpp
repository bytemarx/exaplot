/*
 * ZetaPlot
 * 2-dimensional plot
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include <QVector>

#include "plot.hpp"


class Plot2D : public Plot
{
public:
    Plot2D(
        const QString& title = QString{},
        const QString& labelX = QString{},
        const QString& labelY = QString{},
        const QCPRange& rangeX = QCPRange{-10, 10},
        const QCPRange& rangeY = QCPRange{-10, 10},
        QCPGraph::LineStyle lineStyle = QCPGraph::LineStyle::lsNone,
        const QCPScatterStyle& scatStyle = QCPScatterStyle{},
        const QPen& pen = QPen{},
        bool rescaleAxes = true
    );
    Type type() const override;
    void clear() override;
    void replot() override;
    void setRangeX(const QCPRange&);
    const QCPRange rangeX() const;
    void setRangeY(const QCPRange&);
    const QCPRange rangeY() const;
    void setLineStyle(QCPGraph::LineStyle);
    QCPGraph::LineStyle lineStyle() const;
    void setScatStyle(const QCPScatterStyle&);
    QCPScatterStyle scatStyle() const;
    void setPen(const QPen&);
    QPen pen() const;
    void addData(double x, double y);
    void addData(const QVector<double>& x, const QVector<double>& y);
    void setRescaleAxes(bool);

private:
    QCPGraph* m_graph;
    bool m_rescaleAxes;
};
