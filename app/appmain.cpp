#include "appmain.hpp"


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


void
AppMain::slot_init(
        const std::vector<std::string>& params,
        const std::vector<orbital::GridPoint>& plots)
{
}


void
AppMain::slot_msg(const std::string& message, bool append)
{
}


void
AppMain::slot_plot(long dataSet, const std::vector<double>& data)
{
}


void
AppMain::slot_plotVec(long dataSet, const std::vector<std::vector<double>>& data)
{
}


void
AppMain::slot_clear(long dataSet)
{
}
