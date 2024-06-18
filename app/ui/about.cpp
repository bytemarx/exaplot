/*
 * ExaPlot
 * about dialog
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include <QFile>
#include <QMessageBox>

#include "about.hpp"

#include "config.h"

#define APP_VERSION "v" EXAPLOT_PROJECT_VERSION


About::About(QWidget* parent)
    : QDialog{parent}
{
    this->ui.setupUi(this);
    this->ui.label_version->setText(QString{APP_VERSION});
}
