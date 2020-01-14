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
    \brief DiscardSink public header */

#ifndef HH_SENF_PPI_DiscardSink_
#define HH_SENF_PPI_DiscardSink_ 1

// Custom includes
#include "Connectors.hh"
#include "Module.hh"

//#include "DiscardSink.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Module discarding all received packets

        DiscardSink will accept any number of packets and will silently discard them.

        \ingroup io_modules
      */
    class DiscardSink
        : public Module
    {
        SENF_PPI_MODULE(DiscardSink);
    public:
        connector::PassiveInput<> input;

        DiscardSink();

    private:
        void request();
    };


}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "DiscardSink.cci"
//#include "DiscardSink.ct"
//#include "DiscardSink.cti"
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
