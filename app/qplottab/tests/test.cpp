#include <QtWidgets/QApplication>

#include "gtest/gtest.h"
#include "qplottab.hpp"
#include "qplottabprivate.hpp"


namespace testing {


TEST(BasicTest, Init) {
    auto plotTab = new QPlotTab;
    delete plotTab;
}


}


int main(int argc, char** argv) {
    QApplication a{argc, argv};
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
