#include <QMessageBox>

#include "appui.hpp"


AppUI::AppUI(QObject* parent)
    : QObject{parent}
    , mainWindow{new MainWindow}
    , aboutDialog{new About{this->mainWindow}}
    , plotEditorDialog{new PlotEditor{this->mainWindow}}
{
    this->mainWindow->setPlots(this->plotEditorDialog->plots());

    QObject::connect(
        this->mainWindow, &MainWindow::closed,
        [this] { emit this->closed(); }
    );
    QObject::connect(
        this->mainWindow->actionLoad(), &QAction::triggered,
        this, &AppUI::loadScript
    );
    QObject::connect(
        this->mainWindow->buttonRun(), &QPushButton::clicked,
        [this] { emit this->scriptRun(this->scriptArgs()); }
    );
    QObject::connect(
        this->mainWindow->buttonStop(), &QPushButton::clicked,
        [this] { emit this->scriptStop(); }
    );
    QObject::connect(
        this->mainWindow->actionAbout(), &QAction::triggered,
        this->aboutDialog, &About::open
    );
    QObject::connect(
        this->mainWindow->actionPlotEditor(), &QAction::triggered,
        this->plotEditorDialog, &PlotEditor::open
    );
    QObject::connect(
        this->plotEditorDialog, &QDialog::accepted,
        [this] { this->mainWindow->setPlots(this->plotEditorDialog->plots()); }
    );
    QObject::connect(
        this->mainWindow, &MainWindow::plotsSet,
        [this](const std::vector<PlotEditor::PlotInfo>& plots) { emit this->plotsSet(plots); }
    );
}


AppUI::~AppUI()
{
    delete this->mainWindow;
}


void
AppUI::show()
{
    this->mainWindow->show();
}


bool
AppUI::close()
{
    return this->mainWindow->close();
}


void
AppUI::reset()
{
    this->plotEditorDialog->reset();
    this->mainWindow->setPlots(this->plotEditorDialog->plots());
    this->mainWindow->plot(0)->clear();
    this->mainWindow->setMessage();
}


void
AppUI::clear()
{
    auto n = this->mainWindow->plotCount();
    for (decltype(n) i = 0; i < n; ++i)
        this->mainWindow->plot(i)->clear();
}


std::vector<std::string>
AppUI::scriptArgs() const
{
    return this->mainWindow->scriptArgs();
}


void
AppUI::setMessage(const QString& message, bool append)
{
    this->mainWindow->setMessage(message, append);
}


bool
AppUI::init(
    const std::vector<zeta::GridPoint>& arrangement,
    const std::vector<std::pair<std::string, std::string>>& params)
{
    if (!this->plotEditorDialog->setArrangement(arrangement))
        return false;

    this->mainWindow->setPlots(this->plotEditorDialog->plots());
    this->mainWindow->initArgs(params);
    return true;
}


void
AppUI::setScriptStatus(const QString& message)
{
    this->mainWindow->setScriptStatus(message);
}


QPlot*
AppUI::plot(std::size_t plotIdx)
{
    assert(plotIdx < this->mainWindow->plotCount());
    return this->mainWindow->plot(plotIdx);
}


std::size_t
AppUI::plotCount() const
{
    return this->mainWindow->plotCount();
}


void
AppUI::enableRun(bool enable)
{
    this->mainWindow->enableRun(enable);
}


void
AppUI::enableStop(bool enable)
{
    this->mainWindow->enableStop(enable);
}


