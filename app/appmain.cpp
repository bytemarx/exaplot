#include "appmain.hpp"
#include <iostream>

#define QCONNECT_INTERFACE(fn) QObject::connect(&this->interface, &Interface::signal_##fn, this, &AppMain::slot_##fn)


AppMain::AppMain(int& argc, char* argv[])
    : QObject{Q_NULLPTR}
    , a{argc, argv}
    , ui{this}
    , interface{this}
    , core{&interface}
{
    QObject::connect(&this->ui, &AppUI::scriptLoaded, this, &AppMain::load);
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
AppMain::load(const QString& file)
{
    std::cout << "Loading: " << file.toStdString() << '\n';
    auto status = this->core.load(std::filesystem::path{file.toStdString()}, this->module);
    if (status) {
        auto message = status.message();
        if (!status.traceback().empty())
            message.append("\n").append(status.traceback());
        this->ui.displayError(message.c_str(), QString{"ERROR::"}.append(status.type()));
    }
}


void
AppMain::run()
{
    if (!this->module)
        return;

    auto status = this->module.get()->run(this->ui.scriptArgs());
    if (status) {
        auto message = status.message();
        if (!status.traceback().empty())
            message.append("\n").append(status.traceback());
        this->ui.displayError(message.c_str(), QString{"ERROR::"}.append(status.type()));
    }
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
    this->ui.setMessage(QString::fromStdString(message));
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
