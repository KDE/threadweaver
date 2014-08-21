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
