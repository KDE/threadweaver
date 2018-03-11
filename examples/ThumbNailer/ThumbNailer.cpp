﻿/* -*- C++ -*-

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

#include <QTest>
#include <QFileInfoList>
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "Model.h"
#include "MainWindow.h"
#include "Benchmark.h"

using namespace std;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QLatin1String("ThumbNailer"));
    app.setOrganizationDomain(QLatin1String("kde.org"));
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
        MainWindow mainWindow;
        mainWindow.show();
        return app.exec();
    } else {
        wcerr << "Unknown mode " << mode.toStdWString() << endl << endl;
        parser.showHelp();
        Q_UNREACHABLE();
    }
    return 0;
}
