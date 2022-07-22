#ifndef PCCHECKBOX_HPP
#define PCCHECKBOX_HPP

#include <QWidget>
#include <QCheckBox>
#include <QGroupBox>

// this class is a box with QCheckBox buttons for each pc signal (fc & pe0 to pe9)
// in order to give the user the choice to allow to view each Pc signal or not
// in the tlgview.

class PcCheckBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit PcCheckBox(QWidget *parent = nullptr);
    QCheckBox * fcButton;
    std::array<QCheckBox*, 9> pcButtons;

private:
    const int pcNb = 9;
};

#endif // PCCHECKBOX_HPP
