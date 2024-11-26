#include "MoveDialog.h"
#include <QLabel>

MoveDialog::MoveDialog(QWidget* parent)
    : QDialog(parent) {

    xSpinBox = new QDoubleSpinBox(this);
    ySpinBox = new QDoubleSpinBox(this);
    okButton = new QPushButton("Confirm", this);
    cancelButton = new QPushButton("Cancel", this);

    QHBoxLayout* xLayout = new QHBoxLayout();
    xLayout->addWidget(new QLabel("X = X* + "));
    xLayout->addWidget(xSpinBox);

    QHBoxLayout* yLayout = new QHBoxLayout();
    yLayout->addWidget(new QLabel("Y = Y* + "));
    yLayout->addWidget(ySpinBox);


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(xLayout);
    mainLayout->addLayout(yLayout);
    mainLayout->addWidget(okButton);
    mainLayout->addWidget(cancelButton);

    setLayout(mainLayout);
    setWindowTitle("Move Shape");

    resize(300, 150);
    xSpinBox->setMaximum(100.0);
    xSpinBox->setMinimum(-100.0);
    ySpinBox->setMaximum(100.0); 
    ySpinBox->setMinimum(-100.0);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

qreal MoveDialog::getX() const {
    return xSpinBox->value();
}

qreal MoveDialog::getY() const {
    return ySpinBox->value();
}
