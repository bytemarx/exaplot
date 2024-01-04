#include <QIntValidator>

#include "qplottabprivate.hpp"


MinSizeFramePrivate::MinSizeFramePrivate(QWidget* parent)
    : QFrame{parent}
    , m_layout{new QHBoxLayout{this}}
    , m_label_width{new QLabel{"W,", this}}
    , m_label_height{new QLabel{"H", this}}
    , m_lineEdit_width{new QLineEdit{"0", this}}
    , m_lineEdit_height{new QLineEdit{"0", this}}
{
    auto validator = new QIntValidator{0, INT_MAX, this};
    this->m_lineEdit_width->setValidator(validator);
    this->m_lineEdit_height->setValidator(validator);
    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->addWidget(this->m_lineEdit_width);
    this->m_layout->addWidget(this->m_label_width);
    this->m_layout->addWidget(this->m_lineEdit_height);
    this->m_layout->addWidget(this->m_label_height);
}


void
MinSizeFramePrivate::setWidth(int val)
{
    if (val < 0) return;
    this->m_lineEdit_width->setText(QString::number(val));
}


int
MinSizeFramePrivate::width() const
{
    return this->m_lineEdit_width->text().toInt();
}


void
MinSizeFramePrivate::setHeight(int val)
{
    if (val < 0) return;
    this->m_lineEdit_height->setText(QString::number(val));
}


int
MinSizeFramePrivate::height() const
{
    return this->m_lineEdit_height->text().toInt();
}


RangeBoxPrivate::RangeBoxPrivate(const QString& title, QWidget* parent)
    : QGroupBox{title, parent}
    , m_layout{new QHBoxLayout{this}}
    , m_label_min{new QLabel{"Min", this}}
    , m_label_max{new QLabel{"Max", this}}
    , m_lineEdit_min{new QLineEdit{this}}
    , m_lineEdit_max{new QLineEdit{this}}
{
    this->m_layout->addWidget(this->m_label_min);
    this->m_layout->addWidget(this->m_lineEdit_min);
    this->m_layout->addWidget(this->m_label_max);
    this->m_layout->addWidget(this->m_lineEdit_max);
}


void
RangeBoxPrivate::setMin(const QString& text)
{
    this->m_lineEdit_min->setText(text);
}


QString
RangeBoxPrivate::min() const
{
    return this->m_lineEdit_min->text();
}


void
RangeBoxPrivate::setMax(const QString& text)
{
    this->m_lineEdit_max->setText(text);
}


QString
RangeBoxPrivate::max() const
{
    return this->m_lineEdit_max->text();
}


LineBoxPrivate::LineBoxPrivate(const QString& title, QWidget* parent)
    : QGroupBox{title, parent}
    , m_layout{new QFormLayout{this}}
    , m_label_type{new QLabel{"Type", this}}
    , m_label_color{new QLabel{"Color", this}}
    , m_label_style{new QLabel{"Style", this}}
    , m_comboBox_type{new QComboBox{this}}
    , m_comboBox_style{new QComboBox{this}}
    , m_pushButton_color{new QPushButton{this}}
    , m_colorDialog{new QColorDialog{Qt::GlobalColor::black, this}}
{
    this->m_comboBox_type->addItems({
        "None",
        "Line",
        "Step Left",
        "Step Right",
        "Step Center",
        "Impulse"
    });
    this->m_comboBox_type->setCurrentIndex(1);

    this->m_comboBox_style->addItems({
        "Solid",
        "Dashed",
        "Dotted",
        "Dash-Dotted",
        "Dash-Double-Dotted"
    });

    this->m_pushButton_color->setAutoFillBackground(true);
    this->setColor(Qt::GlobalColor::black);

    this->m_layout->setWidget(0, QFormLayout::LabelRole, this->m_label_type);
    this->m_layout->setWidget(0, QFormLayout::FieldRole, this->m_comboBox_type);
    this->m_layout->setWidget(1, QFormLayout::LabelRole, this->m_label_color);
    this->m_layout->setWidget(1, QFormLayout::FieldRole, this->m_pushButton_color);
    this->m_layout->setWidget(2, QFormLayout::LabelRole, this->m_label_style);
    this->m_layout->setWidget(2, QFormLayout::FieldRole, this->m_comboBox_style);

    QObject::connect(
        this->m_pushButton_color, &QPushButton::clicked,
        [=] { this->m_colorDialog->open(); }
    );
    QObject::connect(
        this->m_colorDialog, &QColorDialog::accepted,
        this, &LineBoxPrivate::colorSelected
    );
}


