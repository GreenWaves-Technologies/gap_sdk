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
