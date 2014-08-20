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

#include <iostream>

#include <QtCore>
#include <QtTest/QTest>
#include <QFileInfoList>
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <ThreadWeaver/ThreadWeaver>

#include <Model.h>

using namespace std;
using namespace ThreadWeaver;

class Benchmark : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void processThumbNailsAsBenchmarkInLoop() {
        const QFileInfoList files = images();
        {   // create a block to avoid adding the time needed to remove the temporary
            // directory from the file system to the measured time:
            QTemporaryDir temp;
            QBENCHMARK {
                Model model;
                model.prepareConversions(files, temp.path());
                QVERIFY(model.computeThumbNailsBlockingInLoop());
            }
        }
    }

    void processThumbNailsAsBenchmarkWithThreadWeaver() {
        const QFileInfoList files = images();
        {   // create a block to avoid adding the time needed to remove the temporary
            // directory from the file system to the measured time:
            QTemporaryDir temp;
            QBENCHMARK {
                Model model;
                model.prepareConversions(files, temp.path());
                QVERIFY(model.computeThumbNailsBlockingConcurrent());
            }
        }
    }

private:
    const QFileInfoList images() {
        const QDir dir = QDir(QLatin1String("/usr/share/backgrounds"));
        return dir.entryInfoList(QStringList() << QLatin1String("*.jpg"));
    }
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QCommandLineParser parser;
    parser.setApplicationDescription(app.translate("main", "ThreadWeaver ThumbNailer Example"));
    parser.addHelpOption();
    parser.addPositionalArgument(QLatin1String("mode"), QLatin1String("Benchmark or demo mode"));
    parser.process(app);
    const QStringList positionals = parser.positionalArguments();
    const QString mode = positionals.isEmpty() ? QLatin1String("demo") : positionals[0];
    if (mode == QLatin1String("benchmark")) {
        Benchmark benchmark;
        const QStringList arguments = app.arguments().mid(1); // remove mode selection
        return QTest::qExec(&benchmark, arguments);
    } else if (mode == QLatin1String("demo")){
        // demo mode
        qDebug() << "NI: demo mode";
        Queue::instance()->finish();
    } else {
        wcerr << "Unknown mode " << mode.toStdWString() << endl << endl;
        parser.showHelp();
        Q_UNREACHABLE();
    }
    return 0;
}

#include "ThumbNailer.moc"
