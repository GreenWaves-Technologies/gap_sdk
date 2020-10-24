#include "prolabel.hpp"

ProLabel::ProLabel(const QString & text, QWidget * parent, Qt::WindowFlags f)
    :QLabel(text, parent, f)
{
}

ProLabel::~ProLabel()
{
}

void ProLabel::mousePressEvent(QMouseEvent*)
{
    emit clicked();
}