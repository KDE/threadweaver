#include <QFileDialog>
#include <QApplication>
#include <QCloseEvent>
#include <QSettings>
#include <QVariant>
#include <QString>
#include <ThreadWeaver/ThreadWeaver>

#include "MainWindow.h"
#include "ui_MainWindow.h"

const QString MainWindow::Setting_OpenLocation = QLatin1String("OpenFilesLocation");
const QString MainWindow::Setting_OutputLocation = QLatin1String("OutputLocation");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen_Files, SIGNAL(triggered()), SLOT(slotOpenFiles()));
    connect(ui->outputDirectory, SIGNAL(clicked()), SLOT(slotSelectOutputDirectory()));
    connect(ui->actionQuit, SIGNAL(triggered()), SLOT(slotQuit()));
    connect(&m_model, SIGNAL(progress(int,int)), SLOT(slotProgress(int,int)));

    m_outputDirectory = QSettings().value(Setting_OutputLocation).toString();
    if (!m_outputDirectory.isEmpty()) {
        ui->outputDirectory->setText(m_outputDirectory);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
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

void MainWindow::slotQuit()
{
    ThreadWeaver::Queue::instance()->finish();
    QApplication::instance()->quit();
}
