#ifndef LAYER_H
#define LAYER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLine>
#include <QPolygon>
#include <QRect>
#include <QMouseEvent>
#include <QMessageBox>
#include "MoveDialog.h"
#include <QColor>
#include <QColorDialog>

class Layer : public QWidget {
    Q_OBJECT

public:
    enum DrawMode { 
        None, Line, Polyline, Ellipse, Select, Move, ChangeColor };

    enum ShapeType {
        NoneType,
        LineType,
        PolylineType,
        EllipseType
    };

    explicit Layer(QWidget* parent = nullptr);
    void addWidget(QWidget* widget);
    void clear();
    void setDrawMode(DrawMode mode);
    void setImage(const QImage& img); 

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QImage image;
    ShapeType selectedShapeType; 
    int selectedShapeIndex;

    DrawMode drawMode;
    bool drawing;
    QPoint startPoint;
    QPoint endPoint;
    QVBoxLayout* layout;
    QVector<QLine> lines;
    QVector<QPolygon> polylines;
    QVector<QRect> ellipses;
    QVector<QPoint> currentPolylinePoints;//用来存储绘制的图形

    QVector<qreal> lineLengths;
    QVector<qreal> polylineLengths;
    QVector<qreal> ellipseAreas;//存储绘制图形的长度、面积属性

    QPoint selectedPoint;
    DrawMode moveMode = None;

    void moveSelectedShape(QPoint translationVector);

    void showShapeProperties(const QString& shapeType, qreal property);

    qreal calculateLineLength(const QLine& line) const;
    qreal calculatePolylineLength(const QPolygon& polyline) const;
    qreal calculateEllipseArea(const QRect& rect) const;

    bool isPointNearLine(const QPoint& point, const QLine& line, qreal tolerance) const;
    bool isPointInPolygon(const QPoint& point, const QPolygon& polygon) const;

    QVector<QColor> lineColors;
    QVector<QColor> polylineColors;
    QVector<QColor> ellipseColors;//存储图形的颜色

    void changeShapeColor(const QColor& newColor);
};

#endif 
