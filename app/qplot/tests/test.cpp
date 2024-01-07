#include <QtWidgets/QApplication>

#include "gtest/gtest.h"
#include "qplot.hpp"


namespace testing {


TEST(BasicTest, Init) {
    auto plot = new QPlot;
    delete plot;
}


TEST(BasicTest, Set2D) {
    auto plot = new QPlot;
    plot->setType(QPlot::Type::TWODIMEN);
    delete plot;
}


TEST(BasicTest, SetColorMap) {
    auto plot = new QPlot;
    plot->setType(QPlot::Type::COLORMAP);
    delete plot;
}


TEST(BasicTest, CycleTypes) {
    auto plot = new QPlot;
    plot->setType(QPlot::Type::COLORMAP);
    plot->setType(QPlot::Type::TWODIMEN);
    delete plot;
}


}


int main(int argc, char** argv) {
    QApplication a{argc, argv};
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
