#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP

#include <QWidget>
#include <QPlainTextEdit>
#include "profiler_backend_user.hpp"
#include "safehotspot.hpp"
#include "safehotspotfunctioninfo.hpp"
#include "safehotspotfileinfo.hpp"
#include "safehotspotlineinfo.hpp"

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void setHotspotsList(const std::list<SafeHotspotLineInfo> &fileHotspots);
    void setButtonsStatus(const bool percentChecked,
                          const bool callChecked,
                          const bool durationChecked);
    void highlightCurrentHotspotLine(int hotspotLine);
protected:
    void resizeEvent(QResizeEvent *event) override;
public slots:
    void radioPercentClicked(bool checked);
    void radioCallClicked(bool checked);
    void radioDurationClicked(bool checked);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);


private:
    QWidget *lineNumberArea;
    const int hotspotInfoWidth = 10 * fontMetrics().width(QLatin1Char('9'));
    int digits; // max number of digits of a line number
    int lineNumberWidth; // nb of pixels of the space left for line numbers
    std::list<SafeHotspotLineInfo> hotspots; // list of hotspots within the source file
    // status of hotspot info checked by the radio buttons
    bool percentChecked;
    bool callChecked;
    bool durationChecked;

};

#endif // CODEEDITOR_HPP
