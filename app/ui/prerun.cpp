/*
 * ExaPlot
 * pre-run dialog
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "prerun.hpp"


PreRun::PreRun(QWidget* parent)
    : QDialog{parent}
{
    this->ui.setupUi(this);
}
