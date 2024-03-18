#include "appmain.hpp"

#include "zetaplot.hpp"

#include <filesystem>
#include <string>


int main(int argc, char* argv[])
{
    AppMain a{argc, argv};
    return a.exec();
}