void
LineBoxPrivate::setType(QCPGraph::LineStyle type)
{
    auto index = static_cast<int>(type);
    if (index < 0 || index > 5) return;
    this->m_comboBox_type->setCurrentIndex(index);
}


QCPGraph::LineStyle
LineBoxPrivate::type() const
{
    return static_cast<QCPGraph::LineStyle>(this->m_comboBox_type->currentIndex());
}


void
LineBoxPrivate::setColor(const QColor& color)
{
    QPalette palette = this->m_pushButton_color->palette();
    palette.setColor(this->m_pushButton_color->backgroundRole(), color);
    this->m_pushButton_color->setPalette(palette);
    this->m_colorDialog->setCurrentColor(color);
}


QColor
LineBoxPrivate::color() const
{
    return this->m_colorDialog->selectedColor();
}


void
LineBoxPrivate::setStyle(Qt::PenStyle style)
{
    auto index = static_cast<int>(style);
    if (index < 1 || index > 5) return;
    this->m_comboBox_style->setCurrentIndex(index - 1);
}


Qt::PenStyle
LineBoxPrivate::style() const
{
    return static_cast<Qt::PenStyle>(this->m_comboBox_style->currentIndex() + 1);
}


void
LineBoxPrivate::colorSelected()
{
    this->setColor(this->m_colorDialog->selectedColor());
}


PointsBoxPrivate::PointsBoxPrivate(const QString& title, QWidget* parent)
    : QGroupBox{title, parent}
    , m_layout{new QFormLayout{this}}
    , m_label_shape{new QLabel{"Shape", this}}
    , m_label_color{new QLabel{"Color", this}}
    , m_label_size{new QLabel{"Size", this}}
    , m_comboBox_shape{new QComboBox{this}}
    , m_pushButton_color{new QPushButton{this}}
    , m_spinBox_size{new QDoubleSpinBox{this}}
    , m_colorDialog{new QColorDialog{Qt::GlobalColor::black, this}}
{
    this->m_comboBox_shape->addItems({
        "None",
        "Dot",
        "Cross",
        "Plus",
        "Circle",
        "Disc",
        "Square",
        "Diamond",
        "Star",
        "Triangle",
        "Triangle Inverted",
        "Cross Square",
        "Plus Square",
        "Cross Circle",
        "Plus Circle",
        "Peace"
    });

    this->m_pushButton_color->setAutoFillBackground(true);
    this->setColor(Qt::GlobalColor::black);

    this->m_spinBox_size->setValue(4.0);

    this->m_layout->setWidget(0, QFormLayout::LabelRole, this->m_label_shape);
    this->m_layout->setWidget(0, QFormLayout::FieldRole, this->m_comboBox_shape);
    this->m_layout->setWidget(1, QFormLayout::LabelRole, this->m_label_color);
    this->m_layout->setWidget(1, QFormLayout::FieldRole, this->m_pushButton_color);
    this->m_layout->setWidget(2, QFormLayout::LabelRole, this->m_label_size);
    this->m_layout->setWidget(2, QFormLayout::FieldRole, this->m_spinBox_size);

    QObject::connect(
        this->m_pushButton_color, &QPushButton::clicked,
        [=] { this->m_colorDialog->open(); }
    );
    QObject::connect(
        this->m_colorDialog, &QColorDialog::accepted,
        this, &PointsBoxPrivate::colorSelected
    );
}


