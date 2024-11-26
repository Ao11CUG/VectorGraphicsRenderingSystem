#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_VectorGraphicsRenderingSystem.h"
#include "Layer.h"
#include "Tips.h"


class VectorGraphicsRenderingSystem : public QMainWindow
{
    Q_OBJECT

public:
    VectorGraphicsRenderingSystem(QWidget *parent = nullptr);
    ~VectorGraphicsRenderingSystem();

public slots:
    void createLayer();
    void openFile();
    void saveFile();
    void setLineMode();
    void setPolylineMode();
    void setEllipseMode();
    void setSelectMode();
    void setMoveMode();
    void setChangeColorMode();
    void showTips();

private:
    Ui::VectorGraphicsRenderingSystemClass ui;
    Layer* layer;
};
