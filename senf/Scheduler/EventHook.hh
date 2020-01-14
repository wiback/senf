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
    \brief EventHook public header */

#ifndef HH_SENF_Scheduler_EventHook_
#define HH_SENF_Scheduler_EventHook_ 1

// Custom includes
#include <boost/function.hpp>
#include <boost/intrusive/list_hook.hpp>
#include "FIFORunner.hh"

//#include "EventHook.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace scheduler {

    namespace detail {
        struct EventHookListTag;
        typedef boost::intrusive::list_base_hook< boost::intrusive::tag<EventHookListTag> > EventHookListBase;
        class EventHookDispatcher;
    }

    /** \brief Event hook event

        This event is special: It is not a real event, it is a kind of hook which is called,
        whenever any other event is signaled. Combining this with explicit priority specification,
        this can be used to implement hooks which are called before or after any other callback.

        \code
        void beforeEventHook();
        void afterEventHook();

        senf::scheduler::EventHook beforeEventHookEvent (
            "beforeEventHook", beforeEventHook, senf::scheduler::EventHook::POST);
        senf::scheduler::EventHook afterEventHookEvent (
            "afterEventHook", afterEventHook, senf::scheduler::EventHook::PRE);
        \endcode

        The EventHook class is an implementation of the RAII idiom: The event will be automatically
        unregistered in the EventHook destructor. The EventHook instance should be created within
        the same scope or on a scope below where the callback is defined (e.g. if the callback is a
        member function it should be defined as a class member).
      */
    class EventHook
        : public detail::FIFORunner::TaskInfo,
          public detail::EventHookListBase
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void ()> Callback;

        static Priority const PRE = PRIORITY_HIGH; ///< Execute hook BEFORE all other events
        static Priority const POST = PRIORITY_LOW; ///< Execute hook AFTER all other events

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        EventHook(std::string const & name, Callback const & cb,
                  Priority priority, bool initiallyEnabled = true);
                                        ///< Register an event hook
                                        /**< Registers \a cb to be called whenever any other event
                                             is signaled by the scheduler. If \a initiallyEnabled is
                                             set \c false, the callback will not be enabled
                                             automatically. Use enable() to do so.
                                             \param[in] name Descriptive event name (purely
                                                 informational)
                                             \param[in] cb Callback to call
                                             \param[in] initiallyEnabled if set \c false, do not
                                                 enable callback automatically.
                                             \param[in] priority event priority, defaults to
                                                 POST */
        ~EventHook();

        //\}
        //-////////////////////////////////////////////////////////////////////////

        void disable();                 ///< Disable event
        void enable();                  ///< Enable event

        void action(Callback const & cb); ///< Change event callback

    protected:

    private:
        virtual void v_run();
        virtual char const * v_type() const;
        virtual std::string v_info() const;

        Callback cb_;

        friend class detail::EventHookDispatcher;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "EventHook.cci"
//#include "EventHook.ct"
//#include "EventHook.cti"
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
