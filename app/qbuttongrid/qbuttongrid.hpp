/*
 * ExaPlot
 * QButtonGrid widget
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include <QWidget>
#include <QGridLayout>

#include "exaplot.hpp"

#include <vector>


class QButtonGridNode;
class QButtonGridRootNode;
template<typename T> class Matrix;

class QButtonGrid : public QWidget
{
    Q_OBJECT

public:
    using GridPoint = exa::GridPoint;
    explicit QButtonGrid(QWidget* parent = Q_NULLPTR);
    QButtonGrid(const QButtonGrid&) = delete;
    ~QButtonGrid();

    std::vector<GridPoint> arrangement() const;
    bool setArrangement(const std::vector<GridPoint>&);
    std::size_t nRows() const;
    std::size_t nCols() const;
    std::size_t nButtons() const;
    int id(std::size_t col, std::size_t row) const;
    void select(std::size_t col, std::size_t row, bool val = true);
    void addRow();
    void delRow();
    void addCol();
    void delCol();
    void combine();
    void split();

Q_SIGNALS:
    void gridChanged();

private Q_SLOTS:
    void clickLeft() { this->delCol(); }
    void clickRight() { this->addCol(); }
    void clickUp() { this->delRow(); }
    void clickDown() { this->addRow(); }
    void clickCombine() { this->combine(); }
    void clickSplit() { this->split(); }

private:
    void clear();
    bool split(std::size_t col, std::size_t row);
    int selectionVertices(std::size_t& cmin, std::size_t& cmax, std::size_t& rmin, std::size_t& rmax);
    QButtonGridRootNode* root(std::size_t col, std::size_t row);
    const QButtonGridRootNode* root(std::size_t col, std::size_t row) const;

    Matrix<QButtonGridNode*>* m;
    QGridLayout* gridLayout;
};
