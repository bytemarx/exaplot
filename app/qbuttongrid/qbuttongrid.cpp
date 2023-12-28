#include <QScrollArea>

#include "qbuttongrid.hpp"
#include "qbuttongridprivate.hpp"

#include <limits>

using orbital::GridPoint;
using orbital::GridPoint_t;


static constexpr auto QBUTTONGRID_MAX = std::numeric_limits<int>::max();


static void
updateIds(const Matrix<QButtonGridNode*>* m)
{
    int id = 1;
    for (decltype(m->nRows()) r = 0; r < m->nRows(); ++r) {
        for (decltype(m->nCols()) c = 0; c < m->nCols(); ++c) {
            if (m->itemAt(c, r)->isRoot())
                dynamic_cast<QButtonGridRootNode*>(m->itemAt(c, r))->setId(id++);
        }
    }
}


/**
 * @brief Checks if the given arrangement is a valid arrangement. If valid, returns `true` and
 * populates the given `cols` and `rows` variables with the total columns and rows, respectively.
 * 
 * @param arrangement 
 * @param cols 
 * @param rows 
 * @return true 
 * @return false 
 */
static bool
isArrangementValid(
    const QList<GridPoint>& arrangement,
    GridPoint_t& cols,
    GridPoint_t& rows)
{
    if (arrangement.size() == 0) return false;
    cols = 0;
    rows = 0;

    for (const auto& button : arrangement) {
        if (button.x + button.dx > cols) cols = button.x + button.dx;
        if (button.y + button.dy > rows) rows = button.y + button.dy;
    }
    // max(a+1, b+1) = max(a, b)+1
    cols += 1;
    rows += 1;
    if (cols > QBUTTONGRID_MAX || rows > QBUTTONGRID_MAX)
        return false;

    Matrix<int> m{cols, rows, 0};
    for (const auto& button : arrangement) {
        for (GridPoint_t cs = 0; cs <= button.dx; ++cs) {
            for (GridPoint_t rs = 0; rs <= button.dy; ++rs) {
                if (m.itemAt(button.x + cs, button.y + rs))
                    return false;
                m.setCell(button.x + cs, button.y + rs, 1);
            }
        }
    }
    for (const auto& col : m) {
        for (const auto& occupied : col)
            if (occupied == 0)
                return false;
    }

    return true;
}


QButtonGrid::QButtonGrid(QWidget* parent)
    : QWidget{parent}
    , m{new Matrix<QButtonGridNode*>{1, 1, new QButtonGridRootNode}}
{
    QGridLayout* layout = new QGridLayout{this};
    QScrollArea* gridArea = new QScrollArea{this};
    QPushButton* buttonLeft = new QPushButton{"←", this};
    QPushButton* buttonRight = new QPushButton{"→", this};
    QPushButton* buttonUp = new QPushButton{"↑", this};
    QPushButton* buttonDown = new QPushButton{"↓", this};
    QPushButton* buttonCombine = new QPushButton{"combine", this};
    QPushButton* buttonSplit = new QPushButton{"split", this};

    QWidget* gridContents = new QWidget{gridArea};
    this->gridLayout = new QGridLayout{gridContents};
    this->gridLayout->addWidget(dynamic_cast<QPushButton*>(this->m->itemAt(0, 0)), 0, 0, 1, 1);
    gridArea->setWidgetResizable(true);
    gridArea->setWidget(gridContents);

    QSizePolicy sizePolicy{QSizePolicy::Ignored, QSizePolicy::Ignored};
    buttonLeft->setSizePolicy(sizePolicy);
    buttonRight->setSizePolicy(sizePolicy);
    buttonUp->setSizePolicy(sizePolicy);
    buttonDown->setSizePolicy(sizePolicy);

    layout->addWidget(gridArea, 0, 0, 1, 1);
    layout->addWidget(buttonLeft, 0, 1, 1, 1);
    layout->addWidget(buttonRight, 0, 2, 1, 1);
    layout->addWidget(buttonUp, 1, 0, 1, 1);
    layout->addWidget(buttonDown, 2, 0, 1, 1);
    layout->addWidget(buttonCombine, 1, 1, 1, 2);
    layout->addWidget(buttonSplit, 2, 1, 1, 2);

    QObject::connect(buttonLeft, &QPushButton::clicked, this, &QButtonGrid::clickLeft);
    QObject::connect(buttonRight, &QPushButton::clicked, this, &QButtonGrid::clickRight);
    QObject::connect(buttonUp, &QPushButton::clicked, this, &QButtonGrid::clickUp);
    QObject::connect(buttonDown, &QPushButton::clicked, this, &QButtonGrid::clickDown);
    QObject::connect(buttonCombine, &QPushButton::clicked, this, &QButtonGrid::clickCombine);
    QObject::connect(buttonSplit, &QPushButton::clicked, this, &QButtonGrid::clickSplit);
}


