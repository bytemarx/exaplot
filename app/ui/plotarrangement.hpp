/*
 * ExaPlot
 * plot arrangement dialog
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include "ui_plotarrangement.h"

#include "qbuttongrid.hpp"


class PlotArrangement : public QDialog
{
    Q_OBJECT

public:
    PlotArrangement(QWidget* parent, const std::vector<exa::GridPoint>& arrangement);

private:
    Ui::PlotArrangementDialog ui;
};
