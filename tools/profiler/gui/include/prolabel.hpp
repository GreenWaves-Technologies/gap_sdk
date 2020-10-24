#ifndef PROLABEL_HPP
#define PROLABEL_HPP

#include <QLabel>

// This class is made to create label that is clickable
// All that's needed is to connect the clicked() signal of the label
// to some slot. 
class ProLabel : public QLabel 
{
   Q_OBJECT
public:
   ProLabel(const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0);
   ~ProLabel();
signals:
   void clicked();
protected:
   void mousePressEvent(QMouseEvent*);
};

#endif /* PROLABEL_HPP */