void
PointsBoxPrivate::setShape(QCPScatterStyle::ScatterShape shape)
{
    auto index = static_cast<int>(shape);
    if (index < 0 || index > 15) return;
    this->m_comboBox_shape->setCurrentIndex(index);
}


QCPScatterStyle::ScatterShape
PointsBoxPrivate::shape() const
{
    return static_cast<QCPScatterStyle::ScatterShape>(this->m_comboBox_shape->currentIndex());
}


void
PointsBoxPrivate::setColor(const QColor& color)
{
    QPalette palette = this->m_pushButton_color->palette();
    palette.setColor(this->m_pushButton_color->backgroundRole(), color);
    this->m_pushButton_color->setPalette(palette);
    this->m_colorDialog->setCurrentColor(color);
}


QColor
PointsBoxPrivate::color() const
{
    return this->m_colorDialog->selectedColor();
}


void
PointsBoxPrivate::setSize(double size)
{
    this->m_spinBox_size->setValue(size);
}


double
PointsBoxPrivate::size() const
{
    return this->m_spinBox_size->value();
}


void
PointsBoxPrivate::colorSelected()
{
    this->setColor(this->m_colorDialog->selectedColor());
}


DataSizeBoxPrivate::DataSizeBoxPrivate(const QString& title, QWidget* parent)
    : QGroupBox{title, parent}
    , m_layout{new QHBoxLayout{this}}
    , m_label_x{new QLabel{"X", this}}
    , m_label_y{new QLabel{", Y", this}}
    , m_lineEdit_x{new QLineEdit{"0", this}}
    , m_lineEdit_y{new QLineEdit{"0", this}}
{
    QIntValidator* validator = new QIntValidator{0, INT_MAX, this};
    this->m_lineEdit_x->setValidator(validator);
    this->m_lineEdit_y->setValidator(validator);
    this->m_layout->addWidget(this->m_label_x);
    this->m_layout->addWidget(this->m_lineEdit_x);
    this->m_layout->addWidget(this->m_label_y);
    this->m_layout->addWidget(this->m_lineEdit_y);
}


void
DataSizeBoxPrivate::setX(int val)
{
    if (val < 0) return;
    this->m_lineEdit_x->setText(QString::number(val));
}


int
DataSizeBoxPrivate::x() const
{
    return this->m_lineEdit_x->text().toInt();
}


void
DataSizeBoxPrivate::setY(int val)
{
    if (val < 0) return;
    this->m_lineEdit_y->setText(QString::number(val));
}


int
DataSizeBoxPrivate::y() const
{
    return this->m_lineEdit_y->text().toInt();
}


ColorBoxPrivate::ColorBoxPrivate(const QString& title, QWidget* parent)
    : QGroupBox{title, parent}
    , m_layout{new QFormLayout{this}}
    , m_label_min{new QLabel{"Min", this}}
    , m_label_max{new QLabel{"Max", this}}
    , m_pushButton_min{new QPushButton{this}}
    , m_pushButton_max{new QPushButton{this}}
    , m_colorDialog_min{new QColorDialog{Qt::GlobalColor::white, this}}
    , m_colorDialog_max{new QColorDialog{Qt::GlobalColor::black, this}}
{
    this->m_pushButton_min->setAutoFillBackground(true);
    this->m_pushButton_max->setAutoFillBackground(true);
    this->setMin(Qt::GlobalColor::white);
    this->setMax(Qt::GlobalColor::black);

    this->m_layout->setWidget(0, QFormLayout::LabelRole, this->m_label_min);
    this->m_layout->setWidget(0, QFormLayout::FieldRole, this->m_pushButton_min);
    this->m_layout->setWidget(1, QFormLayout::LabelRole, this->m_label_max);
    this->m_layout->setWidget(1, QFormLayout::FieldRole, this->m_pushButton_max);

    QObject::connect(
        this->m_pushButton_min, &QPushButton::clicked,
        [=] { this->m_colorDialog_min->open(); }
    );
    QObject::connect(
        this->m_pushButton_max, &QPushButton::clicked,
        [=] { this->m_colorDialog_max->open(); }
    );
    QObject::connect(
        this->m_colorDialog_min, &QColorDialog::accepted,
        this, &ColorBoxPrivate::minColorSelected
    );
    QObject::connect(
        this->m_colorDialog_max, &QColorDialog::accepted,
        this, &ColorBoxPrivate::maxColorSelected
    );
}


