#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QColorDialog>
#include <QFrame>

#include "qcustomplot.h"
#include "qplot.hpp"


class MinSizeFramePrivate;
class SubTab2DPrivate;
class SubTabColorMapPrivate;

class QPlotTab : public QWidget
{
    Q_OBJECT

public:
    class MinSizeFrame
    {
    public:
        typedef struct
        {
            int width;
            int height;
        } Cache;
        typedef struct
        {
            QString minSize;
            QString width;
            QString height;
        } ToolTips;
        void setCache(const Cache&);
        Cache cache() const;
        virtual void setWidth(int) = 0;
        virtual int width() const = 0;
        virtual void setHeight(int) = 0;
        virtual int height() const = 0;
    };

    class RangeBox
    {
    public:
        typedef struct
        {
            double min;
            double max;
        } Cache;
        typedef struct
        {
            QString min;
            QString max;
        } ToolTips;
        void setCache(const Cache&);
        Cache cache() const;
        virtual void setMin(double) = 0;
        virtual double min() const = 0;
        virtual void setMax(double) = 0;
        virtual double max() const = 0;
    };

    class LineBox
    {
    public:
        typedef struct
        {
            QCPGraph::LineStyle type;
            QColor color;
            Qt::PenStyle style;
        } Cache;
        typedef struct
        {
            QString type;
            QString color;
            QString style;
        } ToolTips;
        void setCache(const Cache&);
        Cache cache() const;
        virtual void setType(QCPGraph::LineStyle) = 0;
        virtual QCPGraph::LineStyle type() const = 0;
        virtual void setColor(const QColor&) = 0;
        virtual QColor color() const = 0;
        virtual void setStyle(Qt::PenStyle) = 0;
        virtual Qt::PenStyle style() const = 0;
    };

    class PointsBox
    {
    public:
        typedef struct
        {
            QCPScatterStyle::ScatterShape shape;
            QColor color;
            double size;
        } Cache;
        typedef struct
        {
            QString shape;
            QString color;
            QString size;
        } ToolTips;
        void setCache(const Cache&);
        Cache cache() const;
        virtual void setShape(QCPScatterStyle::ScatterShape) = 0;
        virtual QCPScatterStyle::ScatterShape shape() const = 0;
        virtual void setColor(const QColor&) = 0;
        virtual QColor color() const = 0;
        virtual void setSize(double) = 0;
        virtual double size() const = 0;
    };

    class DataSizeBox
    {
    public:
        typedef struct
        {
            int x;
            int y;
        } Cache;
        typedef struct
        {
            QString x;
            QString y;
        } ToolTips;
        void setCache(const Cache&);
        Cache cache() const;
        virtual void setX(int) = 0;
        virtual int x() const = 0;
        virtual void setY(int) = 0;
        virtual int y() const = 0;
    };

    class ColorBox
    {
    public:
        typedef struct
        {
            QColor min;
            QColor max;
        } Cache;
        typedef struct
        {
            QString min;
            QString max;
        } ToolTips;
        void setCache(const Cache&);
        Cache cache() const;
        virtual void setMin(const QColor&) = 0;
        virtual QColor min() const = 0;
        virtual void setMax(const QColor&) = 0;
        virtual QColor max() const = 0;
    };

    class SubTab2D
    {
    public:
        typedef struct
        {
            RangeBox::Cache xRange;
            RangeBox::Cache yRange;
            LineBox::Cache line;
            PointsBox::Cache points;
        } Cache;
        typedef struct
        {
            RangeBox::ToolTips xRange;
            RangeBox::ToolTips yRange;
            LineBox::ToolTips line;
            PointsBox::ToolTips points;
        } ToolTips;
        void setCache(const Cache&);
        Cache cache() const;
        virtual RangeBox* rangeBoxX() = 0;
        virtual const RangeBox* rangeBoxX() const = 0;
        virtual RangeBox* rangeBoxY() = 0;
        virtual const RangeBox* rangeBoxY() const = 0;
        virtual LineBox* lineBox() = 0;
        virtual const LineBox* lineBox() const = 0;
        virtual PointsBox* pointsBox() = 0;
        virtual const PointsBox* pointsBox() const = 0;
    };

    class SubTabColorMap
    {
    public:
        typedef struct
        {
            RangeBox::Cache xRange;
            RangeBox::Cache yRange;
            RangeBox::Cache zRange;
            DataSizeBox::Cache dataSize;
            ColorBox::Cache color;
        } Cache;
        typedef struct
        {
            RangeBox::ToolTips xRange;
            RangeBox::ToolTips yRange;
            RangeBox::ToolTips zRange;
            DataSizeBox::ToolTips dataSize;
            ColorBox::ToolTips color;
        } ToolTips;
        void setCache(const Cache&);
        Cache cache() const;
        virtual RangeBox* rangeBoxX() = 0;
        virtual const RangeBox* rangeBoxX() const = 0;
        virtual RangeBox* rangeBoxY() = 0;
        virtual const RangeBox* rangeBoxY() const = 0;
        virtual RangeBox* rangeBoxZ() = 0;
        virtual const RangeBox* rangeBoxZ() const = 0;
        virtual DataSizeBox* dataSizeBox() = 0;
        virtual const DataSizeBox* dataSizeBox() const = 0;
        virtual ColorBox* colorBox() = 0;
        virtual const ColorBox* colorBox() const = 0;
    };

    typedef struct
    {
        QString title;
        QString xAxis;
        QString yAxis;
        MinSizeFrame::Cache minSize;
        SubTab2D::Cache twoDimen;
        SubTabColorMap::Cache colorMap;
    } Cache;

    static struct
    {
        QString title;
        QString xAxis;
        QString yAxis;
        MinSizeFrame::ToolTips minSize;
        SubTab2D::ToolTips twoDimen;
        SubTabColorMap::ToolTips colorMap;
    } toolTips;

    explicit QPlotTab(QWidget* parent = Q_NULLPTR);
    QPlotTab(const QPlotTab&) = delete;

    void setCache(const Cache&);
    Cache cache() const;
    void setTitle(const QString&);
    QString title() const;
    void setXAxis(const QString&);
    QString xAxis() const;
    void setYAxis(const QString&);
    QString yAxis() const;
    MinSizeFrame* minSize();
    const MinSizeFrame* minSize() const;
    SubTab2D* tab2D();
    const SubTab2D* tab2D() const;
    SubTabColorMap* tabColorMap();
    const SubTabColorMap* tabColorMap() const;
    QPlot::Type selected() const;
    void setTitleToolTip(const QString&);
    void setXAxisToolTip(const QString&);
    void setYAxisToolTip(const QString&);
    void setMinSizeToolTip(const QString&);

private:
    QVBoxLayout* m_layout;
    QFrame* m_frame_common;
    QTabWidget* m_tabWidget;
    QFormLayout* m_layout_common;
    QLabel* m_label_title;
    QLabel* m_label_xAxis;
    QLabel* m_label_yAxis;
    QLabel* m_label_minSize;
    QLineEdit* m_lineEdit_title;
    QLineEdit* m_lineEdit_xAxis;
    QLineEdit* m_lineEdit_yAxis;
    MinSizeFramePrivate* m_minSizeFrame;
    SubTab2DPrivate* m_tab2D;
    SubTabColorMapPrivate* m_tabColorMap;
};
