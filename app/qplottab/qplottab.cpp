#include <QIntValidator>

#include "qplottabprivate.hpp"


void
QPlotTab::MinSizeFrame::setCache(const QPlotTab::MinSizeFrame::Cache& cache)
{
    this->setWidth(cache.width);
    this->setHeight(cache.height);
}


QPlotTab::MinSizeFrame::Cache
QPlotTab::MinSizeFrame::cache() const
{
    return {
        .width = this->width(),
        .height = this->height()
    };
}


void
QPlotTab::RangeBox::setCache(const QPlotTab::RangeBox::Cache& cache)
{
    this->setMin(cache.min);
    this->setMax(cache.max);
}


QPlotTab::RangeBox::Cache
QPlotTab::RangeBox::cache() const
{
    return {
        .min = this->min(),
        .max = this->max()
    };
}


void
QPlotTab::LineBox::setCache(const QPlotTab::LineBox::Cache& cache)
{
    this->setType(cache.type);
    this->setColor(cache.color);
    this->setStyle(cache.style);
}


QPlotTab::LineBox::Cache
QPlotTab::LineBox::cache() const
{
    return {
        .type = this->type(),
        .color = this->color(),
        .style = this->style()
    };
}


void
QPlotTab::PointsBox::setCache(const QPlotTab::PointsBox::Cache& cache)
{
    this->setShape(cache.shape);
    this->setColor(cache.color);
    this->setSize(cache.size);
}


QPlotTab::PointsBox::Cache
QPlotTab::PointsBox::cache() const
{
    return {
        .shape = this->shape(),
        .color = this->color(),
        .size = this->size()
    };
}


void
QPlotTab::DataSizeBox::setCache(const QPlotTab::DataSizeBox::Cache& cache)
{
    this->setX(cache.x);
    this->setY(cache.y);
}


QPlotTab::DataSizeBox::Cache
QPlotTab::DataSizeBox::cache() const
{
    return {
        .x = this->x(),
        .y = this->y()
    };
}


void
QPlotTab::ColorBox::setCache(const QPlotTab::ColorBox::Cache& cache)
{
    this->setMin(cache.min);
    this->setMax(cache.max);
}


QPlotTab::ColorBox::Cache
QPlotTab::ColorBox::cache() const
{
    return {
        .min = this->min(),
        .max = this->max()
    };
}


void
QPlotTab::SubTab2D::setCache(const QPlotTab::SubTab2D::Cache& cache)
{
    this->rangeBoxX()->setCache(cache.xRange);
    this->rangeBoxY()->setCache(cache.yRange);
    this->lineBox()->setCache(cache.line);
    this->pointsBox()->setCache(cache.points);
}


QPlotTab::SubTab2D::Cache
QPlotTab::SubTab2D::cache() const
{
    return {
        .xRange = this->rangeBoxX()->cache(),
        .yRange = this->rangeBoxY()->cache(),
        .line = this->lineBox()->cache(),
        .points = this->pointsBox()->cache()
    };
}


void QPlotTab::SubTabColorMap::setCache(const QPlotTab::SubTabColorMap::Cache& cache)
{
    this->rangeBoxX()->setCache(cache.xRange);
    this->rangeBoxY()->setCache(cache.yRange);
    this->rangeBoxZ()->setCache(cache.zRange);
    this->dataSizeBox()->setCache(cache.dataSize);
    this->colorBox()->setCache(cache.color);
}


QPlotTab::SubTabColorMap::Cache
QPlotTab::SubTabColorMap::cache() const
{
    return {
        .xRange = this->rangeBoxX()->cache(),
        .yRange = this->rangeBoxY()->cache(),
        .zRange = this->rangeBoxZ()->cache(),
        .dataSize = this->dataSizeBox()->cache(),
        .color = this->colorBox()->cache()
    };
}


