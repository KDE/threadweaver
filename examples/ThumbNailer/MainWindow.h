/* -*- C++ -*-

   This file is part of ThreadWeaver.

   Author: Mirko Boehm
   Copyright: (C) 2005-2014 Mirko Boehm
   Contact: mirko@kde.org
         http://www.kde.org
         http://creative-destruction.me

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Model.h"

class QSortFilterProxyModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent*);

public Q_SLOTS:
    void slotProgress(int step, int total);

private Q_SLOTS:
    void slotOpenFiles();
    void slotSelectOutputDirectory();
    void slotQuit();

private:
    Ui::MainWindow *ui;
    QString m_outputDirectory;
    Model m_model;
    QSortFilterProxyModel* m_filter;

    static const QString Setting_OpenLocation;
    static const QString Setting_OutputLocation;
};

#endif // MAINWINDOW_H
