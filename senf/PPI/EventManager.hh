//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief EventManager public header */

#ifndef HH_SENF_PPI_EventManager_
#define HH_SENF_PPI_EventManager_ 1

// Custom includes
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <senf/Scheduler/ClockService.hh>
#include "predecl.hh"
#include "detail/Callback.hh"
#include "detail/EventBinding.hh"

//#include "EventManager.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Event registry and control

        The EventManager control event registration and manages global event parameters. The
        EventManager controls event dispatch but does \e not control event generation. This is the
        responsibility of an external component (the Scheduler)
      */
    class EventManager
        : boost::noncopyable
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

#ifndef DOXYGEN
        // Somehow doxygen barfs on this definition
        template <class Descriptor>
        struct Callback
            : public detail::Callback<typename Descriptor::EventArg>
        {};
#endif

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        static EventManager & instance();
        static bool alive();

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // no conversion constructors

        //\}
        //-////////////////////////////////////////////////////////////////////////

        ClockService::clock_type const & now(); ///< Current time at last event dispatch
        ClockService::clock_type const & time(); ///< Expected time of the last event

    protected:

    private:
        EventManager();
        ~EventManager();

        template <class Descriptor>
        void registerEvent(module::Module & module,
                           typename Callback<Descriptor>::type callback,
                           Descriptor & descriptor);

        void destroyModule(module::Module & module);
        void destroyEvent(EventDescriptor & event);

        typedef boost::ptr_vector<detail::EventBindingBase> EventRegistrations;
        EventRegistrations registrations_;

        void eventTime(ClockService::clock_type const & time);

        ClockService::clock_type eventTime_;

        static bool alive_;

        friend class detail::EventBindingBase;
        friend class module::Module;
        friend class EventDescriptor;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "EventManager.cci"
#include "EventManager.ct"
//#include "EventManager.cti"
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
