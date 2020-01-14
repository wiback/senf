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
    \brief CloneSource public header */

#ifndef HH_SENF_PPI_CloneSource_
#define HH_SENF_PPI_CloneSource_ 1

// Custom includes
#include "Module.hh"
#include "Connectors.hh"

//#include "CloneSource.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Generate clone's of a template packet

        CloneSource will provide clone's of a template \a packet on it's \a output.

        \ingroup io_modules
     */
    class CloneSource
        : public Module
    {
        SENF_PPI_MODULE(CloneSource);

    public:
        connector::PassiveOutput<> output;

        CloneSource(senf::Packet const & packet);
        void replacePacket(senf::Packet const & packet);

    private:
        void request();

        senf::Packet packet_;
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "CloneSource.cci"
//#include "CloneSource.ct"
//#include "CloneSource.cti"
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