QButtonGrid::~QButtonGrid()
{
    this->clear();
}


/**
 * @brief Returns the current button arrangement.
 * 
 * @return QList<GridPoint> 
 */
QList<GridPoint>
QButtonGrid::arrangement() const
{
    QList<GridPoint> arrangement;
    for (decltype(this->m->nRows()) r = 0; r < this->m->nRows(); ++r) {
        for (decltype(this->m->nCols()) c = 0; c < this->m->nCols(); ++c) {
            if (this->m->itemAt(c, r)->isRoot()) {
                auto root = dynamic_cast<QButtonGridRootNode*>(this->m->itemAt(c, r));
                arrangement.push_back(GridPoint{
                    .x = static_cast<GridPoint_t>(c),
                    .dx = static_cast<GridPoint_t>(root->cs),
                    .y = static_cast<GridPoint_t>(r),
                    .dy = static_cast<GridPoint_t>(root->rs)
                });
            }
        }
    }
    return arrangement;
}


/**
 * @brief Sets the button arrangement.
 * 
 * @param arrangement 
 */
void
QButtonGrid::setArrangement(const QList<GridPoint>& arrangement)
{
    GridPoint_t cols;
    GridPoint_t rows;

    if (!isArrangementValid(arrangement, cols, rows))
        return;

    this->clear();
    this->m = new Matrix<QButtonGridNode*>{cols, rows};
    for (GridPoint_t c = 0; c < cols; ++c) {
        for (GridPoint_t r = 0; r < rows; ++r) {
            auto root = new QButtonGridRootNode;
            this->m->setCell(c, r, root);
            this->gridLayout->addWidget(dynamic_cast<QPushButton*>(root), r, c);
        }
    }

    for (auto button : arrangement) {
        if (button.dx || button.dy) {
            this->select(button.x, button.y);
            this->select(button.x + button.dx, button.y + button.dy);
            this->combine();
        }
    }
}


/**
 * @brief Returns the number of rows.
 * 
 * @return std::size_t 
 */
std::size_t
QButtonGrid::nRows() const
{
    return this->m->nRows();
}


/**
 * @brief Returns the number of columns.
 * 
 * @return std::size_t 
 */
std::size_t
QButtonGrid::nCols() const
{
    return this->m->nCols();
}


/**
 * @brief Returns the ID of the designated button.
 * 
 * @param c 
 * @param r 
 * @return int 
 */
int
QButtonGrid::id(std::size_t c, std::size_t r) const
{
    if (c >= this->m->nCols() || r >= this->m->nRows())
        return 0;

    return this->root(c, r)->id;
}


/**
 * @brief Select or deselect the designated button. The given value determines whether the button
 * will be selected (`true`), which is the default value, or deselected (`false`).
 * 
 * @param col 
 * @param row 
 * @param val 
 */
void
QButtonGrid::select(std::size_t col, std::size_t row, bool val)
{
    this->root(col, row)->setChecked(val);
}


/**
 * @brief Adds a row of buttons to the bottom.
 * 
 */
void
QButtonGrid::addRow()
{
    assert(this->m->nCols() > 0 && this->m->nRows() > 0);
    auto r = this->m->nRows();
    if (r == QBUTTONGRID_MAX) return;

    this->m->resize(this->m->nCols(), r + 1);
    for (std::size_t c = 0; c < this->m->nCols(); ++c) {
        auto root = new QButtonGridRootNode;
        this->m->setCell(c, r, root);
        this->gridLayout->addWidget(dynamic_cast<QPushButton*>(root), r, c);
    }
    updateIds(this->m);
    emit this->gridChanged();
}


/**
 * @brief Removes the bottom-most row of buttons.
 * 
 */
void
QButtonGrid::delRow()
{
    assert(this->m->nCols() > 0 && this->m->nRows() > 0);
    if (this->m->nRows() == 1) return;

    for (auto c = this->m->nCols(); c > 0; --c) {
        auto node = this->m->itemAt(c - 1, this->m->nRows() - 1);
        if (node->isRoot()) {
            this->gridLayout->removeWidget(dynamic_cast<QPushButton*>(node));
        } else if (dynamic_cast<QButtonGridExtNode*>(node)->dc == 0) {
            auto root = this->root(c - 1, this->m->nRows() - 1);
            this->gridLayout->removeWidget(dynamic_cast<QPushButton*>(root));
            auto row = this->nRows() - dynamic_cast<QButtonGridExtNode*>(node)->dr - 1;
            auto col = c - 1;
            auto rowSpan = --root->rs;
            auto colSpan = root->cs;
            this->gridLayout->addWidget(
                dynamic_cast<QPushButton*>(root), row, col, rowSpan + 1, colSpan + 1
            );
        }
        delete node;
    }
    this->m->resize(this->m->nCols(), this->m->nRows() - 1);
    updateIds(this->m);
    emit this->gridChanged();
}


