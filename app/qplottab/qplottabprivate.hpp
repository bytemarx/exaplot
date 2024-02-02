#pragma once

#include <QSpacerItem>

#include "qplottab.hpp"

#include <limits>


class QNumberEdit : public QLineEdit
{
    Q_OBJECT

public:
    virtual ~QNumberEdit() = default;

protected:
    QNumberEdit(QWidget*, const QString&);
    void focusInEvent(QFocusEvent*) override;
    void focusOutEvent(QFocusEvent*) override;

private:
    bool validate();

    QString m_cache;
};


class QIntEdit : public QNumberEdit
{
    Q_OBJECT

public:
    QIntEdit(
        QWidget* parent,
        int defaultValue,
        int min = std::numeric_limits<int>::lowest(),
        int max = std::numeric_limits<int>::max()
    );
};


class QDoubleEdit : public QNumberEdit
{
    Q_OBJECT

public:
    QDoubleEdit(
        QWidget* parent,
        double defaultValue,
        double min = std::numeric_limits<double>::lowest(),
        double max = std::numeric_limits<double>::max(),
        int decimals = -1
    );
};


class MinSizeFramePrivate : public QFrame, public QPlotTab::MinSizeFrame
{
    Q_OBJECT

public:
    explicit MinSizeFramePrivate(QWidget* parent);

    void setWidth(int) override;
    int width() const override;
    void setHeight(int) override;
    int height() const override;
    void setWidthToolTip(const QString&);
    void setHeightToolTip(const QString&);

private:
    QHBoxLayout* m_layout;
    QLabel* m_label_width;
    QLabel* m_label_height;
    QIntEdit* m_intEdit_width;
    QIntEdit* m_intEdit_height;
};


class RangeBoxPrivate : public QGroupBox, public QPlotTab::RangeBox
{
    Q_OBJECT

public:
    RangeBoxPrivate(QWidget* parent, const QString& title, double defaultMin, double defaultMax);

    void setMin(double) override;
    double min() const override;
    void setMax(double) override;
    double max() const override;
    void setMinToolTip(const QString&);
    void setMaxToolTip(const QString&);

private:
    QHBoxLayout* m_layout;
    QLabel* m_label_min;
    QLabel* m_label_max;
    QDoubleEdit* m_doubleEdit_min;
    QDoubleEdit* m_doubleEdit_max;
};


class LineBoxPrivate : public QGroupBox, public QPlotTab::LineBox
{
    Q_OBJECT

public:
    LineBoxPrivate(QWidget* parent, const QString& title);

    void setType(QCPGraph::LineStyle) override;
    QCPGraph::LineStyle type() const override;
    void setColor(const QColor&) override;
    QColor color() const override;
    void setStyle(Qt::PenStyle) override;
    Qt::PenStyle style() const override;
    void setTypeToolTip(const QString&);
    void setColorToolTip(const QString&);
    void setStyleToolTip(const QString&);

private Q_SLOTS:
    void colorSelected();

private:
    QFormLayout* m_layout;
    QLabel* m_label_type;
    QLabel* m_label_color;
    QLabel* m_label_style;
    QComboBox* m_comboBox_type;
    QComboBox* m_comboBox_style;
    QPushButton* m_pushButton_color;
    QColorDialog* m_colorDialog;
};


class PointsBoxPrivate : public QGroupBox, public QPlotTab::PointsBox
{
    Q_OBJECT

public:
    PointsBoxPrivate(QWidget* parent, const QString& title);

    void setShape(QCPScatterStyle::ScatterShape) override;
    QCPScatterStyle::ScatterShape shape() const override;
    void setColor(const QColor&) override;
    QColor color() const override;
    void setSize(double) override;
    double size() const override;
    void setShapeToolTip(const QString&);
    void setColorToolTip(const QString&);
    void setSizeToolTip(const QString&);

private Q_SLOTS:
    void colorSelected();

private:
    QFormLayout* m_layout;
    QLabel* m_label_shape;
    QLabel* m_label_color;
    QLabel* m_label_size;
    QComboBox* m_comboBox_shape;
    QPushButton* m_pushButton_color;
    QDoubleSpinBox* m_spinBox_size;
    QColorDialog* m_colorDialog;
};


