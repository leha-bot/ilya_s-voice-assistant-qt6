#include "sliderbutton.h"

SliderButton::SliderButton(const QString &offParameter, const QString &onParameter, QWidget *parent) : QWidget(parent)
{
    stateSlider = false;
    label1 = offParameter;
    label2 = onParameter;
    btn1 = new QPushButton(offParameter, this);
    btn1->setStyleSheet("text-align: left;");
    connect(btn1, &QPushButton::clicked, this, &SliderButton::changeStateSlider);
    btn1->setObjectName("buttonPartSlider1");
    btn1->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    width1 = label1.size()*10;
    height = btn1->height();

    btn2 = new QPushButton(onParameter, this);
    btn2->setObjectName("buttonPartSlider2");
    btn2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    width2 = label2.size()*10;

    btn1->setGeometry(0, 0, width1+width2-10, height+2);
    btn2->setGeometry(width1-10, 1, width2, height);

    setMinimumSize(width1+width2-5, height+2);
    setMaximumSize(width1+width2, height+2);
}

SliderButton::~SliderButton()
{
    delete btn1;
    delete btn2;
}

void SliderButton::changeStateSlider()
{
    stateSlider = !stateSlider;
    if (stateSlider) {
        btn2->setGeometry(0, 1, width1, height);
        btn1->setGeometry(0, 0, width1+width2-10, height+2);
        btn1->setText(label2);
        btn1->setStyleSheet("text-align: right;");
        btn2->setText(label1);
        emit currentState(false);
    }
    else {
        btn1->setGeometry(0, 1, width1+width2-10, height+2);
        btn2->setGeometry(width1-10, 1, width2, height);
        btn1->setText(label1);
        btn1->setStyleSheet("text-align: left;");
        btn2->setText(label2);
        emit currentState(true);
    }
}
