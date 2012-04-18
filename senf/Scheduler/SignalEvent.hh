// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief SignalDispatcher public header */

#ifndef HH_SENF_Scheduler_SignalEvent_
#define HH_SENF_Scheduler_SignalEvent_ 1

// Custom includes
#include <signal.h>
#include <boost/function.hpp>
#include "FIFORunner.hh"
#include <boost/intrusive/set_hook.hpp>

//#include "SignalEvent.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class Scheduler;

namespace scheduler {

    namespace detail {
        struct SignalSetTag;
        typedef boost::intrusive::set_base_hook< boost::intrusive::tag<SignalSetTag> > SignalSetBase;
        struct SignalSetCompare;
        struct FindNumericSignal;
        class SignalDispatcher;
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
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (siginfo_t const &)> Callback;
                                        ///< Callback type

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

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

        //\}
        //-////////////////////////////////////////////////////////////////////////

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

        friend struct detail::SignalSetCompare;
        friend struct detail::FindNumericSignal;
        friend class detail::SignalDispatcher;
    };


}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
