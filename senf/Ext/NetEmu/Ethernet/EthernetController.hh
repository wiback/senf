// $Id:$
//
// Copyright (C) 2011
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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

/** \file
    \brief WLANController public header */

#ifndef HH_SENF_Ext_NetEmu_Ethernet_EthernetController_
#define HH_SENF_Ext_NetEmu_Ethernet_EthernetController_ 1

// Custom includes
#include <senf/Socket/NetdeviceController.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    /** \brief Control ethernet interfaces

     */
    class EthernetController
        : public NetdeviceController
    {
    public:
        enum Duplex_mode { HALF_DUPLEX, FULL_DUPLEX, UNKNOWN_DUPLEX } ;

        explicit EthernetController(std::string const & interface_name);
        explicit EthernetController(int interface_index);

        unsigned speed() const;
        Duplex_mode duplex() const;
        std::string duplexAsString() const;
        static std::string duplexAsString(Duplex_mode mode);
        bool hasLink() const;
        bool nWayReset();
        bool ringSize(std::uint32_t rx, std::uint32_t tx);
        std::pair<std::uint32_t,std::uint32_t> ringSize();
        bool setEEE(bool on);
        bool getEEE();
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "EthernetController.cci"
//#include "EthernetController.ct"
//#include "EthernetController.cti"
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
