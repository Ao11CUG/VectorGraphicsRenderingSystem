#include "Tips.h"

Tips::Tips(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/VectorGraphicsRenderingSystem/res/tips.png"));
}

Tips::~Tips()
{}