void
AppUI::setPlotProperty(
    std::size_t plotIdx,
    const zeta::PlotProperty& property,
    const QPlotTab::Cache& properties)
{
    auto plot = this->plot(plotIdx);
    using PlotProperty = zeta::PlotProperty;
    switch (property) {
    case PlotProperty::TITLE:
        plot->setTitle(properties.title);
        break;
    case PlotProperty::XAXIS:
        plot->setLabelX(properties.xAxis);
        break;
    case PlotProperty::YAXIS:
        plot->setLabelY(properties.yAxis);
        break;
    case PlotProperty::MINSIZE_W:
        plot->setMinimumWidth(properties.minSize.width);
        break;
    case PlotProperty::MINSIZE_H:
        plot->setMinimumHeight(properties.minSize.height);
        break;
    case PlotProperty::TWODIMEN_XRANGE_MIN:
    case PlotProperty::TWODIMEN_XRANGE_MAX:
        plot->plot2D()->setRangeX({properties.twoDimen.xRange.min, properties.twoDimen.xRange.max});
        break;
    case PlotProperty::TWODIMEN_YRANGE_MIN:
    case PlotProperty::TWODIMEN_YRANGE_MAX:
        plot->plot2D()->setRangeY({properties.twoDimen.yRange.min, properties.twoDimen.yRange.max});
        break;
    case PlotProperty::TWODIMEN_LINE_TYPE:
        plot->plot2D()->setLineStyle(properties.twoDimen.line.type);
        break;
    case PlotProperty::TWODIMEN_LINE_COLOR:
    case PlotProperty::TWODIMEN_LINE_STYLE:
        {
            QPen pen{properties.twoDimen.line.color};
            pen.setStyle(properties.twoDimen.line.style);
            plot->plot2D()->setPen(pen);
        }
        break;
    case PlotProperty::TWODIMEN_POINTS_SHAPE:
    case PlotProperty::TWODIMEN_POINTS_COLOR:
    case PlotProperty::TWODIMEN_POINTS_SIZE:
        plot->plot2D()->setScatStyle({
            properties.twoDimen.points.shape,
            properties.twoDimen.points.color,
            properties.twoDimen.points.size
        });
        break;
    case PlotProperty::TWODIMEN_AUTORS_AXES:
        plot->plot2D()->setRescaleAxes(properties.twoDimen.autoRescaleAxes);
        break;
    case PlotProperty::COLORMAP_XRANGE_MIN:
    case PlotProperty::COLORMAP_XRANGE_MAX:
        plot->plotColorMap()->setRangeX({properties.colorMap.xRange.min, properties.colorMap.xRange.max});
        break;
    case PlotProperty::COLORMAP_YRANGE_MIN:
    case PlotProperty::COLORMAP_YRANGE_MAX:
        plot->plotColorMap()->setRangeY({properties.colorMap.yRange.min, properties.colorMap.yRange.max});
        break;
    case PlotProperty::COLORMAP_ZRANGE_MIN:
    case PlotProperty::COLORMAP_ZRANGE_MAX:
        plot->plotColorMap()->setRangeZ({properties.colorMap.zRange.min, properties.colorMap.zRange.max});
        break;
    case PlotProperty::COLORMAP_DATASIZE_X:
    case PlotProperty::COLORMAP_DATASIZE_Y:
        plot->plotColorMap()->setDataSize(properties.colorMap.dataSize.x, properties.colorMap.dataSize.y);
        break;
    case PlotProperty::COLORMAP_COLOR_MIN:
    case PlotProperty::COLORMAP_COLOR_MAX:
        {
            QCPColorGradient colorGradient;
            colorGradient.setColorStopAt(0, properties.colorMap.color.min);
            colorGradient.setColorStopAt(1, properties.colorMap.color.max);
            plot->plotColorMap()->setColorGradient(colorGradient);
        }
        break;
    case PlotProperty::COLORMAP_AUTORS_AXES:
        plot->plotColorMap()->setRescaleAxes(properties.colorMap.autoRescaleAxes);
        break;
    case PlotProperty::COLORMAP_AUTORS_DATA:
        plot->plotColorMap()->setRescaleData(properties.colorMap.autoRescaleData);
        break;
    default:
        return this->displayError(QString{"Invalid plot property (%1)"}.arg(property));
    }
    this->plotEditorDialog->setPlot(plotIdx, properties);
}


void
AppUI::showPlot(std::size_t plotIdx, QPlot::Type plotType)
{
    this->plot(plotIdx)->setType(plotType);
    this->plotEditorDialog->setSelectedPlot(plotIdx, plotType);
}


void
AppUI::displayError(const QString& msg, const QString& title)
{
    QMessageBox msgBox{this->mainWindow};
    msgBox.setWindowTitle(title);
    msgBox.setText(msg);
    msgBox.setStyleSheet(
        "QLabel {"
            "font-family: 'Monospace';"
            "font-size: 10pt;"
        "}"
    );
    msgBox.exec();
}


void
AppUI::loadScript()
{
    auto file = QFileDialog::getOpenFileName(
        this->mainWindow,
        tr("Load Script File"),
        QString{},
        tr("Python Files (*.py *.opy);;All Files (*)")
    );
    if (file.isEmpty()) return;
    emit this->scriptLoad(file);
}
