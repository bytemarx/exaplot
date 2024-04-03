/*
 * ExaPlot
 * colormap plot
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include "plot.hpp"

#include <optional>


class PlotColorMap : public Plot
{
public:
    PlotColorMap(
        const QString& title = QString{},
        const QString& labelX = QString{},
        const QString& labelY = QString{},
        const QString& labelZ = QString{},
        const QCPRange& rangeX = QCPRange{-10, 10},
        const QCPRange& rangeY = QCPRange{-10, 10},
        const QCPRange& rangeZ = QCPRange{0, 1},
        int sizeX = 21,
        int sizeY = 21,
        const QCPColorGradient& color = QCPColorGradient::GradientPreset::gpGrayscale,
        bool rescaleAxes = true,
        bool rescaleData = true
    );
    Type type() const override;
    void clear() override;
    void replot() override;
    void setRangeX(const QCPRange&);
    QCPRange rangeX() const;
    void setRangeY(const QCPRange&);
    QCPRange rangeY() const;
    void setRangeZ(const QCPRange&);
    QCPRange rangeZ() const;
    void setDataSize(int, int);
    int getDataSizeX() const;
    int getDataSizeY() const;
    void setColorGradient(const QCPColorGradient&);
    QCPColorGradient colorGradient() const;
    void setCell(int, int, double);
    void setRescaleAxes(bool);
    void setRescaleData(bool);

private:
    QCPColorMap* m_map;
    QCPColorScale* m_colorScale;
    bool m_rescaleAxes;
    bool m_rescaleData;
};
