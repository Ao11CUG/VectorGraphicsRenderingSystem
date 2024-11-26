#include "Layer.h"
#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QVector2D>
#include <cmath>
#include <QInputDialog>
#include <QColorDialog>
#include <QMessageBox>

Layer::Layer(QWidget* parent)
    : QWidget(parent), drawMode(DrawMode::None), selectedShapeType(ShapeType::NoneType), drawing(false),
    lineColors({ Qt::black }), polylineColors({ Qt::black }), ellipseColors({ Qt::black }) {
    setStyleSheet("border: 1px solid black; background-color: white;");
    layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}


//添加图层
void Layer::addWidget(QWidget* widget) {
    if (widget) {
        layout->addWidget(widget);
    }
}

//清空图层
void Layer::clear() {
    //清空组件
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    lines.clear();
    polylines.clear();
    ellipses.clear();
    currentPolylinePoints.clear();
    lineLengths.clear();
    polylineLengths.clear();
    ellipseAreas.clear();
    update();
}

//设置成图片
void Layer::setImage(const QImage& img) {
    image = img;
    update(); 
}

//设置绘图模式
void Layer::setDrawMode(DrawMode mode) {
    if (drawMode == Polyline && !currentPolylinePoints.isEmpty()) {
        polylines.append(QPolygon(currentPolylinePoints));
        polylineLengths.append(calculatePolylineLength(QPolygon(currentPolylinePoints)));
        currentPolylinePoints.clear();
    }
    drawMode = mode;
    drawing = false;
    update();
}

//绘画
void Layer::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    if (!image.isNull()) {
        painter.drawImage(0, 0, image);
    }

    // Draw lines with their respective colors
    for (int i = 0; i < lines.size(); ++i) {
        if (i < lineColors.size()) {
            painter.setPen(QPen(lineColors[i], 2));
        }
        painter.drawLine(lines[i]);
    }

    // Draw polylines with their respective colors
    for (int i = 0; i < polylines.size(); ++i) {
        if (i < polylineColors.size()) {
            painter.setPen(QPen(polylineColors[i], 2));
        }
        painter.drawPolyline(polylines[i]);
    }

    // Draw ellipses with their respective colors
    for (int i = 0; i < ellipses.size(); ++i) {
        if (i < ellipseColors.size()) {
            painter.setPen(QPen(ellipseColors[i], 2));
        }
        painter.drawEllipse(ellipses[i]);
    }

    if (drawing) {
        if (drawMode == Line && !startPoint.isNull() && !endPoint.isNull()) {
            painter.setPen(QPen(lineColors.last(), 2)); // Use last color if drawing a new line
            painter.drawLine(startPoint, endPoint);
        }
        else if (drawMode == Polyline && !currentPolylinePoints.isEmpty()) {
            painter.setPen(QPen(polylineColors.last(), 2)); // Use last color if drawing a new polyline
            for (int i = 0; i < currentPolylinePoints.size() - 1; ++i) {
                painter.drawLine(currentPolylinePoints[i], currentPolylinePoints[i + 1]);
            }
            painter.drawLine(currentPolylinePoints.last(), endPoint);
        }
        else if (drawMode == Ellipse && !startPoint.isNull() && !endPoint.isNull()) {
            painter.setPen(QPen(ellipseColors.last(), 2)); // Use last color if drawing a new ellipse
            QRect rect(startPoint, endPoint);
            painter.drawEllipse(rect);
        }
    }
}

//鼠标移动
void Layer::mouseMoveEvent(QMouseEvent* event) {
    if (drawing && drawMode == Polyline) {
        endPoint = event->pos();
        update();
    }
}

