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

class ImageListFilter;

namespace Ui {
class MainWindow;
}

class AverageLoadManager;

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
    void slotFileLoaderCapChanged();
    void slotImageLoaderCapChanged();
    void slotComputeThumbNailCapChanged();
    void slotSaveThumbNailCapChanged();
    void slotWorkerCapChanged();
    void slotEnableAverageLoadManager(bool);
    void slotRecommendedWorkerCountChanged(int);
    void slotQuit();

private:
    Ui::MainWindow *ui;
    QString m_outputDirectory;
    Model m_model;
    ImageListFilter* m_fileLoaderFilter;
    ImageListFilter* m_imageLoaderFilter;
    ImageListFilter* m_imageScalerFilter;
    ImageListFilter* m_imageWriterFilter;
    AverageLoadManager* m_averageLoadManager;

    static const QString Setting_OpenLocation;
    static const QString Setting_OutputLocation;
    static const QString Setting_WindowState;
    static const QString Setting_WindowGeometry;
};

#endif // MAINWINDOW_H
