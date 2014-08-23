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

#include <QFileDialog>
#include <QApplication>
#include <QCloseEvent>
#include <QSettings>
#include <QVariant>
#include <QString>
#include <QSortFilterProxyModel>

#include <ThreadWeaver/ThreadWeaver>

#include "MainWindow.h"
#include "Model.h"
#include "ItemDelegate.h"

#include "ui_MainWindow.h"

const QString MainWindow::Setting_OpenLocation = QLatin1String("OpenFilesLocation");
const QString MainWindow::Setting_OutputLocation = QLatin1String("OutputLocation");
const QString MainWindow::Setting_WindowState = QLatin1String("WindowState");
const QString MainWindow::Setting_WindowGeometry = QLatin1String("WindowGeometry");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_filter(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);
    m_filter->setSortRole(Model::Role_SortRole);
    m_filter->setSourceModel(&m_model);
    m_filter->setDynamicSortFilter(true);
    m_filter->sort(0, Qt::AscendingOrder);
    ui->listView->setModel(m_filter);
    ui->listView->setItemDelegate(new ItemDelegate(this));
    ui->fileLoaderCap->setValue(m_model.fileLoaderCap());
    connect(ui->actionOpen_Files, SIGNAL(triggered()), SLOT(slotOpenFiles()));
    connect(ui->outputDirectory, SIGNAL(clicked()), SLOT(slotSelectOutputDirectory()));
    connect(ui->actionQuit, SIGNAL(triggered()), SLOT(slotQuit()));
    connect(&m_model, SIGNAL(progress(int,int)), SLOT(slotProgress(int,int)));
    connect(ui->fileLoaderCap, SIGNAL(valueChanged(int)), SLOT(slotFileLoaderCapChanged()));

    QSettings settings;
    m_outputDirectory = settings.value(Setting_OutputLocation).toString();
    if (!m_outputDirectory.isEmpty()) {
        ui->outputDirectory->setText(m_outputDirectory);
    }
    restoreGeometry(settings.value(Setting_WindowGeometry).toByteArray());
    restoreState(settings.value(Setting_WindowState).toByteArray());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    QMainWindow::closeEvent(event);
    slotQuit();
}

void MainWindow::slotProgress(int step, int total)
{
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(step);
}

void MainWindow::slotOpenFiles()
{
    QSettings settings;
    const QString previousLocation = settings.value(Setting_OpenLocation, QDir::homePath()).toString();
    auto const files = QFileDialog::getOpenFileNames(this, tr("Select images to convert"), previousLocation);
    if (files.isEmpty()) return;
    if (m_outputDirectory.isNull()) {
        slotSelectOutputDirectory();
    }
    m_model.clear();
    const QFileInfo fi(files.at(0));
    settings.setValue(Setting_OpenLocation, fi.absolutePath());
    m_model.queueUpConversion(files, m_outputDirectory);
}

void MainWindow::slotSelectOutputDirectory()
{
    QSettings settings;
    const QString previousLocation = settings.value(Setting_OutputLocation, QDir::homePath()).toString();
    auto directory = QFileDialog::getExistingDirectory(this, tr("Select output directory..."));
    if (directory.isNull()) return;
    m_outputDirectory = directory;
    settings.setValue(Setting_OutputLocation, directory);
    ui->outputDirectory->setText(directory);
}

void MainWindow::slotFileLoaderCapChanged()
{
    const int value = ui->fileLoaderCap->value();
    Q_ASSERT(value > 0 && value <= 1000); // limits set in UI file
    m_model.setFileLoaderCap(value);
}

void MainWindow::slotQuit()
{
    QSettings settings;
    settings.setValue(Setting_WindowGeometry, saveGeometry());
    settings.setValue(Setting_WindowState, saveState());
    ThreadWeaver::Queue::instance()->dequeue();
    ThreadWeaver::Queue::instance()->finish();
    QApplication::instance()->quit();
}
