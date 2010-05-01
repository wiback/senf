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
    \brief ThrottleBarrier public header */

#ifndef HH_SENF_PPI_ThrottleBarrier_
#define HH_SENF_PPI_ThrottleBarrier_ 1

// Custom includes
#include "Connectors.hh"
#include "Module.hh"

//#include "ThrottleBarrier.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Packet discarding barrier for throttle notifications

        A ThrottleBarrier will \e not forward any throttle notifications received on it's output. It
        will instead discard any packet received on it's input while the output is throttled. This
        module is used to circumvent any operating system based queueing. This allows the queueing
        to be controlled explicitly within the PPI.

        \ingroup routing_modules
     */
    class ThrottleBarrier
        : public Module
    {
        SENF_PPI_MODULE(ThrottleBarrier);
    public:

        connector::PassiveInput<> input;
        connector::ActiveOutput<> output;

        ThrottleBarrier();

    private:
        void request();
    };


}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ThrottleBarrier.cci"
//#include "ThrottleBarrier.ct"
//#include "ThrottleBarrier.cti"
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
