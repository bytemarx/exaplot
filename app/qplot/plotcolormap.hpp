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
        const QCPColorGradient& color = QCPColorGradient::GradientPreset::gpGrayscale
    );
    Type type() const override;
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
    void clear();

private:
    QCPColorMap* m_map;
    QCPColorScale* m_colorScale;
};