QPlotTab::QPlotTab(QWidget* parent)
    : QWidget{parent}
    , m_layout{new QVBoxLayout{this}}
    , m_frame_common{new QFrame{this}}
    , m_tabWidget{new QTabWidget{this}}
    , m_layout_common{new QFormLayout{m_frame_common}}
    , m_label_title{new QLabel{"Title", m_frame_common}}
    , m_label_xAxis{new QLabel{"X-Axis", m_frame_common}}
    , m_label_yAxis{new QLabel{"Y-Axis", m_frame_common}}
    , m_label_minSize{new QLabel{"Min Size", m_frame_common}}
    , m_lineEdit_title{new QLineEdit{m_frame_common}}
    , m_lineEdit_xAxis{new QLineEdit{m_frame_common}}
    , m_lineEdit_yAxis{new QLineEdit{m_frame_common}}
    , m_minSizeFrame{new MinSizeFramePrivate{m_frame_common}}
    , m_tab2D{new SubTab2DPrivate{m_tabWidget}}
    , m_tabColorMap{new SubTabColorMapPrivate{m_tabWidget}}
{
    this->m_layout_common->setWidget(0, QFormLayout::LabelRole, this->m_label_title);
    this->m_layout_common->setWidget(0, QFormLayout::FieldRole, this->m_lineEdit_title);
    this->m_layout_common->setWidget(1, QFormLayout::LabelRole, this->m_label_xAxis);
    this->m_layout_common->setWidget(1, QFormLayout::FieldRole, this->m_lineEdit_xAxis);
    this->m_layout_common->setWidget(2, QFormLayout::LabelRole, this->m_label_yAxis);
    this->m_layout_common->setWidget(2, QFormLayout::FieldRole, this->m_lineEdit_yAxis);
    this->m_layout_common->setWidget(3, QFormLayout::LabelRole, this->m_label_minSize);
    this->m_layout_common->setWidget(3, QFormLayout::FieldRole, this->m_minSizeFrame);
    this->m_tabWidget->addTab(this->m_tab2D, "2D");
    this->m_tabWidget->addTab(this->m_tabColorMap, "Color Map");
    this->m_layout->addWidget(this->m_frame_common);
    this->m_layout->addWidget(this->m_tabWidget);
}


void
QPlotTab::setCache(const QPlotTab::Cache& cache)
{
    this->m_lineEdit_title->setText(cache.title);
    this->m_lineEdit_xAxis->setText(cache.xAxis);
    this->m_lineEdit_yAxis->setText(cache.yAxis);
    this->m_minSizeFrame->setCache(cache.minSize);
    this->m_tab2D->setCache(cache.twoDimen);
    this->m_tabColorMap->setCache(cache.colorMap);
}


QPlotTab::Cache
QPlotTab::cache() const
{
    return {
        .title = this->m_lineEdit_title->text(),
        .xAxis = this->m_lineEdit_xAxis->text(),
        .yAxis = this->m_lineEdit_yAxis->text(),
        .minSize = this->m_minSizeFrame->cache(),
        .twoDimen = this->m_tab2D->cache(),
        .colorMap = this->m_tabColorMap->cache()
    };
}


void
QPlotTab::setTitle(const QString& text)
{
    this->m_lineEdit_title->setText(text);
}


QString
QPlotTab::title() const
{
    return this->m_lineEdit_title->text();
}


void
QPlotTab::setXAxis(const QString& text)
{
    this->m_lineEdit_xAxis->setText(text);
}


QString
QPlotTab::xAxis() const
{
    return this->m_lineEdit_xAxis->text();
}


void
QPlotTab::setYAxis(const QString& text)
{
    this->m_lineEdit_yAxis->setText(text);
}


QString
QPlotTab::yAxis() const
{
    return this->m_lineEdit_yAxis->text();
}


QPlotTab::MinSizeFrame*
QPlotTab::minSize()
{
    return static_cast<MinSizeFrame*>(this->m_minSizeFrame);
}


const QPlotTab::MinSizeFrame*
QPlotTab::minSize() const
{
    return static_cast<const MinSizeFrame*>(this->m_minSizeFrame);
}


QPlotTab::SubTab2D*
QPlotTab::tab2D()
{
    return static_cast<SubTab2D*>(this->m_tab2D);
}


const QPlotTab::SubTab2D*
QPlotTab::tab2D() const
{
    return static_cast<const SubTab2D*>(this->m_tab2D);
}


QPlotTab::SubTabColorMap*
QPlotTab::tabColorMap()
{
    return static_cast<SubTabColorMap*>(this->m_tabColorMap);
}


const QPlotTab::SubTabColorMap*
QPlotTab::tabColorMap() const
{
    return static_cast<const SubTabColorMap*>(this->m_tabColorMap);
}


QPlot::Type
QPlotTab::selected() const
{
    switch (this->m_tabWidget->currentIndex()) {
        default:
        case 0: return QPlot::Type::TWODIMEN;
        case 1: return QPlot::Type::COLORMAP;
    }
}