class DataSizeBoxPrivate : public QGroupBox, public QPlotTab::DataSizeBox
{
    Q_OBJECT

public:
    DataSizeBoxPrivate(QWidget* parent, const QString& title, int defaultX, int defaultY);

    void setX(int) override;
    int x() const override;
    void setY(int) override;
    int y() const override;
    void setXToolTip(const QString&);
    void setYToolTip(const QString&);

private:
    QHBoxLayout* m_layout;
    QLabel* m_label_x;
    QLabel* m_label_y;
    QIntEdit* m_intEdit_x;
    QIntEdit* m_intEdit_y;
};


class ColorBoxPrivate : public QGroupBox, public QPlotTab::ColorBox
{
    Q_OBJECT

public:
    ColorBoxPrivate(QWidget* parent, const QString& title);

    void setMin(const QColor&) override;
    QColor min() const override;
    void setMax(const QColor&) override;
    QColor max() const override;
    void setMinToolTip(const QString&);
    void setMaxToolTip(const QString&);

private Q_SLOTS:
    void minColorSelected(const QColor&);
    void maxColorSelected(const QColor&);

private:
    QFormLayout* m_layout;
    QLabel* m_label_min;
    QLabel* m_label_max;
    QPushButton* m_pushButton_min;
    QPushButton* m_pushButton_max;
    QColorDialog* m_colorDialog_min;
    QColorDialog* m_colorDialog_max;
};


class SubTab2DPrivate : public QWidget, public QPlotTab::SubTab2D
{
    Q_OBJECT

public:
    explicit SubTab2DPrivate(QWidget* parent);

    QPlotTab::RangeBox* rangeBoxX() override;
    const QPlotTab::RangeBox* rangeBoxX() const override;
    QPlotTab::RangeBox* rangeBoxY() override;
    const QPlotTab::RangeBox* rangeBoxY() const override;
    QPlotTab::LineBox* lineBox() override;
    const QPlotTab::LineBox* lineBox() const override;
    QPlotTab::PointsBox* pointsBox() override;
    const QPlotTab::PointsBox* pointsBox() const override;

private:
    QVBoxLayout* m_layout;
    QScrollArea* m_scrollArea;
    QWidget* m_contents;
    QVBoxLayout* m_layout_contents;
    RangeBoxPrivate* m_rangeBox_x;
    RangeBoxPrivate* m_rangeBox_y;
    LineBoxPrivate* m_lineBox;
    PointsBoxPrivate* m_pointsBox;
    QSpacerItem* m_spacer;
};


class SubTabColorMapPrivate : public QWidget, public QPlotTab::SubTabColorMap
{
    Q_OBJECT

public:
    explicit SubTabColorMapPrivate(QWidget* parent);

    QPlotTab::RangeBox* rangeBoxX() override;
    const QPlotTab::RangeBox* rangeBoxX() const override;
    QPlotTab::RangeBox* rangeBoxY() override;
    const QPlotTab::RangeBox* rangeBoxY() const override;
    QPlotTab::RangeBox* rangeBoxZ() override;
    const QPlotTab::RangeBox* rangeBoxZ() const override;
    QPlotTab::DataSizeBox* dataSizeBox() override;
    const QPlotTab::DataSizeBox* dataSizeBox() const override;
    QPlotTab::ColorBox* colorBox() override;
    const QPlotTab::ColorBox* colorBox() const override;

private:
    QVBoxLayout* m_layout;
    QScrollArea* m_scrollArea;
    QWidget* m_contents;
    QVBoxLayout* m_layout_contents;
    RangeBoxPrivate* m_rangeBox_x;
    RangeBoxPrivate* m_rangeBox_y;
    RangeBoxPrivate* m_rangeBox_z;
    DataSizeBoxPrivate* m_dataSizeBox;
    ColorBoxPrivate* m_colorBox;
    QSpacerItem* m_spacer;
};
