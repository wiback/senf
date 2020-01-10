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
    \brief IdleEvent public header */

#ifndef HH_SENF_Scheduler_IdleEvent_
#define HH_SENF_Scheduler_IdleEvent_ 1

// Custom includes
#include <boost/function.hpp>
#include <boost/intrusive/list_hook.hpp>
#include "FIFORunner.hh"

//#include "IdleEvent.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace scheduler {

namespace detail {
    struct IdleEventListTag;
    typedef boost::intrusive::list_base_hook< boost::intrusive::tag<IdleEventListTag> > IdleEventListBase;
    class IdleEventDispatcher;
}

    class IdleEvent
        : public detail::FIFORunner::TaskInfo,
          public detail::IdleEventListBase
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void ()> Callback;

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        IdleEvent(std::string const & name, Callback const & cb,
                  bool initiallyEnabled = true);
        ~IdleEvent();

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

        friend class detail::IdleEventDispatcher;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "IdleEvent.cci"
//#include "IdleEvent.ct"
//#include "IdleEvent.cti"
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
