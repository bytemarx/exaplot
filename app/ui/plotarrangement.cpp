/*
 * ExaPlot
 * plot arrangement dialog
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "plotarrangement.hpp"


PlotArrangement::PlotArrangement(QWidget* parent, const std::vector<exa::GridPoint>& arrangement)
    : QDialog{parent, Qt::Window}
{
    this->ui.setupUi(this);

    QString arrangementStr('[');
    for (const auto& button : arrangement) {
        arrangementStr.append("\n\t(");
        arrangementStr.append(QString::number(button.x));
        arrangementStr.append(", ");
        arrangementStr.append(QString::number(button.dx));
        arrangementStr.append(", ");
        arrangementStr.append(QString::number(button.y));
        arrangementStr.append(", ");
        arrangementStr.append(QString::number(button.dy));
        arrangementStr.append("),");
    }
    arrangementStr.append("\n]");
    this->ui.plainTextEdit->setPlainText(arrangementStr);
}