/**
 * @brief Adds a column of buttons to the right.
 * 
 */
void
QButtonGrid::addCol()
{
    assert(this->m->nCols() > 0 && this->m->nRows() > 0);
    auto c = this->m->nCols();
    if (c == QBUTTONGRID_MAX) return;

    this->m->resize(c + 1, this->m->nRows());
    for (std::size_t r = 0; r < this->m->nRows(); ++r) {
        auto root = new QButtonGridRootNode;
        this->m->setCell(c, r, root);
        this->gridLayout->addWidget(dynamic_cast<QPushButton*>(root), r, c);
    }
    updateIds(this->m);
    emit this->gridChanged();
}


/**
 * @brief Removes the right-most column of buttons.
 * 
 */
void
QButtonGrid::delCol()
{
    assert(this->m->nCols() > 0 && this->m->nRows() > 0);
    if (this->m->nCols() == 1) return;

    for (auto r = this->m->nRows(); r > 0; --r) {
        auto node = this->m->itemAt(this->m->nCols() - 1, r - 1);
        if (node->isRoot()) {
            this->gridLayout->removeWidget(dynamic_cast<QPushButton*>(node));
        } else if (dynamic_cast<QButtonGridExtNode*>(node)->dr == 0) {
            auto root = this->root(this->m->nCols() - 1, r - 1);
            this->gridLayout->removeWidget(dynamic_cast<QPushButton*>(root));
            this->gridLayout->addWidget(
                dynamic_cast<QPushButton*>(root),
                r - 1,
                this->nCols() - dynamic_cast<QButtonGridExtNode*>(node)->dc - 1,
                root->rs + 1,
                --root->cs + 1
            );
        }
        delete node;
    }
    this->m->resize(this->m->nCols() - 1, this->m->nRows());
    updateIds(this->m);
    emit this->gridChanged();
}


/**
 * @brief Combines the currently selected buttons.
 * 
 */
void
QButtonGrid::combine()
{
    std::size_t colMin;
    std::size_t colMax;
    std::size_t rowMin;
    std::size_t rowMax;

    if (!selectionVertices(colMin, colMax, rowMin, rowMax))
        return;

    // split up buttons crossing the border (i'm building a wall and it's going to be beautiful, believe me)
    for (auto c = colMin; c <= colMax; ++c) {
        for (auto r = rowMin; r <= rowMax; ++r) {
            this->split(c, r);
            assert(this->m->itemAt(c, r)->isRoot());
            dynamic_cast<QButtonGridRootNode*>(this->m->itemAt(c, r))->setChecked(true);
        }
    }

    auto col = colMin;
    auto row = rowMin;
    auto colSpan = colMax - colMin;
    auto rowSpan = rowMax - rowMin;
    auto root = new QButtonGridRootNode{colSpan, rowSpan};
    for (auto c = colMin; c <= colMax; ++c) {
        for (auto r = rowMin; r <= rowMax; ++r) {
            auto consumed = this->m->itemAt(c, r);
            if (consumed->isRoot())
                this->gridLayout->removeWidget(dynamic_cast<QPushButton*>(consumed));
            delete consumed;
            if (c != colMin || r != rowMin)
                this->m->setCell(c, r, new QButtonGridExtNode{c - colMin, r - rowMin});
        }
    }
    this->m->setCell(col, row, root);
    this->gridLayout->addWidget(
        dynamic_cast<QPushButton*>(root),
        row,
        col,
        rowSpan + 1,
        colSpan + 1
    );
    updateIds(this->m);
    emit this->gridChanged();
}


/**
 * @brief Splits all of the currently selected buttons.
 * 
 */
void
QButtonGrid::split()
{
    bool gridChanged = false;
    for (std::size_t c = 0; c < this->m->nCols(); ++c) {
        for (std::size_t r = 0; r < this->m->nRows(); ++r) {
            if (this->root(c, r)->isChecked()) {
                this->root(c, r)->setChecked(false);
                gridChanged = this->split(c, r);
            }
        }
    }
    if (gridChanged) {
        updateIds(this->m);
        emit this->gridChanged();
    }
}


/**
 * @brief Clears the button grid and deletes the internal button matrix.
 * 
 */
