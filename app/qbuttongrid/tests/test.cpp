#include <QtWidgets/QApplication>

#include "gtest/gtest.h"
#include "qbuttongrid.hpp"
#include "qbuttongridprivate.hpp"


namespace testing {


TEST(BasicTest, Init) {
    auto buttonGrid = new QButtonGrid;
    ASSERT_EQ(1, buttonGrid->nCols());
    ASSERT_EQ(1, buttonGrid->nRows());
    ASSERT_EQ(1, buttonGrid->id(0, 0));
    ASSERT_EQ(0, buttonGrid->id(1, 0));
    ASSERT_EQ(0, buttonGrid->id(0, 1));
    delete buttonGrid;
}


TEST(BasicTest, AddCol) {
    QButtonGrid buttonGrid;
    buttonGrid.addCol();
    ASSERT_EQ(2, buttonGrid.nCols());
    ASSERT_EQ(1, buttonGrid.nRows());
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(2, buttonGrid.id(1, 0));
}


TEST(BasicTest, DelCol) {
    QButtonGrid buttonGrid;
    buttonGrid.addCol();
    buttonGrid.delCol();
    ASSERT_EQ(1, buttonGrid.nCols());
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(0, buttonGrid.id(1, 0));
}


TEST(BasicTest, AddRow) {
    QButtonGrid buttonGrid;
    buttonGrid.addRow();
    ASSERT_EQ(2, buttonGrid.nRows());
    ASSERT_EQ(1, buttonGrid.nCols());
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(2, buttonGrid.id(0, 1));
}


TEST(BasicTest, DelRow) {
    QButtonGrid buttonGrid;
    buttonGrid.addRow();
    buttonGrid.delRow();
    ASSERT_EQ(1, buttonGrid.nRows());
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(0, buttonGrid.id(0, 1));
}


TEST(BasicTest, Combine) {
    QButtonGrid buttonGrid;
    buttonGrid.addCol();
    buttonGrid.addRow();
    buttonGrid.select(0, 0);
    buttonGrid.select(1, 1);
    buttonGrid.combine();
    ASSERT_EQ(2, buttonGrid.nCols());
    ASSERT_EQ(2, buttonGrid.nRows());
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(1, buttonGrid.id(1, 0));
    ASSERT_EQ(1, buttonGrid.id(0, 1));
    ASSERT_EQ(1, buttonGrid.id(1, 1));
}


TEST(BasicTest, Split) {
    QButtonGrid buttonGrid;
    buttonGrid.addCol();
    buttonGrid.addRow();
    buttonGrid.select(0, 0);
    buttonGrid.select(1, 1);
    buttonGrid.combine();
    buttonGrid.select(0, 0);
    buttonGrid.split();
    ASSERT_EQ(2, buttonGrid.nCols());
    ASSERT_EQ(2, buttonGrid.nRows());
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(2, buttonGrid.id(1, 0));
    ASSERT_EQ(3, buttonGrid.id(0, 1));
    ASSERT_EQ(4, buttonGrid.id(1, 1));
}


TEST(ComplexTest, MultipleAddDel) {
    QButtonGrid buttonGrid;
    int nCol = 3;
    int nRow = 5;
    for (int i = 1; i < nCol; ++i) buttonGrid.addCol();
    for (int i = 1; i < nRow; ++i) buttonGrid.addRow();
    ASSERT_EQ(nCol, buttonGrid.nCols());
    ASSERT_EQ(nRow, buttonGrid.nRows());
    int id = 1;
    for (int r = 0; r < nRow; ++r) {
        for (int c = 0; c < nCol; ++c)
            ASSERT_EQ(id++, buttonGrid.id(c, r));
    }
    for (int i = 0; i < nCol; ++i) buttonGrid.delCol();
    for (int i = 0; i < nRow; ++i) buttonGrid.delRow();
    ASSERT_EQ(1, buttonGrid.nCols());
    ASSERT_EQ(1, buttonGrid.nRows());
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(0, buttonGrid.id(1, 0));
    ASSERT_EQ(0, buttonGrid.id(0, 1));
}


TEST(ComplexTest, Combine) {
    QButtonGrid buttonGrid;
    buttonGrid.addCol();
    buttonGrid.addCol();
    buttonGrid.addRow();
    buttonGrid.addRow();
    buttonGrid.select(0, 0);
    buttonGrid.select(1, 1);
    buttonGrid.combine();
    ASSERT_EQ(3, buttonGrid.nCols());
    ASSERT_EQ(3, buttonGrid.nRows());

    // combined top left button
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(1, buttonGrid.id(1, 0));
    ASSERT_EQ(1, buttonGrid.id(0, 1));
    ASSERT_EQ(1, buttonGrid.id(1, 1));

    // right side of combined button
    ASSERT_EQ(2, buttonGrid.id(2, 0));
    ASSERT_EQ(3, buttonGrid.id(2, 1));

    // under combined button
    ASSERT_EQ(4, buttonGrid.id(0, 2));
    ASSERT_EQ(5, buttonGrid.id(1, 2));
    ASSERT_EQ(6, buttonGrid.id(2, 2));
}


TEST(ComplexTest, MultipleCombine) {
    QButtonGrid buttonGrid;
    buttonGrid.addCol();
    buttonGrid.addCol();
    buttonGrid.addRow();
    buttonGrid.addRow();

    // combine top left
    buttonGrid.select(0, 0);
    buttonGrid.select(1, 1);
    buttonGrid.combine();

    // combine bottom right (splits top left)
    buttonGrid.select(1, 2);
    buttonGrid.select(2, 1);
    buttonGrid.combine();

    // combine top left (splits bottom right)
    buttonGrid.select(1, 0);
    buttonGrid.select(0, 1);
    buttonGrid.combine();

    // combine bottom right (splits top left)
    buttonGrid.select(1, 2);
    buttonGrid.select(2, 1);
    buttonGrid.combine();

    ASSERT_EQ(3, buttonGrid.nCols());
    ASSERT_EQ(3, buttonGrid.nRows());

    // top of combined button
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(2, buttonGrid.id(1, 0));
    ASSERT_EQ(3, buttonGrid.id(2, 0));

    // left of button
    ASSERT_EQ(4, buttonGrid.id(0, 1));
    ASSERT_EQ(6, buttonGrid.id(0, 2));

    // combined bottom right button
    ASSERT_EQ(5, buttonGrid.id(1, 1));
    ASSERT_EQ(5, buttonGrid.id(2, 1));
    ASSERT_EQ(5, buttonGrid.id(1, 2));
    ASSERT_EQ(5, buttonGrid.id(2, 2));
}


TEST(ArrangementTest, Simple) {
    QButtonGrid buttonGrid;
    QList<QButtonGrid::GridPoint> points{
        {
            .x = 0,
            .dx = 0,
            .y = 0,
            .dy = 0
        }
    };
    buttonGrid.setArrangement(points);
    ASSERT_EQ(1, buttonGrid.nCols());
    ASSERT_EQ(1, buttonGrid.nRows());
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(0, buttonGrid.id(1, 0));
    ASSERT_EQ(0, buttonGrid.id(0, 1));
}


TEST(ArrangementTest, Overwrite) {
    QButtonGrid buttonGrid;
    QList<QButtonGrid::GridPoint> points{
        {
            .x = 0,
            .dx = 0,
            .y = 0,
            .dy = 0
        }
    };
    buttonGrid.addCol();
    buttonGrid.setArrangement(points);
    ASSERT_EQ(1, buttonGrid.nCols());
    ASSERT_EQ(1, buttonGrid.nRows());
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(0, buttonGrid.id(1, 0));
    ASSERT_EQ(0, buttonGrid.id(0, 1));
}


TEST(ArrangementTest, Complex) {
    QButtonGrid buttonGrid;
    QList<QButtonGrid::GridPoint> points{
        {
            .x = 0,
            .dx = 1,
            .y = 0,
            .dy = 1
        },
        {
            .x = 2,
            .dx = 0,
            .y = 0,
            .dy = 1
        },
        {
            .x = 0,
            .dx = 2,
            .y = 2,
            .dy = 0
        }
    };
    buttonGrid.setArrangement(points);
    ASSERT_EQ(3, buttonGrid.nCols());
    ASSERT_EQ(3, buttonGrid.nRows());
    ASSERT_EQ(1, buttonGrid.id(0, 0));
    ASSERT_EQ(1, buttonGrid.id(1, 0));
    ASSERT_EQ(1, buttonGrid.id(0, 1));
    ASSERT_EQ(1, buttonGrid.id(1, 1));
    ASSERT_EQ(2, buttonGrid.id(2, 0));
    ASSERT_EQ(2, buttonGrid.id(2, 1));
    ASSERT_EQ(3, buttonGrid.id(0, 2));
    ASSERT_EQ(3, buttonGrid.id(1, 2));
    ASSERT_EQ(3, buttonGrid.id(2, 2));
}


}


int main(int argc, char** argv) {
    QApplication a{argc, argv};
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
