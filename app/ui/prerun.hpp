/*
 * ExaPlot
 * pre-run dialog
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include "ui_prerun.h"


class PreRun : public QDialog
{
    Q_OBJECT

public:
    PreRun(QWidget* parent);

private:
    Ui::PreRunDialog ui;
};
