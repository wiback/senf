//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
