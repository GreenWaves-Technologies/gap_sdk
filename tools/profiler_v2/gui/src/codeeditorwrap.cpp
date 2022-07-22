#include "codeeditorwrap.hpp"

#include <iostream>
#include <string>
#include <QVBoxLayout>
#include <QLabel>

CodeEditorWrap::CodeEditorWrap(QWidget *parent)
{
    setParent(parent);
    QVBoxLayout *vbox = new QVBoxLayout;
    sourceCode = new CodeEditor(this);
    sourceCode->setWordWrapMode(QTextOption::NoWrap);
    sourceCode->setReadOnly(false);
    vbox->addWidget(createFirstExclusiveGroup());
    vbox->addWidget(sourceCode);
    setLayout(vbox);

    //setWindowTitle(tr("Group Boxes"));

}

QGroupBox *CodeEditorWrap::createFirstExclusiveGroup()
{
    QGroupBox *groupBox = new QGroupBox(tr(" "));

    radioPercent = new QRadioButton(tr("&Time Percentage"));
    radioCall = new QRadioButton(tr("&#calls"));
    radioDuration = new QRadioButton(tr("&Total Time(s)"));

    radioCall->setChecked(true);

    QObject::connect(radioPercent, SIGNAL(clicked(bool)), sourceCode,SLOT(radioPercentClicked(bool)));
    QObject::connect(radioCall, SIGNAL(clicked(bool)), sourceCode,SLOT(radioCallClicked(bool)));
    QObject::connect(radioDuration, SIGNAL(clicked(bool)), sourceCode,SLOT(radioDurationClicked(bool)));

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(radioPercent);
    hbox->addWidget(radioCall);
    hbox->addWidget(radioDuration);
    hbox->addStretch(1);
    groupBox->setLayout(hbox);
    return groupBox;
}
