#include <QFileDialog>
#include <QApplication>
#include <QCloseEvent>

#include <ThreadWeaver/ThreadWeaver>

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen_Files, SIGNAL(triggered()), SLOT(slotOpenFiles()));
    connect(ui->outputDirectory, SIGNAL(clicked()), SLOT(slotSelectOutputDirectory()));
    connect(ui->actionQuit, SIGNAL(triggered()), SLOT(slotQuit()));
    connect(&m_model, SIGNAL(progress(int,int)), SLOT(slotProgress(int,int)));
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
    auto const files = QFileDialog::getOpenFileNames(this, tr("Select images to convert"));
    if (m_outputDirectory.isNull()) {
        slotSelectOutputDirectory();
    }
    m_model.clear();
    m_model.queueUpConversion(files, m_outputDirectory);
}

void MainWindow::slotSelectOutputDirectory()
{
    auto directory = QFileDialog::getExistingDirectory(this, tr("Select output directory..."));
    if (directory.isNull()) return;
    m_outputDirectory = directory;
    ui->outputDirectory->setText(directory);
}

void MainWindow::slotQuit()
{
    ThreadWeaver::Queue::instance()->finish();
    QApplication::instance()->quit();
}
