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
