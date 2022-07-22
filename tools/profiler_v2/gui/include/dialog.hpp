/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QErrorMessage;
QT_END_NAMESPACE

class DialogOptionsWidget;

class Dialog : public QWidget
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);

private slots:
    /*void setInteger();
    void setDouble();
    void setItem();
    void setText();
    void setMultiLineText();
    void setColor();
    void setFont();
    */
    void setExistingDirectory();
    /*void setOpenFileName();
    void setOpenFileNames();
    */
    void setExecFileName();
    /*void criticalMessage();
    void informationMessage();
    void questionMessage();
    void warningMessage();
    void errorMessage();
    */
private:
    /*QLabel *integerLabel;
    QLabel *doubleLabel;
    QLabel *itemLabel;
    QLabel *textLabel;
    QLabel *multiLineTextLabel;
    QLabel *colorLabel;
    QLabel *fontLabel;
    */
    QLabel *directoryLabel;
    /*QLabel *openFileNameLabel;
    QLabel *openFileNamesLabel;
    */
    QLabel *execFileNameLabel;

    /*QLabel *criticalLabel;
    QLabel *informationLabel;
    QLabel *questionLabel;
    QLabel *warningLabel;
    QLabel *errorLabel;
    QErrorMessage *errorMessageDialog;
    */
    DialogOptionsWidget *fileDialogOptionsWidget;
    /*DialogOptionsWidget *colorDialogOptionsWidget;
    DialogOptionsWidget *fontDialogOptionsWidget;
    QString openFilesPath;
    */
};

#endif

