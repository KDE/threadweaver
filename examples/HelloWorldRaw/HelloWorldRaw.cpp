/* -*- C++ -*-

   This file is part of ThreadWeaver.

   Author: Mirko Boehm
   Copyright: (C) 2005-2014 Mirko Boehm
   Contact: mirko@kde.org
         https://www.kde.org
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
#include <ThreadWeaver/ThreadWeaver>

using namespace ThreadWeaver;

//@@snippet_begin(sample-helloworldraw-class)
class QDebugJob : public Job {
public:
    QDebugJob(const char* message = nullptr) : m_message(message) {}
protected:
    void run(JobPointer, Thread*) override {
        qDebug() << m_message;
    }
private:
    const char* m_message;
};
//@@snippet_end

//@@snippet_begin(sample-helloworldraw-main)
int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    // Allocate jobs as local variables:
    QDebugJob j1("Hello");
    QDebugJob j2("World!");
    JobPointer j3(new QDebugJob("This is..."));    
    Job* j4 = new QDebugJob("ThreadWeaver!");
    // Queue the Job using the default Queue stream:
    stream() << j1 << j2 // local variables
	     << j3 // a shared pointer
	     << j4; // a raw pointer
    // Wait for finish(), because job is destroyed
    // before the global queue:
    Queue::instance()->finish();
}
//@@snippet_end
