#pragma once

#include "ui_ploteditor.h"
#include "qplottab.hpp"
#include "qplot.hpp"

#include <vector>


class PlotEditor : public QDialog
{
    Q_OBJECT

public:
    typedef struct
    {
        orbital::GridPoint position;
        QPlotTab::Cache attributes;
        QPlot::Type selected;
    } PlotInfo;

    PlotEditor(QWidget* parent);

    void reset();
    std::vector<PlotInfo> plots() const;
    void setPlot(std::size_t plot, const QPlotTab::Cache& attributes);
    void setSelectedPlot(std::size_t plot, QPlot::Type selected);

public Q_SLOTS:
    void open() override;
    void done(int) override;
    void applyArrangement();
    void showArrangement();

private:
    struct
    {
        std::vector<QPlotTab::Cache> plotTabs;
        QList<orbital::GridPoint> arrangement;
        QPixmap buttonGridStatus;
    } cache;

    void setPlotTabs(std::size_t);

    Ui::PlotDialog ui;
    std::vector<QPlotTab*> plotTabs;
};
