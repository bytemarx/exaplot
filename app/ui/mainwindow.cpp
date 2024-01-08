#include "mainwindow.hpp"


MainWindow::MainWindow()
    : QMainWindow{nullptr}
{
    this->ui.setupUi(this);
}


QAction*
MainWindow::actionLoad()
{
    return this->ui.actionLoadScript;
}


QAction*
MainWindow::actionAbout()
{
    return this->ui.actionAbout;
}


QAction*
MainWindow::actionPlotEditor()
{
    return this->ui.actionPlotEditor;
}


void
MainWindow::setPlots(const std::vector<PlotEditor::PlotInfo>& plots)
{
    for (auto plot : this->m_plots)
        this->ui.gridLayout_plots->removeWidget(plot);

    if (this->m_plots.size() < plots.size()) {
        for (auto i = this->m_plots.size(); i < plots.size(); ++i)
            this->m_plots.push_back(new QPlot{this->ui.widget_plotPanel});
    } else if (this->m_plots.size() > plots.size()) {
        for (auto i = this->m_plots.size(); i > plots.size(); --i)
            delete this->m_plots[i - 1];
        this->m_plots.resize(plots.size());
    }

    for (std::size_t i = 0; i < plots.size(); ++i) {
        auto plot = this->m_plots[i];
        this->ui.gridLayout_plots->addWidget(
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
    this->ui.plainTextEdit_messages->setPlainText(message);
}
