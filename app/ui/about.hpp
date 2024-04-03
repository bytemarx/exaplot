/*
 * ZetaPlot
 * about dialog
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include "ui_about.h"


class About : public QDialog
{
    Q_OBJECT

public:
    About(QWidget* parent);

private:
    Ui::AboutDialog ui;
};
