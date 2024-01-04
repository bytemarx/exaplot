#pragma once

#include "ui_ploteditor.h"
#include "qplottab.hpp"

#include <vector>


class PlotEditor : public QDialog
{
    Q_OBJECT

public:
    PlotEditor(QWidget* parent);

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