void
ColorBoxPrivate::setMin(const QColor& color)
{
    QPalette palette = this->m_pushButton_min->palette();
    palette.setColor(this->m_pushButton_min->backgroundRole(), color);
    this->m_pushButton_min->setPalette(palette);
    this->m_colorDialog_min->setCurrentColor(color);
}


QColor
ColorBoxPrivate::min() const
{
    return this->m_colorDialog_min->selectedColor();
}


void
ColorBoxPrivate::setMax(const QColor& color)
{
    QPalette palette = this->m_pushButton_max->palette();
    palette.setColor(this->m_pushButton_max->backgroundRole(), color);
    this->m_pushButton_max->setPalette(palette);
    this->m_colorDialog_max->setCurrentColor(color);
}


QColor
ColorBoxPrivate::max() const
{
    return this->m_colorDialog_max->selectedColor();
}


void
ColorBoxPrivate::minColorSelected()
{
    this->setMin(this->m_colorDialog_min->selectedColor());
}


void
ColorBoxPrivate::maxColorSelected()
{
    this->setMax(this->m_colorDialog_max->selectedColor());
}


SubTab2DPrivate::SubTab2DPrivate(QWidget* parent)
    : QWidget{parent}
    , m_layout{new QVBoxLayout{this}}
    , m_scrollArea{new QScrollArea{this}}
    , m_contents{new QWidget{this}}
    , m_layout_contents{new QVBoxLayout{m_contents}}
    , m_rangeBox_x{new RangeBoxPrivate{"X-Range", m_contents}}
    , m_rangeBox_y{new RangeBoxPrivate{"Y-Range", m_contents}}
    , m_lineBox{new LineBoxPrivate{"Line", m_contents}}
    , m_pointsBox{new PointsBoxPrivate{"Points", m_contents}}
    , m_spacer{new QSpacerItem{0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding}}
{
    this->m_scrollArea->setFrameShape(QFrame::NoFrame);
    this->m_scrollArea->setWidgetResizable(true);
    this->m_layout_contents->addWidget(this->m_rangeBox_x);
    this->m_layout_contents->addWidget(this->m_rangeBox_y);
    this->m_layout_contents->addWidget(this->m_lineBox);
    this->m_layout_contents->addWidget(this->m_pointsBox);
    this->m_layout_contents->addItem(this->m_spacer);
    this->m_scrollArea->setWidget(this->m_contents);
    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->addWidget(this->m_scrollArea);
}


QPlotTab::RangeBox*
SubTab2DPrivate::rangeBoxX()
{
    return static_cast<QPlotTab::RangeBox*>(this->m_rangeBox_x);
}


const QPlotTab::RangeBox*
SubTab2DPrivate::rangeBoxX() const
{
    return static_cast<const QPlotTab::RangeBox*>(this->m_rangeBox_x);
}


QPlotTab::RangeBox*
SubTab2DPrivate::rangeBoxY()
{
    return static_cast<QPlotTab::RangeBox*>(this->m_rangeBox_y);
}


const QPlotTab::RangeBox*
SubTab2DPrivate::rangeBoxY() const
{
    return static_cast<const QPlotTab::RangeBox*>(this->m_rangeBox_y);
}


QPlotTab::LineBox*
SubTab2DPrivate::lineBox()
{
    return static_cast<QPlotTab::LineBox*>(this->m_lineBox);
}


