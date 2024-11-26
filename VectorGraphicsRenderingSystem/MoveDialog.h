#ifndef MOVEDIALOG_H
#define MOVEDIALOG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class MoveDialog : public QDialog {
    Q_OBJECT

public:
    MoveDialog(QWidget* parent = nullptr);

    qreal getX() const;
    qreal getY() const;

private:
    QDoubleSpinBox* xSpinBox;
    QDoubleSpinBox* ySpinBox;
    QPushButton* okButton;
    QPushButton* cancelButton;
};

#endif // MOVEDIALOG_H
