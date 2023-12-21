#pragma once

#include "ui_about.h"


class About : public QDialog
{
    Q_OBJECT

public:
    About(QWidget* parent);

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::AboutDialog ui;
};
