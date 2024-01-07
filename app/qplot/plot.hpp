#pragma once

#include "qcustomplot.h"


class Plot
{
friend class QPlot;

	Q_GADGET

public:
	enum Type {
		TWODIMEN,
		COLORMAP,
	};
	Q_ENUM(Type)

	virtual ~Plot();
	virtual Type type() const = 0;
	QCustomPlot* widget();

protected:
	Plot(
		const QString& title = QString{},
		const QString& labelX = QString{},
		const QString& labelY = QString{}
	);

	void setTitle(const QString&);
	QString title() const;
	void setLabelX(const QString&);
	QString labelX() const;
	void setLabelY(const QString&);
	QString labelY() const;

	QCustomPlot* m_plot;
	QCPTextElement* m_title;
};
