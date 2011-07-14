// $Id$
//
// Copyright (C) 2007
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
    \brief ThrottleBarrier public header */

#ifndef HH_SENF_PPI_ThrottleBarrier_
#define HH_SENF_PPI_ThrottleBarrier_ 1

// Custom includes
#include "Connectors.hh"
#include "Module.hh"

//#include "ThrottleBarrier.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
