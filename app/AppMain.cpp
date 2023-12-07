#include "AppMain.hpp"


#define QCONNECT_INTERFACE(fn) QObject::connect(&this->interface, &Interface::signal_##fn, this, &AppMain::slot_##fn)


AppMain::AppMain(int& argc, char* argv[])
    : QObject{Q_NULLPTR}
    , a{argc, argv}
    , ui{this}
    , interface{this}
{
    QCONNECT_INTERFACE(init);
    QCONNECT_INTERFACE(msg);
    QCONNECT_INTERFACE(plot);
    QCONNECT_INTERFACE(plotVec);
    QCONNECT_INTERFACE(clear);
}


AppMain::~AppMain()
{
}


int
AppMain::exec()
{
    ui.show();
    return a.exec();
}
