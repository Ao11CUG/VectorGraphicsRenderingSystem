#pragma once

#include <QWidget>
#include "ui_Tips.h"

class Tips : public QWidget
{
	Q_OBJECT

public:
	Tips(QWidget *parent = nullptr);
	~Tips();

private:
	Ui::TipsClass ui;
};
