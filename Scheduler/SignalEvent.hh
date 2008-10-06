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
#include <boost/function.hpp>
#include "FIFORunner.hh"
#include "../boost/intrusive/iset_hook.hpp"

//#include "SignalEvent.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    class Scheduler;

namespace scheduler {

    namespace detail {
        struct SignalSetTag;
        typedef boost::intrusive::iset_base_hook<SignalSetTag> SignalSetBase;
        struct SignalSetCompare;
        struct FindNumericSignal;
        struct SignalDispatcher;
    }

    /** \brief UNIX signal event

        The SignalEvent class registers a callback for UNIX signals. The callback will be called \e
        synchronously (not from within the UNIX signal handler) by the scheduler.

        The SignalEvent class is an implementation of the RAII idiom: The event will be
        automatically unregistered in the SignalEvent destructor. The SignalEvent instance should be
        created within the same scope or on a scope below where the callback is defined (e.g. if the
        callback is a member function it should be defined as a class member).
     */
    class SignalEvent
        : public detail::FIFORunner::TaskInfo,
          public detail::SignalSetBase 
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (siginfo_t const &)> Callback;
                                        ///< Callback type

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        SignalEvent(int signal, Callback const & cb, bool initiallyEnabled=true);
                                        ///< Register a signal event
                                        /**< Registers \a cb as callback for the UNIX signal \a
                                             signal. If \a initiallyEnabled is set \c false, the
                                             callback will not be enabled automatically. Use
                                             enable() to do so.
                                             \param[in] signal UNIX signal to register callback for
                                             \param[in] cb Callback to call
                                             \param[in] initiallyEnabled if set \c false, do not
                                                 enable callback automatically. */
        ~SignalEvent();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void disable();                 ///< Enable signal event registration
        void enable();                  ///< Disable the signal event registration

        void action(Callback const & cb); ///< Change signal event callback

    private:
        virtual void v_run();
        virtual char const * v_type() const;
        virtual std::string v_info() const;
        
        int signal_;
        Callback cb_;
        siginfo_t siginfo_;

        friend class detail::SignalSetCompare;
        friend class detail::FindNumericSignal;
        friend class detail::SignalDispatcher;
    };


}}

///////////////////////////////hh.e////////////////////////////////////////
#include "SignalEvent.cci"
//#include "SignalEvent.ct"
//#include "SignalEvent.cti"
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
