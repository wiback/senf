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
    \brief DebugEvent public header */

#ifndef HH_SENF_PPI_DebugEvent_
#define HH_SENF_PPI_DebugEvent_ 1

// Custom includes
#include "Events.hh"

//#include "DebugEvent.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Manually triggered event

        DebugEvent is a manually triggered event. The event is triggered by explicit calls to
        trigger() and not by the event infrastructure.
      */
    class DebugEvent
        : public EventImplementation<>
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor
        // no conversion constructors

        //\}
        //-////////////////////////////////////////////////////////////////////////

        void trigger();                 ///< Trigger event

    protected:

    private:
        virtual void v_enable();
        virtual void v_disable();
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "DebugEvent.cci"
//#include "DebugEvent.ct"
//#include "DebugEvent.cti"
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
