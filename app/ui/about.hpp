#pragma once

#include "ui_about.h"


class About : public QDialog
{
    Q_OBJECT

public:
    About(QWidget* parent);

private:
    Ui::AboutDialog ui;
};
