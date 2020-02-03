#include "tetrix.h"

Tetrix::Tetrix(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(0, 0, 800, 600);
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel("Label!"));
    setLayout(layout);
}

Tetrix::~Tetrix()
{
}