//鼠标按压
void Layer::mousePressEvent(QMouseEvent* event) {
    //选择模式
    if (drawMode == Select) {
        if (event->button() == Qt::RightButton) {
            QPoint clickPos = event->pos();
            const qreal tolerance = 5.0;

            ///清除选中状态
            selectedShapeType = NoneType;
            selectedShapeIndex = -1;

            //遍历已有的线
            for (int i = 0; i < lines.size(); ++i) {
                if (isPointNearLine(clickPos, lines[i], tolerance)) {
                    selectedShapeType = LineType;
                    selectedShapeIndex = i;
                    showShapeProperties("Line", lineLengths[i]);
                    return;
                }
            }

            //遍历已有的折线
            for (int i = 0; i < polylines.size(); ++i) {
                if (isPointInPolygon(clickPos, polylines[i])) {
                    selectedShapeType = PolylineType;
                    selectedShapeIndex = i;
                    showShapeProperties("Polyline", polylineLengths[i]);
                    return;
                }
            }

            //遍历已有的椭圆
            for (int i = 0; i < ellipses.size(); ++i) {
                if (ellipses[i].contains(clickPos)) {
                    selectedShapeType = EllipseType;
                    selectedShapeIndex = i;
                    showShapeProperties("Ellipse", ellipseAreas[i]);
                    return;
                }
            }

            QMessageBox::information(this, "No Shape Selected", "No shape found at the selected position.");
        }
    }
    //平移模式
    else if (drawMode == Move) {
        if (event->button() == Qt::RightButton) {
            QPoint clickPos = event->pos();
            const qreal tolerance = 5.0;
            bool shapeFound = false;

            //遍历已有的线
            for (int i = 0; i < lines.size(); ++i) {
                if (isPointNearLine(clickPos, lines[i], tolerance)) {
                    shapeFound = true;
                    selectedShapeType = LineType;
                    selectedShapeIndex = i;
                    selectedPoint = clickPos;
                    break;
                }
            }
            //遍历已有的折线
            for (int i = 0; i < polylines.size(); ++i) {
                if (isPointInPolygon(clickPos, polylines[i])) {
                    shapeFound = true;
                    selectedShapeType = PolylineType;
                    selectedShapeIndex = i;
                    selectedPoint = clickPos;
                    break;
                }
            }
            //遍历已有的椭圆
            for (int i = 0; i < ellipses.size(); ++i) {
                if (ellipses[i].contains(clickPos)) {
                    shapeFound = true;
                    selectedShapeType = EllipseType;
                    selectedShapeIndex = i;
                    selectedPoint = clickPos;
                    break;
                }
            }
            //如果图形被找到，则跳转可交互界面
            if (shapeFound) {
                MoveDialog dialog(this);
                if (dialog.exec() == QDialog::Accepted) {
                    qreal dx = dialog.getX();
                    qreal dy = dialog.getY();
                    moveSelectedShape(QPoint(dx, dy));
                }
            }
            else {
                QMessageBox::information(this, "No Shape Selected", "No shape found at the selected position.");
            }
        }
    }
    //改色模式
    else if (drawMode == ChangeColor) {
        if (event->button() == Qt::RightButton) {
            QPoint clickPos = event->pos();
            const qreal tolerance = 5.0;
            bool shapeFound = false;

            //遍历已有的线
            for (int i = 0; i < lines.size(); ++i) {
                if (isPointNearLine(clickPos, lines[i], tolerance)) {
                    shapeFound = true;
                    selectedShapeType = LineType;
                    selectedShapeIndex = i;
                    selectedPoint = clickPos;
                    break;
                }
            }
            //遍历已有的折线
            for (int i = 0; i < polylines.size(); ++i) {
                if (isPointInPolygon(clickPos, polylines[i])) {
                    shapeFound = true;
                    selectedShapeType = PolylineType;
                    selectedShapeIndex = i;
                    selectedPoint = clickPos;
                    break;
                }
            }
            //遍历已有的椭圆
            for (int i = 0; i < ellipses.size(); ++i) {
                if (ellipses[i].contains(clickPos)) {
                    shapeFound = true;
                    selectedShapeType = EllipseType;
                    selectedShapeIndex = i;
                    selectedPoint = clickPos;
                    break;
                }
            }
            //如果图形被找到，执行改色操作
            if (shapeFound) {
                QColor newColor = QColorDialog::getColor(Qt::black, this, "Select Color");
                if (newColor.isValid()) {
                    changeShapeColor(newColor);
                }
                }
            }
    }

    else if (drawMode != None) {
        if (drawMode == Polyline) {
            if (currentPolylinePoints.isEmpty()) {
                currentPolylinePoints.append(event->pos());
                drawing = true;
            }
            else {
                currentPolylinePoints.append(event->pos());
                update();
            }
        }
        else {
            startPoint = event->pos();
            endPoint = startPoint;
            drawing = true;
        }
    }
}

//鼠标释放
void Layer::mouseReleaseEvent(QMouseEvent* event) {
    //与折线无关
    if (drawMode != None && drawMode != Polyline) {
        endPoint = event->pos();
        //线的话则根据起点终点添加新的线，并计算距离
        if (drawMode == Line) {
            lines.append(QLine(startPoint, endPoint));
            lineLengths.append(calculateLineLength(QLine(startPoint, endPoint)));
        }
        //椭圆的话就根据起点终点添加新的椭圆，并计算距离
        else if (drawMode == Ellipse) {
            QRect rect(startPoint, endPoint);
            ellipses.append(rect);
            ellipseAreas.append(calculateEllipseArea(rect));
        }
        drawing = false;
        update();
    }
}

