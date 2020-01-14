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
    \brief ForwardSwitch public header */

#ifndef HH_SENF_Ext_NetEmu_DVB_ForwardSwitch_
#define HH_SENF_Ext_NetEmu_DVB_ForwardSwitch_ 1

// Custom includes
#include <senf/PPI/Module.hh>

///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace emu {

    class ForwardSwitch
        : public ppi::module::Module
    {
        SENF_PPI_MODULE(ForwardSwitch);

    public:
        ForwardSwitch(bool state = true);

        bool forward() const;
        void forward(bool state);

        ppi::connector::PassiveInput<EthernetPacket> input;
        ppi::connector::ActiveOutput<EthernetPacket> output;

    private:
        void onRequest_discard();
        void onRequest_forward();
        bool forward_;
    };

}}
///////////////////////////////hh.e////////////////////////////////////////
#include "ForwardSwitch.cci"
//#include "ForwardSwitch.ct"
//#include "ForwardSwitch.cti"
#endif
