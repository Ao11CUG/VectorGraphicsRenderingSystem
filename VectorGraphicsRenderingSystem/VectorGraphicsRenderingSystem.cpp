#include "VectorGraphicsRenderingSystem.h"
#include "ui_VectorGraphicsRenderingSystem.h"
#include "Layer.h"
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QLabel>

VectorGraphicsRenderingSystem::VectorGraphicsRenderingSystem(QWidget* parent)
    : QMainWindow(parent), layer(nullptr) {
    ui.setupUi(this);

    setWindowIcon(QIcon(":/VectorGraphicsRenderingSystem/res/draw.png"));

    ui.scrollArea = new QScrollArea(this);
    ui.scrollArea->setWidgetResizable(true);

    QWidget* containerWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(containerWidget);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    containerWidget->setLayout(layout);
    ui.scrollArea->setWidget(containerWidget);

    setCentralWidget(ui.scrollArea);

    connect(ui.createFile, &QAction::triggered, this, &VectorGraphicsRenderingSystem::createLayer);
    connect(ui.openFile, &QAction::triggered, this, &VectorGraphicsRenderingSystem::openFile);
    connect(ui.saveFile, &QAction::triggered, this, &VectorGraphicsRenderingSystem::saveFile);

    connect(ui.line, &QAction::triggered, this, &VectorGraphicsRenderingSystem::setLineMode);
    connect(ui.polyline, &QAction::triggered, this, &VectorGraphicsRenderingSystem::setPolylineMode);
    connect(ui.ellipse, &QAction::triggered, this, &VectorGraphicsRenderingSystem::setEllipseMode);

    connect(ui.choose, &QAction::triggered, this, &VectorGraphicsRenderingSystem::setSelectMode);
    connect(ui.move, &QAction::triggered, this, &VectorGraphicsRenderingSystem::setMoveMode);
    connect(ui.changeColor, &QAction::triggered, this, &VectorGraphicsRenderingSystem::setChangeColorMode);

    connect(ui.Tips, & QAction::triggered, this, &VectorGraphicsRenderingSystem::showTips);
}

VectorGraphicsRenderingSystem::~VectorGraphicsRenderingSystem() {
    delete layer;
}

//�½�ͼ��
void VectorGraphicsRenderingSystem::createLayer() {
    if (layer) {
        delete layer;
        layer = nullptr;
    }

    layer = new Layer(this);

    QWidget* containerWidget = ui.scrollArea->widget();
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(containerWidget->layout());

    if (layout) {
        layout->addWidget(layer);

        layer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
}

//���ļ�
void VectorGraphicsRenderingSystem::openFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open Image File", "", "Images (*.png *.jpg)");

    if (filePath.isEmpty()) {
        return;
    }

    QImage image(filePath);
    if (image.isNull()) {
        QMessageBox::critical(this, "Error", "Failed to load image.");
        return;
    }

    if (!layer) {
        createLayer();
    }

    layer->clear();

    layer->setImage(image);
}

//�����ļ�
void VectorGraphicsRenderingSystem::saveFile() {
    if (!layer) {
        QMessageBox::warning(this, "Save Error", "No layer to save.");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, "Save Image File", "", "Images (*.png *.jpg)");
    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(".png") && !filePath.endsWith(".jpg")) {
        filePath.append(".png");
    }

    QPixmap pixmap(layer->size());
    layer->render(&pixmap);

    QImage image = pixmap.toImage();

    if (!image.save(filePath)) {
        QMessageBox::critical(this, "Error", "Failed to save image.");
    }
}

//������ת������ģʽ
//����
void VectorGraphicsRenderingSystem::setLineMode() {
    if (layer) {
        layer->setDrawMode(Layer::Line);
    }
}

//������
void VectorGraphicsRenderingSystem::setPolylineMode() {
    if (layer) {
        layer->setDrawMode(Layer::Polyline);
    }
}

//����Բ
void VectorGraphicsRenderingSystem::setEllipseMode() {
    if (layer) {
        layer->setDrawMode(Layer::Ellipse);
    }
}

//ѡ��
void VectorGraphicsRenderingSystem::setSelectMode() {
    if (layer) {
        layer->setDrawMode(Layer::Select);
    }
}

//ƽ��
void VectorGraphicsRenderingSystem::setMoveMode() {
    if (layer) {
        layer->setDrawMode(Layer::Move);
    }
}

//��ɫ
void VectorGraphicsRenderingSystem::setChangeColorMode() {
    if (layer) {
        layer->setDrawMode(Layer::ChangeColor);
    }
}

//��ת��ʾҳ��
void VectorGraphicsRenderingSystem::showTips() {
    Tips* tips = new Tips();
    tips->show();
}



