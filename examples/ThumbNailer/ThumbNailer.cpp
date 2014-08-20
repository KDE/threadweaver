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

#include <QtCore>
#include <QtTest/QTest>
#include <QFileInfoList>

#include <ThreadWeaver/ThreadWeaver>

#include <Model.h>

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
                qDebug() << "Overall progress:" << model.progress().first << "/" << model.progress().second;
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
    if (true) {
        // benchmark mode
        QApplication app(argc, argv);
        Benchmark benchmark;
        return QTest::qExec(&benchmark, argc, argv);
    } else {
        // demo mode
        QCoreApplication app(argc, argv);
        Queue::instance()->finish();
    }
}

#include "ThumbNailer.moc"
