#include <QHBoxLayout>
#include <QVBoxLayout>

#include "splitterdesign.hpp"

void SplitterDesign::decorateSplitter(QSplitter* splitter, int index)
{
    Q_ASSERT(splitter != NULL);

    const int gripLength = 15; 
    const int gripWidth = 1;
    const int grips = 3;

    splitter->setOpaqueResize(false);
    splitter->setChildrenCollapsible(false);

    splitter->setHandleWidth(7);
    QSplitterHandle* handle = splitter->handle(index);
    Qt::Orientation orientation = splitter->orientation();
    QHBoxLayout* layout = new QHBoxLayout(handle);
    layout->setSpacing(0);
    layout->setMargin(0);

    if (orientation == Qt::Horizontal)
    {
        for (int i=0;i<grips;++i)
        {
            QFrame* line = new QFrame(handle);
            line->setMinimumSize(gripWidth, gripLength);
            line->setMaximumSize(gripWidth, gripLength);
            line->setFrameShape(QFrame::StyledPanel);
            layout->addWidget(line);
        }
    }
    else
    {
        //this will center the vertical grip
        //add a horizontal spacer
        layout->addStretch();
        //create the vertical grip
        QVBoxLayout* vbox = new QVBoxLayout;
        for (int i=0;i<grips;++i)
        {
            QFrame* line = new QFrame(handle);
            line->setMinimumSize(gripLength, gripWidth);
            line->setMaximumSize(gripLength, gripWidth);
            line->setFrameShape(QFrame::StyledPanel);
            vbox->addWidget(line);
        }
        layout->addLayout(vbox);
        //add another horizontal spacer
        layout->addStretch();
    }
}