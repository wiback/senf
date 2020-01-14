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
    \brief EthernetInterface public header */

#ifndef HH_SENF_Ext_NetEmu_Ethernet_EthernetInterface_
#define HH_SENF_Ext_NetEmu_Ethernet_EthernetInterface_ 1

// Custom includes
#include "senf/Ext/NetEmu/InterfaceAPIBase.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class EthernetInterface
        : public BidirectionalWiredInterface
    {
    public:
        template <class Output, class Input>
        EthernetInterface(Output & output, Input & input);

    protected:
        void init();

    private:
        virtual std::uint8_t v_linkTypeId() const;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "EthernetInterface.cci"
//#include "EthernetInterface.ct"
#include "EthernetInterface.cti"
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
