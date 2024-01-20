#include "mainwindow.hpp"


MainWindow::MainWindow()
    : QMainWindow{nullptr}
    , m_timer{this}
    , m_programmaticClose{false}
{
    this->m_ui.setupUi(this);
    this->m_ui.tableWidget_args->setHorizontalHeaderLabels({"Parameter", "Value"});

    QObject::connect(&this->m_timer, &QTimer::timeout, this, &MainWindow::redraw);
    QObject::connect(this->m_ui.actionQuit, &QAction::triggered, [this] { emit this->closed(); });

    this->m_timer.start(16);
}


bool
MainWindow::close()
{
    this->m_programmaticClose = true;
    return QMainWindow::close();
}


QAction*
MainWindow::actionLoad()
{
    return this->m_ui.actionLoadScript;
}


QPushButton*
MainWindow::buttonRun()
{
    return this->m_ui.pushButton_run;
}


QAction*
MainWindow::actionAbout()
{
    return this->m_ui.actionAbout;
}


QAction*
MainWindow::actionPlotEditor()
{
    return this->m_ui.actionPlotEditor;
}


void
MainWindow::setPlots(const std::vector<PlotEditor::PlotInfo>& plots)
{
    for (auto plot : this->m_plots)
        this->m_ui.gridLayout_plots->removeWidget(plot);

    if (this->m_plots.size() < plots.size()) {
        for (auto i = this->m_plots.size(); i < plots.size(); ++i)
            this->m_plots.push_back(new QPlot{this->m_ui.widget_plotPanel});
    } else if (this->m_plots.size() > plots.size()) {
        for (auto i = this->m_plots.size(); i > plots.size(); --i)
            delete this->m_plots[i - 1];
        this->m_plots.resize(plots.size());
    }

    for (std::size_t i = 0; i < plots.size(); ++i) {
        auto plot = this->m_plots[i];
        this->m_ui.gridLayout_plots->addWidget(
            plot,
            static_cast<int>(plots[i].position.y),
            static_cast<int>(plots[i].position.x),
            static_cast<int>(plots[i].position.dy) + 1,
            static_cast<int>(plots[i].position.dx) + 1
        );
        plot->setType(plots[i].selected);
        plot->setTitle(plots[i].attributes.title);
        plot->setLabelX(plots[i].attributes.xAxis);
        plot->setLabelY(plots[i].attributes.yAxis);
        plot->setMinimumSize({
            plots[i].attributes.minSize.width,
            plots[i].attributes.minSize.height
        });
        plot->plot2D()->setRangeX({
            plots[i].attributes.twoDimen.xRange.min.toDouble(),
            plots[i].attributes.twoDimen.xRange.max.toDouble()
        });
        plot->plot2D()->setRangeY({
            plots[i].attributes.twoDimen.yRange.min.toDouble(),
            plots[i].attributes.twoDimen.yRange.max.toDouble()
        });
        plot->plot2D()->setLineStyle(plots[i].attributes.twoDimen.line.type);
        QPen pen{plots[i].attributes.twoDimen.line.color};
        pen.setStyle(plots[i].attributes.twoDimen.line.style);
        plot->plot2D()->setPen(pen);
        plot->plot2D()->setScatStyle({
            plots[i].attributes.twoDimen.points.shape,
            plots[i].attributes.twoDimen.points.color,
            plots[i].attributes.twoDimen.points.size
        });
        plot->plotColorMap()->setRangeX({
            plots[i].attributes.colorMap.xRange.min.toDouble(),
            plots[i].attributes.colorMap.xRange.max.toDouble()
        });
        plot->plotColorMap()->setRangeY({
            plots[i].attributes.colorMap.yRange.min.toDouble(),
            plots[i].attributes.colorMap.yRange.max.toDouble()
        });
        plot->plotColorMap()->setRangeZ({
            plots[i].attributes.colorMap.zRange.min.toDouble(),
            plots[i].attributes.colorMap.zRange.max.toDouble()
        });
        plot->plotColorMap()->setDataSize(
            plots[i].attributes.colorMap.dataSize.x,
            plots[i].attributes.colorMap.dataSize.y
        );
        QCPColorGradient color;
        color.setColorStopAt(0, plots[i].attributes.colorMap.color.min);
        color.setColorStopAt(1, plots[i].attributes.colorMap.color.max);
        plot->plotColorMap()->setColorGradient(color);
    }
}


void
MainWindow::setMessage(const QString& message)
{
    this->m_ui.plainTextEdit_messages->setPlainText(message);
}


void
MainWindow::initArgs(const std::vector<std::string>& params)
{
    this->m_ui.tableWidget_args->clearContents();
    this->m_ui.tableWidget_args->setRowCount(static_cast<int>(params.size()));
    int i = 0;
    for (const auto& param : params) {
        this->m_ui.tableWidget_args->setVerticalHeaderItem(i++, new QTableWidgetItem{QString::fromStdString(param)});
    }
}


std::map<std::string, std::string>
MainWindow::scriptArgs() const
{
    std::map<std::string, std::string> kwargs;
    for (int row = 0; row < this->m_ui.tableWidget_args->rowCount(); ++row) {
        auto param = this->m_ui.tableWidget_args->verticalHeaderItem(row)->text().toStdString();
        auto arg = this->m_ui.tableWidget_args->item(row, 0);
        if (arg != nullptr && !arg->text().isEmpty())
            kwargs[param] = arg->text().toStdString();
    }
    return kwargs;
}


void
MainWindow::setScriptStatus(const QString& message)
{
    this->m_ui.label_scriptStatus->setText(message);
}


QPlot*
MainWindow::plot(std::size_t n)
{
    return this->m_plots.at(n);
}


std::size_t
MainWindow::plotCount() const
{
    return this->m_plots.size();
}


void
MainWindow::closeEvent(QCloseEvent* event)
{
    // Since QWidget::close will also invoke this method, we use
    //   a flag to check if we're invoked from the close method
    //   programmatically or from the user. User invocations will
    //   be translated to a MainWindow::closed signal emission,
    //   whereas programmatic ones will use the default method.
    if (this->m_programmaticClose) {
        this->m_programmaticClose = false;
        return QMainWindow::closeEvent(event);
    }
    event->ignore();
    emit this->closed();
}


void
MainWindow::redraw()
{
    for (const auto& plot : this->m_plots)
        plot->redraw();
}
