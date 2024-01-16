#include "appmain.hpp"

#include "orbital.hpp"

#include <filesystem>
#include <string>


int main(int argc, char* argv[])
{
    QThread thread{};
    Interface iface{};
    QObject::connect(&thread, &QThread::started, &iface, &Interface::pythonInit);
    QObject::connect(&thread, &QThread::finished, &iface, &Interface::pythonDeInit);
    iface.moveToThread(&thread);
    thread.start();
    AppMain a{argc, argv, &iface, &thread};
    return a.exec();
}
