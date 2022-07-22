#ifndef CODEEDITORWRAP_HPP
#define CODEEDITORWRAP_HPP

#include <QWidget>
#include "codeeditor.hpp"
#include <QRadioButton>
#include <QGroupBox>
#include <QString>


class CodeEditorWrap : public QWidget
{
    Q_OBJECT

public:
    explicit CodeEditorWrap(QWidget *parent = nullptr);
    CodeEditor* sourceCode;
    QRadioButton *radioPercent;
    QRadioButton *radioCall;
    QRadioButton *radioDuration;

private:
    QGroupBox *createFirstExclusiveGroup();
    CodeEditor* createCodeEditor();
    QWidget* titleBar;
};

#endif // CODEEDITORWRAP_HPP
