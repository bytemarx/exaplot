#include <QMessageBox>
#include <QPixmap>
#include <QPainter>
#include <QSvgRenderer>

#include "ploteditor.hpp"


static QPixmap
buttonGridStatusPixmap(bool status)
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    if (status)
        QSvgRenderer(QStringLiteral(":/res/checkmark.svg")).render(&painter);
    else
        QSvgRenderer(QStringLiteral(":/res/exclamation.svg")).render(&painter);
    return pixmap;
}


PlotEditor::PlotEditor(QWidget* parent)
    : QDialog{parent, Qt::Window | Qt::WindowStaysOnTopHint}
{
    this->ui.setupUi(this);
    this->applyArrangement();

    QObject::connect(
        this->ui.buttonGrid, &QButtonGrid::gridChanged,
        [=] { this->ui.label_buttonGridStatus->setPixmap(buttonGridStatusPixmap(false)); }
    );
    QObject::connect(
        this->ui.pushButton_buttonGridApply, &QPushButton::clicked,
        this, &PlotEditor::applyArrangement
    );
    QObject::connect(
        this->ui.pushButton_buttonGridArrangement, &QPushButton::clicked,
        this, &PlotEditor::showArrangement
    );
}


void
PlotEditor::open()
{
    this->cache.plotTabs = std::vector<QPlotTab::Cache>{};
    for (const auto& tab : this->plotTabs)
        this->cache.plotTabs.push_back(tab->cache());
    this->cache.arrangement = this->ui.buttonGrid->arrangement();
    this->cache.buttonGridStatus = this->ui.label_buttonGridStatus->pixmap();
    return QDialog::open();
}


std::vector<PlotEditor::PlotInfo>
PlotEditor::plots() const
{
    std::vector<PlotInfo> plotInfo;
    auto arrangement = this->ui.buttonGrid->arrangement();
    // TODO: tighter coupling between button grid arrangement and plot tabs
    assert(arrangement.size() == this->plotTabs.size());
    for (std::size_t i = 0; i < this->plotTabs.size(); ++i) {
        plotInfo.push_back({
            .position = arrangement[i],
            .attributes = this->plotTabs[i]->cache()
        });
    }
    return plotInfo;
}


void
PlotEditor::done(int r)
{
    if (r != QDialog::Accepted) {
        this->setPlotTabs(this->cache.plotTabs.size());
        for (std::size_t i = 0; i < this->cache.plotTabs.size(); ++i)
            this->plotTabs.at(i)->setCache(this->cache.plotTabs.at(i));
        this->ui.buttonGrid->setArrangement(this->cache.arrangement);
        this->ui.label_buttonGridStatus->setPixmap(this->cache.buttonGridStatus);
    } else {
        this->applyArrangement();
    }
    return QDialog::done(r);
}


void
PlotEditor::applyArrangement()
{
    this->ui.label_buttonGridStatus->setPixmap(buttonGridStatusPixmap(true));
    this->setPlotTabs(this->ui.buttonGrid->nButtons());
}


void
PlotEditor::showArrangement()
{
	QString arrangementStr('[');
    for (const auto& button : this->ui.buttonGrid->arrangement()) {
		arrangementStr.append('[');
		arrangementStr.append(QString::number(button.x));
		arrangementStr.append(',');
		arrangementStr.append(QString::number(button.dx));
		arrangementStr.append(',');
		arrangementStr.append(QString::number(button.y));
		arrangementStr.append(',');
		arrangementStr.append(QString::number(button.dy));
		arrangementStr.append("],");
	}
	arrangementStr.back() = QChar(']');

    QMessageBox messageBox;
    messageBox.setText(arrangementStr);
    messageBox.exec();
}


void
PlotEditor::setPlotTabs(std::size_t n)
{
    if (n > this->plotTabs.size()) {
        for (decltype(n) i = this->plotTabs.size(); i < n; ++i) {
            auto plotTab = new QPlotTab{this};
            this->ui.tabWidget_plotTabs->addTab(plotTab, QString::number(i + 1));
            this->plotTabs.push_back(plotTab);
        }
    } else if (n < this->plotTabs.size()) {
        for (decltype(n) i = this->plotTabs.size(); i > n; --i) {
            this->ui.tabWidget_plotTabs->removeTab(i - 1);
            delete this->plotTabs[i - 1];
        }
        this->plotTabs.resize(n);
    }
}
