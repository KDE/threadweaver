/* -*- C++ -*-
    This file implements the state handling in ThreadWeaver.

    SPDX-FileCopyrightText: 2005-2013 Mirko Boehm <mirko@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later

    $Id: State.h 32 2005-08-17 08:38:01Z mirko $
*/

#ifndef THREADWEAVER_STATE_H
#define THREADWEAVER_STATE_H

#include <QString>

#include "queueinterface.h"
#include "queuesignals.h"
#include "threadweaver_export.h"
#include "weaverinterface.h"

namespace ThreadWeaver
{
class Job;
class Thread;

/*!
 * \relates ThreadWeaver::State
 *
 * All weaver objects maintain a state of operation which can be
 * queried by the application. See the threadweaver documentation on
 * how the different states are related.
 *
 * \value InConstruction
 *        The object is in the state of construction and has not yet
 *        started to process jobs.
 * \value WorkingHard
 *        Jobs are being processed.
 * \value Suspending
 *        Job processing is suspended, but some jobs which where already
 *        in progress are not finished yet.
 * \value Suspended
 *        Job processing is suspended, and no jobs are being
 *        processed.
 * \value ShuttingDown
 *        The object is being destructed. Jobs might still be processed,
 *        the destructor will wait for all threads to exit and then
 *        end.
 * \value Destructed
 *        The object is being destructed, and all threads have
 *        exited. No jobs are handled anymore.
 * \value NoOfStates
 *        Not a state, but a sentinel for the number of defined states.
 */
enum StateId {
    InConstruction = 0,
    WorkingHard,
    Suspending,
    Suspended,
    ShuttingDown,
    Destructed,
    NoOfStates,
};

/*!
 * \class ThreadWeaver::State
 * \inheaderfile ThreadWeaver/State
 * \inmodule ThreadWeaver
 *
 * \brief We use a State pattern to handle the system state in ThreadWeaver.
 */
class THREADWEAVER_EXPORT State : public QueueInterface, public WeaverInterface
{
public:
    /*! Default constructor. */
    explicit State(QueueSignals *weaver);

    /*! Destructor. */
    ~State() override;

    /*! The ID of the current state.
     *  \sa StateID
     */
    QString stateName() const;

    /*! The state Id. */
    virtual StateId stateId() const = 0;

    /*! The state has been changed so that this object is responsible for
     *  state handling. */
    virtual void activated();

protected:
    /*! The Weaver interface this state handles. */
    virtual QueueInterface *weaver();
    /*!
     */
    virtual const QueueInterface *weaver() const;

private:
    class Private;
    Private *const d;
};

}

#endif // THREADWEAVER_STATE_H