const QPlotTab::LineBox*
SubTab2DPrivate::lineBox() const
{
    return static_cast<const QPlotTab::LineBox*>(this->m_lineBox);
}


QPlotTab::PointsBox*
SubTab2DPrivate::pointsBox()
{
    return static_cast<QPlotTab::PointsBox*>(this->m_pointsBox);
}


const QPlotTab::PointsBox*
SubTab2DPrivate::pointsBox() const
{
    return static_cast<const QPlotTab::PointsBox*>(this->m_pointsBox);
}


SubTabColorMapPrivate::SubTabColorMapPrivate(QWidget* parent)
    : QWidget{parent}
    , m_layout{new QVBoxLayout{this}}
    , m_scrollArea{new QScrollArea{this}}
    , m_contents{new QWidget{this}}
    , m_layout_contents{new QVBoxLayout{m_contents}}
    , m_rangeBox_x{new RangeBoxPrivate{"X-Range", m_contents}}
    , m_rangeBox_y{new RangeBoxPrivate{"Y-Range", m_contents}}
    , m_rangeBox_z{new RangeBoxPrivate{"Z-Range", m_contents}}
    , m_dataSizeBox{new DataSizeBoxPrivate{"Data Size", m_contents}}
    , m_colorBox{new ColorBoxPrivate{"Color", m_contents}}
    , m_spacer{new QSpacerItem{0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding}}
{
    this->m_scrollArea->setFrameShape(QFrame::NoFrame);
    this->m_scrollArea->setWidgetResizable(true);
    this->m_layout_contents->addWidget(this->m_rangeBox_x);
    this->m_layout_contents->addWidget(this->m_rangeBox_y);
    this->m_layout_contents->addWidget(this->m_rangeBox_z);
    this->m_layout_contents->addWidget(this->m_dataSizeBox);
    this->m_layout_contents->addWidget(this->m_colorBox);
    this->m_layout_contents->addItem(this->m_spacer);
    this->m_scrollArea->setWidget(this->m_contents);
    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->addWidget(this->m_scrollArea);
}


QPlotTab::RangeBox*
SubTabColorMapPrivate::rangeBoxX()
{
    return static_cast<QPlotTab::RangeBox*>(this->m_rangeBox_x);
}


const QPlotTab::RangeBox*
SubTabColorMapPrivate::rangeBoxX() const
{
    return static_cast<const QPlotTab::RangeBox*>(this->m_rangeBox_x);
}


QPlotTab::RangeBox*
SubTabColorMapPrivate::rangeBoxY()
{
    return static_cast<QPlotTab::RangeBox*>(this->m_rangeBox_y);
}


const QPlotTab::RangeBox*
SubTabColorMapPrivate::rangeBoxY() const
{
    return static_cast<const QPlotTab::RangeBox*>(this->m_rangeBox_y);
}


QPlotTab::RangeBox*
SubTabColorMapPrivate::rangeBoxZ()
{
    return static_cast<QPlotTab::RangeBox*>(this->m_rangeBox_z);
}


const QPlotTab::RangeBox*
SubTabColorMapPrivate::rangeBoxZ() const
{
    return static_cast<const QPlotTab::RangeBox*>(this->m_rangeBox_z);
}


QPlotTab::DataSizeBox*
SubTabColorMapPrivate::dataSizeBox()
{
    return static_cast<QPlotTab::DataSizeBox*>(this->m_dataSizeBox);
}


const QPlotTab::DataSizeBox*
SubTabColorMapPrivate::dataSizeBox() const
{
    return static_cast<const QPlotTab::DataSizeBox*>(this->m_dataSizeBox);
}


QPlotTab::ColorBox*
SubTabColorMapPrivate::colorBox()
{
    return static_cast<QPlotTab::ColorBox*>(this->m_colorBox);
}


const QPlotTab::ColorBox*
SubTabColorMapPrivate::colorBox() const
{
    return static_cast<const QPlotTab::ColorBox*>(this->m_colorBox);
}
