/*
 * ZetaPlot
 * QButtonGrid widget plugin
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include <QDesignerCustomWidgetInterface>


class QButtonGridPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface")
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    explicit QButtonGridPlugin(QObject* parent = Q_NULLPTR);

    QString name() const override;
    QString group() const override;
    QString toolTip() const override;
    QString whatsThis() const override;
    QString includeFile() const override;
    QIcon icon() const override;
    bool isContainer() const override;
    QWidget* createWidget(QWidget* parent) override;
    bool isInitialized() const override;
    void initialize(QDesignerFormEditorInterface* core) override;
    QString domXml() const override;
};
