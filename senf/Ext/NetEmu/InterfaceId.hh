// $Id:$
//
// Copyright (C) 2012
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//       Thorsten Horstmann <Thorsten.Horstmann@fokus.fraunhofer.de>

/** \file
    \brief InterfaceId public header */

#ifndef HH_SENF_Ext_NetEmu_InterfaceId_
#define HH_SENF_Ext_NetEmu_InterfaceId_ 1

// Custom includes
#include <set>
#include <senf/Utils/Console/ParsedCommand.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    struct MACAddress;

namespace emu {

    class InterfaceDeviceId
    {
    public:
        enum Bus_t { Unknown = 0, PCI = 1, PCIe, USB };

        InterfaceDeviceId();
        InterfaceDeviceId(Bus_t b, boost::uint16_t v, boost::uint16_t d);
        InterfaceDeviceId(boost::uint8_t b, boost::uint16_t v, boost::uint16_t d);

        static const InterfaceDeviceId None;

        static InterfaceDeviceId get(senf::MACAddress const & addr);

        bool operator<(InterfaceDeviceId const & other) const;
        bool operator==(InterfaceDeviceId const & other) const;

        Bus_t bus() const;
        boost::uint16_t vendor() const;
        boost::uint16_t device() const;

    private:
        Bus_t bus_;
        boost::uint16_t vendor_;
        boost::uint16_t device_;

        SENF_CONSOLE_PARSE_FRIEND( InterfaceDeviceId );
    };

    std::ostream & operator<<(std::ostream & os, InterfaceDeviceId const & id);

    SENF_CONSOLE_REGISTER_ENUM_MEMBER( InterfaceDeviceId, Bus_t, (Unknown)(PCI)(PCIe)(USB) );


    class InterfaceIdFactoryBase
    {
    public:
        virtual ~InterfaceIdFactoryBase() {};

        MACAddress getId();
        void releaseId(MACAddress const & id);

    private:
        virtual MACAddress v_getId() = 0;
        virtual void v_releaseId(MACAddress const & id) = 0;
    };


    class SimpleInterfaceIdFactory
        : public InterfaceIdFactoryBase
    {
    private:
        MACAddress v_getId();
        void v_releaseId(MACAddress const & id);

        boost::uint32_t calcFirstFreeId();

        typedef std::set<boost::uint16_t> IdSet;
        IdSet idSet_;
    };

}};

SENF_CONSOLE_PARSE_AND_FORMAT_AS_TUPLE( senf::emu::InterfaceDeviceId, (bus_)(vendor_)(device_) );


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "InterfaceId.cci"
//#include "InterfaceId.ct"
//#include "InterfaceId.cti"
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
