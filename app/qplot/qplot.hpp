#pragma once


/*

Two-dimensional plots and color maps are each a type of plot:

              +-------------+
              |    Plot     |
              +-------------+
           ,---------^---------.
           v                   v
    +-------------+     +-------------+
    |   2D Plot   |     |  Color Map  |
    +-------------+     +-------------+

A QPlot is a container, or rather a "frame", for displaying a
plot. It contains each type of plot:

        +-------------------+
        |       QPlot       |
        |    +-----------+  |
        |    | Color Map |  |
        |  +-----------+ |  |
        |  |  2D Plot  |-+  |
        |  |           |    |
        |  +-----------+    |
        +-------------------+

*/


#include <QWidget>

#include "plot2d.hpp"
#include "plotcolormap.hpp"


class QPlot : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString xAxis READ labelX WRITE setLabelX)
    Q_PROPERTY(QString yAxis READ labelY WRITE setLabelY)
    Q_PROPERTY(Plot::Type plotType READ type WRITE setType)

public:
    using Type = Plot::Type;

    explicit QPlot(QWidget* parent = Q_NULLPTR);
    QPlot(const QPlot&) = delete;
    ~QPlot();

    void queue();
    void redraw();
    void clear();
	void setTitle(const QString&);
	QString title() const;
	void setLabelX(const QString&);
	QString labelX() const;
	void setLabelY(const QString&);
	QString labelY() const;
    void setType(Plot::Type plot);
    Plot::Type type() const;
    Plot2D* plot2D();
    const Plot2D* plot2D() const;
    PlotColorMap* plotColorMap();
    const PlotColorMap* plotColorMap() const;

private:
    Plot* plot();
    const Plot* plot() const;

    QVBoxLayout* m_layout;
    Plot2D* m_plot2D;
    PlotColorMap* m_plotColorMap;
    Plot::Type m_current;
    bool m_queued;
};
