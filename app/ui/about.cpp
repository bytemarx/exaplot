#include <QFile>
#include <QMessageBox>

#include "about.hpp"

#include "config.h"

#define APP_VERSION "v" ZPLOT_PROJECT_VERSION


static QString
attributions()
{
    QFile file{":/res/attributions.txt"};
    if (!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "error", file.errorString());
    QTextStream in{&file};
    QString content = in.readAll();
    file.close();
    return content;
}


About::About(QWidget* parent)
    : QDialog{parent}
{
    this->ui.setupUi(this);
    this->ui.label_version->setText(QString{APP_VERSION});
    this->ui.plainTextEdit_attributions->setPlainText(attributions());
}
