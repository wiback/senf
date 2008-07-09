// $Id$
//
// Copyright (C) 2008 
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief SignalDispatcher public header */

#ifndef HH_SignalDispatcher_
#define HH_SignalDispatcher_ 1

// Custom includes
#include <signal.h>
#include <map>
#include "FdManager.hh"
#include "FIFORunner.hh"

//#include "SignalDispatcher.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

    /** \brief Scheduler dispatcher managing UNIX signals

        This dispatcher supports registering UNIX signals with the Scheduler.

        \implementation SignalDispatcher provides a single signal handler which all registered
            signals are assigned to. When a signal is received, data is written to a pipe which has
            been added to the FdManager and this signals the event.

        \todo Add signal block/unblock management to the FdManager to reduce the number of
            setprocmask() calls
      */
    class SignalDispatcher
        : public FdManager::Event
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (siginfo_t const &)> Callback;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        SignalDispatcher(FdManager & manager, FIFORunner & runner);
        ~SignalDispatcher();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void add(int signal, Callback const & cb); ///< Add signal event
                                        /**< \param[in] signal signal number
                                             \param[in] cb Callback */

        void remove(int signal); ///< Unregister signal event

        void unblockSignals();          ///< Unblock registered signals
                                        /**< Must be called before waiting for an event */
        void blockSignals();            ///< Block registered signals
                                        /**< Must be called directly after FdManager returns */

        bool empty() const;             ///< \c true, if no signal is registered.

    protected:

    private:
        ///< Internal: UNIX signal event
        struct SignalEvent
            : public FIFORunner::TaskInfo
        {
            SignalEvent(int signal, Callback cb_);
            virtual void run();

            siginfo_t siginfo;
            Callback cb;
        };

        virtual void signal(int events);
        static void sigHandler(int signal, ::siginfo_t * siginfo, void *);

        FdManager & manager_;
        FIFORunner & runner_;

        typedef std::map<int, SignalEvent> HandlerMap;
        HandlerMap handlers_;

        int sigPipe_[2];

        bool blocked_;
        sigset_t sigSet_;

        static SignalDispatcher * instance_;
    };


}}

///////////////////////////////hh.e////////////////////////////////////////
#include "SignalDispatcher.cci"
//#include "SignalDispatcher.ct"
//#include "SignalDispatcher.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
