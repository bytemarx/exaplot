#include "About.hpp"

#include "config.h"

#define APP_VERSION "v" ORBITAL_PROJECT_VERSION


About::About(QWidget* parent)
    : QDialog(parent)
{
    this->ui.setupUi(this);
    this->ui.label_version->setText(QString{APP_VERSION});
}