void
QButtonGrid::clear()
{
    for (auto c = this->m->nCols(); c > 0; --c) {
        for (auto r = this->m->nRows(); r > 0; --r) {
            auto node = this->m->itemAt(c - 1, r - 1);
            if (node->isRoot())
                this->gridLayout->removeWidget(dynamic_cast<QPushButton*>(node));
            delete this->m->itemAt(c - 1, r - 1);
        }
    }
    delete this->m;
}


/**
 * @brief Separates the designated button fully into root nodes. Returns `true` if the designated
 * button was actually split, `false` otherwise.
 * 
 * @param col 
 * @param row 
 * @return true 
 * @return false 
 */
bool
QButtonGrid::split(std::size_t col, std::size_t row)
{
    auto node = this->m->itemAt(col, row);
    if (node->isRoot()) {
        if (dynamic_cast<QButtonGridRootNode*>(node)->cs == 0 &&
            dynamic_cast<QButtonGridRootNode*>(node)->rs == 0)
            return false;
    } else {
        auto ext = dynamic_cast<QButtonGridExtNode*>(node);
        assert(col >= ext->dc);
        assert(row >= ext->dr);
        col -= ext->dc;
        row -= ext->dr;
    }
    auto colEnd = col + this->root(col, row)->cs;
    auto rowEnd = row + this->root(col, row)->rs;
    this->gridLayout->removeWidget(dynamic_cast<QPushButton*>(this->root(col, row)));
    for (std::size_t c = col; c <= colEnd; ++c) {
        for (std::size_t r = row; r <= rowEnd; ++r) {
            delete this->m->itemAt(c, r);
            auto newRoot = new QButtonGridRootNode;
            this->m->setCell(c, r, newRoot);
            this->gridLayout->addWidget(dynamic_cast<QPushButton*>(newRoot), r, c);
        }
    }
    return true;
}


/**
 * @brief Calculates the vertices of the rectangle for the current selection. Returns `true` if
 * more than one button is selected, `false` otherwise.
 * 
 * @param colMin 
 * @param colMax 
 * @param rowMin 
 * @param rowMax 
 * @return true 
 * @return false 
 */
bool
QButtonGrid::selectionVertices(
    std::size_t& colMin,
    std::size_t& colMax,
    std::size_t& rowMin,
    std::size_t& rowMax)
{
    bool oneSelected = false;
    bool twoSelected = false;
    std::size_t cmin = this->m->nCols();
    std::size_t cmax = 0;
    std::size_t rmin = this->m->nRows();
    std::size_t rmax = 0;
    for (std::size_t c = 0; c < this->m->nCols(); ++c) {
        for (std::size_t r = 0; r < this->m->nRows(); ++r) {
            if (this->root(c, r)->isChecked()) {
                if (this->m->itemAt(c, r)->isRoot()) {
                    twoSelected = oneSelected;
                    oneSelected = true;
                }
                if (c < cmin) cmin = c;
                if (c > cmax) cmax = c;
                if (r < rmin) rmin = r;
                if (r > rmax) rmax = r;
            }
        }
    }
    colMin = cmin;
    colMax = cmax;
    rowMin = rmin;
    rowMax = rmax;
    return twoSelected;
}


/**
 * @brief Retrieves the root node at the designated position.
 * 
 * @param col 
 * @param row 
 * @return QButtonGridRootNode* 
 */
QButtonGridRootNode*
QButtonGrid::root(std::size_t col, std::size_t row)
{
    auto node = this->m->itemAt(col, row);
    if (node->isRoot())
        return dynamic_cast<QButtonGridRootNode*>(node);

    auto ext = dynamic_cast<QButtonGridExtNode*>(node);
    auto rootCol = col - ext->dc;
    auto rootRow = row - ext->dr;
    assert(this->m->itemAt(rootCol, rootRow)->isRoot());
    return dynamic_cast<QButtonGridRootNode*>(this->m->itemAt(rootCol, rootRow));
}


/**
 * @brief Like the other method but more const.
 * 
 * @param col 
 * @param row 
 * @return const QButtonGridRootNode* 
 */
const QButtonGridRootNode*
QButtonGrid::root(std::size_t col, std::size_t row) const
{
    auto node = this->m->itemAt(col, row);
    if (node->isRoot())
        return dynamic_cast<const QButtonGridRootNode*>(node);

    auto ext = dynamic_cast<QButtonGridExtNode*>(node);
    auto rootCol = col - ext->dc;
    auto rootRow = row - ext->dr;
    assert(this->m->itemAt(rootCol, rootRow)->isRoot());
    return dynamic_cast<const QButtonGridRootNode*>(this->m->itemAt(rootCol, rootRow));
}
