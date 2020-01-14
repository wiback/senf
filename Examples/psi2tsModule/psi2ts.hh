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
    \brief psi2ts.hh public header */

#ifndef HH_SENF_Examples_psi2tsModule_psi2ts_
#define HH_SENF_Examples_psi2tsModule_psi2ts_ 1

// Custom includes
#include <senf/PPI/PPI.hh>
#include <senf/PPI/Module.hh>
#include <senf/PPI/Connectors.hh>
#include <senf/Packets/PacketData.hh>
#include <senf/Packets/MPEGDVBBundle/TransportPacket.hh>

//#include "psi2ts.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

class Psi2TsModule
    : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(Psi2TsModule);

public:
    senf::ppi::connector::PassiveInput<> input;
    senf::ppi::connector::ActiveOutput<senf::TransportPacket> output;
    Psi2TsModule(unsigned pid, senf::ClockService::clock_type timout=0);
    void onRequest();

private:
    enum state {IDLE, PROC, WAIT};
    typedef senf::PacketData::iterator iterator;
    unsigned continuity_counter_;
    unsigned next_continuity_counter();
    state state_;
    unsigned pid_;
    senf::ClockService::clock_type timeout_;
};


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "psi2ts.cci"
//#include "psi2ts.ct"
//#include "psi2ts.cti"
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
