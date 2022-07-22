#include "codeeditor.hpp"
#include "linenumberarea.hpp"

#include <QWidget>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QPainter>
#include <QFontMetrics>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <QGroupBox>
#include <QRadioButton>
#include <QTextDocument>



CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{   // In the constructor we connect our slots to signals in QPlainTextEdit. It is necessary to calculate
    // the line number area width and highlight the first line when the editor is created.

    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth()
{
    // The lineNumberAreaWidth() function calculates the width of the LineNumberArea widget. We take the number
    // of digits in the last line of the editor and multiply that with the maximum width of a digit.
    digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    lineNumberWidth = fontMetrics().width(QLatin1Char('9')) * (digits);
    //int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits; // Qt 5.15
    int space = 3 + lineNumberWidth + hotspotInfoWidth; // Qt 5.6

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    // When we update the width of the line number area, we simply call
    // QAbstractScrollArea::setViewportMargins().
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}


void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{   // This slot is invoked when the editors viewport has been scrolled. The QRect given as argument
    // is the part of the editing area that is to be updated (redrawn). dy holds the number of pixels
    // the view has been scrolled vertically.
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{   // When the size of the editor changes, we also need to resize the line number area.
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth() , cr.height()));
}

void CodeEditor::highlightCurrentHotspotLine(int hotspotLine){

    QBrush backBrush( Qt::yellow );
    QBrush textBrush( Qt::black );
    QPen outlinePen( Qt::gray, 1 );

    QTextBlock block = this->document()->findBlockByLineNumber(hotspotLine -1 > 1? hotspotLine-1:1);
    int blockPos = block.position();
    QString text = block.text();

    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    selection.cursor = QTextCursor(document());
    selection.cursor.setPosition(blockPos);
    selection.cursor.setPosition( blockPos + text.length(), QTextCursor::KeepAnchor );
    selection.format.setBackground( backBrush );
    selection.format.setForeground( textBrush );
    selection.format.setProperty( QTextFormat::OutlinePen, outlinePen );

    extraSelections.append( selection );

    setExtraSelections( extraSelections );

}


void CodeEditor::highlightCurrentLine()
{
    // When the cursor position changes, we highlight the current line, i.e.,
    // the line containing the cursor.

    // QPlainTextEdit gives the possibility to have more than one selection at the same time.
    // we can set the character format (QTextCharFormat) of these selections. We clear the cursors
    // selection before setting the new new QPlainTextEdit::ExtraSelection, else several lines would
    // get highlighted when the user selects multiple lines with the mouse.

    // One sets the selection with a text cursor. When using the FullWidthSelection property,
    // the current cursor text block (line) will be selected. If you want to select just a portion
    // of the text block, the cursor should be moved with QTextCursor::movePosition() from a position
    // set with setPosition().


    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{   // The lineNumberAreaPaintEvent() is called from LineNumberArea whenever
    // it receives a paint event. We start off by painting the widget's background.
    QPainter painter(lineNumberArea);
    QRect rect = event->rect();
    int x1,y1,x2,y2;
    rect.getCoords(&x1,&y1,&x2,&y2);
    QRect rect1(x1,y1,x1 + lineNumberWidth -1,y2 ) ;
    QRect rect2(x1+ lineNumberWidth,y1,x2,y2);
    //painter.fillRect(event->rect(), Qt::lightGray);
    //painter.fillRect(rect1,Qt::lightGray);
    painter.fillRect(rect2,Qt::lightGray);

    // We will now loop through all visible lines and paint the line numbers in the extra
    // area for each line. Notice that in a plain text edit each line will consist of one
    // TextBlock; though, if line wrapping is enabled, a line may span several rows in the
    // text edit's viewport.

    // We get the top and bottom y-coordinate of the first text block, and adjust these values
    // by the height of the current text block in each iteration in the loop.
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    // Notice that we check if the block is visible in addition to check if it is in the
    // areas viewport - a block can, for example, be hidden by a window placed over the text edit.

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberWidth, fontMetrics().height(),
                             Qt::AlignRight, number);
            // fill the second column with hotspots line info
            for (SafeHotspotLineInfo x : hotspots) {
                QString* info;
                if (x.line == blockNumber + 1) {
                    if (callChecked) {
                        info= new QString(QString::number(x.count));
                    }
                    else if (durationChecked) {
                        std::stringstream ss1;
                        ss1  <<  std::fixed << std::setprecision(6) << 1e-12 * x.duration;
                        info = new QString(QString::fromStdString(ss1.str()) + QString("s"));
                    }
                    else if (percentChecked){
                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(4) << x.percentage;
                        info = new QString(QString::fromStdString(ss.str()) + QString("%"));
                    }
                    else
                        info = new QString(QString(" "));

                    painter.drawText(0 + lineNumberWidth, top, hotspotInfoWidth, fontMetrics().height(),
                                     Qt::AlignRight,*info);
                    delete info;
                }
            }


        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::setHotspotsList(const std::list<SafeHotspotLineInfo> &fileHotspots) {

    hotspots.clear();
    hotspots.assign(fileHotspots.begin(),fileHotspots.end());
}


void CodeEditor::setButtonsStatus(  const bool percentChecked,
                                    const bool callChecked,
                                    const bool durationChecked){
    this->percentChecked = percentChecked;
    this->callChecked = callChecked;
    this->durationChecked = durationChecked;
}

void CodeEditor::radioPercentClicked(bool checked){
    percentChecked = checked;
    callChecked = false;
    durationChecked = false;
    lineNumberArea->update();
    this->viewport()->repaint();
}

void CodeEditor::radioCallClicked(bool checked){
    callChecked = checked;
    percentChecked = false;
    durationChecked = false;
    lineNumberArea->update();
    this->viewport()->repaint();
}

void CodeEditor::radioDurationClicked(bool checked){
    durationChecked = checked;
    callChecked = false;
    percentChecked = false;
    lineNumberArea->update();
    this->viewport()->repaint();
}

