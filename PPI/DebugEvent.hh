// $Id$
//
// Copyright (C) 2007
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
    \brief DebugEvent public header */

#ifndef HH_DebugEvent_
#define HH_DebugEvent_ 1

// Custom includes
#include "Events.hh"

//#include "DebugEvent.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void trigger();                 ///< Trigger event

    protected:

    private:
        virtual void v_enable();
        virtual void v_disable();
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
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