//展示属性
void Layer::showShapeProperties(const QString& shapeType, qreal property) {
    QString propertyString;
    if (shapeType == "Line" || shapeType == "Polyline") {
        propertyString = QString("Length: %1").arg(property);
    }
    else if (shapeType == "Ellipse") {
        propertyString = QString("Area: %1").arg(property);
    }

    QMessageBox::information(this, "Shape Properties", QString("Type: %1\n%2").arg(shapeType, propertyString));
}

//以下是计算长度和面积的算法
//计算线长
qreal Layer::calculateLineLength(const QLine& line) const {
    return QLineF(line).length();
}

//计算折线长
qreal Layer::calculatePolylineLength(const QPolygon& polyline) const {
    qreal length = 0.0;
    for (int i = 0; i < polyline.size() - 1; ++i) {
        length += QLineF(polyline[i], polyline[i + 1]).length();
    }
    return length;
}

//计算椭圆面积
qreal Layer::calculateEllipseArea(const QRect& rect) const {
    qreal a = std::abs(rect.width()) / 2.0; // 使用绝对值
    qreal b = std::abs(rect.height()) / 2.0; // 使用绝对值
    return M_PI * a * b;
}

//判断函数
//判断鼠标点击处与线、折线的位置关系
bool Layer::isPointNearLine(const QPoint& point, const QLine& line, qreal tolerance) const {
    QLineF lineF(line);
    qreal distance = QLineF(lineF.p1(), point).length();
    if (distance < tolerance) return true;

    distance = QLineF(lineF.p2(), point).length();
    if (distance < tolerance) return true;

    qreal numerator = std::abs((lineF.dy() * point.x()) - (lineF.dx() * point.y()) + (lineF.p2().x() * lineF.p1().y()) - (lineF.p2().y() * lineF.p1().x()));
    qreal denominator = lineF.length();
    distance = numerator / denominator;

    return (distance <= tolerance && point.x() >= std::min(lineF.p1().x(), lineF.p2().x()) &&
        point.x() <= std::max(lineF.p1().x(), lineF.p2().x()) &&
        point.y() >= std::min(lineF.p1().y(), lineF.p2().y()) &&
        point.y() <= std::max(lineF.p1().y(), lineF.p2().y()));
}

bool Layer::isPointInPolygon(const QPoint& point, const QPolygon& polygon) const {
    return polygon.containsPoint(point, Qt::OddEvenFill);
}

//平移
void Layer::moveSelectedShape(QPoint translationVector) {
    //根据选中的不同的图形类型，有不同的平移算法
    //直接对线段的端点平移
    if (selectedShapeType == LineType && selectedShapeIndex >= 0 && selectedShapeIndex < lines.size()) {
        QLine newLine(QPoint(lines[selectedShapeIndex].x1() + translationVector.x(), lines[selectedShapeIndex].y1() + translationVector.y()),
            QPoint(lines[selectedShapeIndex].x2() + translationVector.x(), lines[selectedShapeIndex].y2() + translationVector.y()));
        lines[selectedShapeIndex] = newLine;
    }

    //对折线的全部点进行平移
    if (selectedShapeType == PolylineType && selectedShapeIndex >= 0 && selectedShapeIndex < polylines.size()) {
        QPolygon newPolygon = polylines[selectedShapeIndex];
        for (int j = 0; j < newPolygon.size(); ++j) {
            newPolygon[j] += translationVector;
        }
        polylines[selectedShapeIndex] = newPolygon;
    }

    //对椭圆的topLeft进行平移
    if (selectedShapeType == EllipseType && selectedShapeIndex >= 0 && selectedShapeIndex < ellipses.size()) {
        QRect newRect = ellipses[selectedShapeIndex];
        newRect.moveTopLeft(newRect.topLeft() + translationVector);
        ellipses[selectedShapeIndex] = newRect;
    }

    update();
}

//改色
void Layer::changeShapeColor(const QColor& newColor) {
    if (selectedShapeType == NoneType) {
        QMessageBox::information(this, "No Shape Selected", "Please right-click on a shape first.");
        return;
    }

    //根据图形类型细分
    //根据图形的索引来实现对图形单独的操作
    switch (selectedShapeType) {
    case LineType:
        if (selectedShapeIndex >= 0 && selectedShapeIndex < lines.size()) {  
                lineColors[selectedShapeIndex] = newColor;
        }
        break;

    case PolylineType:
        if (selectedShapeIndex >= 0 && selectedShapeIndex < polylines.size()) {
                polylineColors[selectedShapeIndex] = newColor;
        }
        break;

    case EllipseType:
        if (selectedShapeIndex >= 0 && selectedShapeIndex < ellipses.size()) {
                ellipseColors[selectedShapeIndex] = newColor;
        }
        break;

    default:
        QMessageBox::information(this, "Shape Not Found", "No shape found at the selected position.");
        return;
    }

    selectedShapeType = NoneType;
    selectedShapeIndex = -1;

    update();
}
