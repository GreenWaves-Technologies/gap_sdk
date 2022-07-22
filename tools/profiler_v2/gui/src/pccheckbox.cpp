#include "pccheckbox.hpp"
#include <iostream>
#include <string>
#include <QGroupBox>
#include <QVBoxLayout>


PcCheckBox::PcCheckBox(QWidget *parent)
{
    setParent(parent);
    setTitle(QString("Hotspots Settings"));
    QVBoxLayout *vbox = new QVBoxLayout;
    fcButton = new QCheckBox(QString("fc"), this);
    vbox->addWidget(fcButton);
    fcButton->setChecked(true);
    //QObject::connect(fcButton, SIGNAL(stateChanged(int)) , , );
    for (int i=0; i<pcNb; i++){
        pcButtons[i] = new QCheckBox (QString("PE") + QString::number(i));
        pcButtons[i]->setChecked(true);
        vbox->addWidget(pcButtons[i]);
    }
    //QObject::connect(pcButtons[], SIGNAL(stateChanged(int)) , , );

    this->setLayout(vbox);

}



